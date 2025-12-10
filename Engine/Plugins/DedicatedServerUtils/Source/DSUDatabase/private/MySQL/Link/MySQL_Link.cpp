#include "MySQL/Link/MySQL_Link.h"
#include "DSUDatabase/DSUDatabase.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "MySQL/Official/mysql.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"

using namespace DSUDatabase;


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
FMySQL_Link::FMySQL_Link(const FString& InHost, const FString& InUser, const FString& InPassword, const uint32& InPort, 
						 const FString& InUnixSocket, const TArray<EMySQL_ClientFlag>& InClientFlags)
	: Host(InHost)
	, User(InUser)
	, Password(InPassword)
	, Port(InPort)
	, UnixSocket(InUnixSocket)
	, ClientFlags(InClientFlags)
	, MySQL(new MYSQL())
	, MysqlStmt(NULL)
{
	mysql_library_init(0, NULL, NULL);
	mysql_init(MySQL);
	MysqlStmt = mysql_stmt_init(MySQL);
}

FMySQL_Link::~FMySQL_Link()
{
	mysql_stmt_close(MysqlStmt);
	mysql_close(MySQL);
	mysql_library_end();
}

bool FMySQL_Link::ConnectMySQL()
{
	unsigned long Flags = 0;
	for (auto& Tmp : ClientFlags)
	{
		Flags |= (1UL << static_cast<uint8>(Tmp));
	}
	return mysql_real_connect(
		MySQL, 
		TCHAR_TO_UTF8(*Host), 
		TCHAR_TO_UTF8(*User), 
		TCHAR_TO_UTF8(*Password), 
		TCHAR_TO_UTF8(*DB),
		Port, 
		TCHAR_TO_UTF8(*UnixSocket), 
		Flags
	) != nullptr;
}

bool FMySQL_Link::QueryLink(const FString& SQL)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *SQL);
	if (mysql_ping(MySQL) == 0)
	{
		if (mysql_query(MySQL, TCHAR_TO_UTF8(*SQL)) == 0)
		{
			return true;
		}
	}
	else
	{
		if (ConnectMySQL() && mysql_query(MySQL, TCHAR_TO_UTF8(*SQL)) == 0)
		{
			return true;
		}
	}
	UE_LOG(LogDSUDatabase, Error, TEXT("mysql_query failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
	return false;
}

bool FMySQL_Link::QueryLinkStmt(const FString& SQL, const TArray<FMySQL_StmtValue>& InQueryStmtParams)
{
	if (PrepareStmtSQL(SQL) && PrepareStmtParams(InQueryStmtParams))
	{
		return ExecuteStmt();
	}
	return false;
}

bool FMySQL_Link::PrepareStmtSQL(const FString& SQL)
{
	int Ret = mysql_ping(MySQL);
	if (Ret == 0)
	{
		Ret = mysql_stmt_prepare(MysqlStmt, TCHAR_TO_UTF8(*SQL), SQL.Len());
		if (Ret != 0)
		{
			UE_LOG(LogDSUDatabase, Error, TEXT("error: %s"), UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt)));
			return false;
		}
	}
	else
	{
		if (ConnectMySQL())
		{
			Ret = mysql_stmt_prepare(MysqlStmt, TCHAR_TO_UTF8(*SQL), SQL.Len());
			if (Ret != 0)
			{
				UE_LOG(LogDSUDatabase, Error, TEXT("error: %s"), UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt)));
				return false;
			}
		}
		else
		{
			UE_LOG(LogDSUDatabase, Error, TEXT("error: %s"), UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt)));
			return false;
		}
	}
	return true;
}

bool FMySQL_Link::PrepareStmtParams(const TArray<FMySQL_StmtValue>& InQueryStmtParams)
{
	if (InQueryStmtParams.Num() != 0)
	{
		BindAllocation.Free();

		int32 MemorySize = sizeof(MYSQL_BIND) * InQueryStmtParams.Num();
		BindAllocation.Size = InQueryStmtParams.Num();
		BindAllocation.StmtBind = (MYSQL_BIND*)FMemory::Malloc(MemorySize);
		FMemory::Memzero(BindAllocation.StmtBind, MemorySize);

		uint8* Byteode = (uint8*)BindAllocation.StmtBind;
		for (const auto& Tmp : InQueryStmtParams)
		{
			MYSQL_BIND* TmpBind = (MYSQL_BIND*)Byteode;
			TmpBind->buffer_type = (enum_field_types)FieldTypeToVariableType(Tmp.FieldType);
			Tmp.GetValue(&TmpBind->buffer);
			TmpBind->length = Tmp.GetValueTypeLenMemory();
			TmpBind->buffer_length = Tmp.GetValueTypeLen();
			Byteode += sizeof(MYSQL_BIND);
		}

		if (mysql_stmt_bind_param(MysqlStmt, BindAllocation.StmtBind) == 0)
		{
			return true;
		}
		UE_LOG(LogDSUDatabase, Error, TEXT("error: %s"), UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt)));
	}
	return false;
}

bool FMySQL_Link::ExecuteStmt()
{
	if (mysql_stmt_execute(MysqlStmt) != 0)
	{
		BindAllocation.Free();
		UE_LOG(LogDSUDatabase, Error, TEXT("error: %s"), UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt)));
		return false;
	}
	return true;
}

bool FMySQL_Link::CreateDatabase(const FString& DatabaseName, EMySQL_Charset Charset, bool bAutoSelect)
{
	FString SQL = TEXT("CREATE DATABASE IF NOT EXISTS `") + DatabaseName + TEXT("`");
	if (Charset != EMySQL_Charset::None)
	{
		SQL += (TEXT(" DEFAULT CHARSET=") + StaticEnum<EMySQL_Charset>()->GetNameStringByIndex((int32)Charset));
	}
	SQL += +TEXT(";");
	return QueryLink(SQL) && (bAutoSelect ? SelectDatabase(DatabaseName) : true);
}

bool FMySQL_Link::DropDatabase(const FString& DatabaseName)
{
	FString SQL = TEXT("DROP DATABASE `") + DatabaseName + TEXT("`;");
	return QueryLink(SQL);
}

bool FMySQL_Link::SelectDatabase(const FString& DatabaseName)
{
	int32 Ret = mysql_ping(MySQL);
	if (Ret == 0)
	{
		if (mysql_select_db(MySQL, TCHAR_TO_UTF8(*DatabaseName)) == 0)
		{
			DB = DatabaseName;
			return true;
		}
	}
	else
	{
		if (ConnectMySQL())
		{
			if (mysql_select_db(MySQL, TCHAR_TO_UTF8(*DatabaseName)) == 0)
			{
				DB = DatabaseName;
				return true;
			}
		}
	}
	UE_LOG(LogDSUDatabase, Error, TEXT("SelectDatabase failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
	return false;
}

bool FMySQL_Link::CreateTable(const FString& TableName, const TMap<FString, FMySQL_FieldTypeProperties>& InFields, 
							  const TArray<FString>& InPrimaryKeys, const FMySQL_TableOptions& InTableOptions, const bool bIsTemporary)
{
	FString SQL = bIsTemporary ? TEXT("CREATE TEMPORARY TABLE `") : TEXT("CREATE TABLE `");
	SQL += TableName + TEXT("`(");
	for (auto& Tmp : InFields)
	{
		SQL += TEXT("`") + Tmp.Key + TEXT("` ") + Tmp.Value.ToString() + TEXT(",");
	}
	if (InPrimaryKeys.Num() > 0)
	{
		SQL += TEXT("PRIMARY KEY(");
		for (auto& Tmp : InPrimaryKeys)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT("),");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(")");

	SQL += InTableOptions.ToString();

	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::OptimiseTable(const FString& TableName)
{
	// 重构表，回收碎片内存
	FString SQL = TEXT("OPTIMISE TABLE `") + TableName + TEXT("`;");
	return QueryLink(SQL);
}

bool FMySQL_Link::DropTable(const FString& TableName)
{
	// 完全删除，不支持回滚
	FString SQL = TEXT("DROP TABLE `") + TableName + TEXT("`;");
	return QueryLink(SQL);
}

bool FMySQL_Link::TruncateTable(const FString& TableName)
{
	// 直接清空表，重建表结构，无法回滚
	FString SQL = TEXT("TRUNCATE TABLE `") + TableName + TEXT("`;");
	return QueryLink(SQL);
}

bool FMySQL_Link::CreateTableLike(const FString& NewTable, const FString& SourceTable, const bool bIsTemporary)
{
	FString SQL = bIsTemporary ? TEXT("CREATE TEMPORARY TABLE ") : TEXT("CREATE TABLE ");
	SQL += TEXT("`") + NewTable + TEXT("` LIKE `") + SourceTable + TEXT("`;");
	return QueryLink(SQL);
}

bool FMySQL_Link::CopyTable(const FString& NewTable, const FString& SourceTable, const bool bIsTemporary)
{
	FString SQL = bIsTemporary ? TEXT("CREATE TEMPORARY TABLE ") : TEXT("CREATE TABLE ");
	SQL += TEXT("`") + NewTable + TEXT("` LIKE `") + SourceTable + TEXT("`;");
	SQL += TEXT(" INSERT INTO `") + NewTable + TEXT("` SELECT * FROM ") + SourceTable + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::CopyTableSelect(const FString& NewTable, const FString& SourceTable, const TArray<FString>& Fields, const FString& Conditions, 
								  const TMap<FString, bool>& OrdersAndIsDesc, const FMySQL_LimitParams& Limit, const bool bIsDistinct, const bool bIsTemporary)
{
	FString SQL = bIsTemporary ? TEXT("CREATE TEMPORARY TABLE ") : TEXT("CREATE TABLE ");
	SQL += TEXT("`") + NewTable + TEXT("` LIKE `") + SourceTable + TEXT("`;");
	SQL += TEXT(" INSERT INTO `") + NewTable + TEXT("`");
	if (Fields.Num() > 0)
	{
		SQL += TEXT(" (");
		for (auto& Tmp : Fields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(",");
		SQL += TEXT(")");
	}
	SQL += bIsDistinct ? TEXT(" SELECT DISTINCT ") : TEXT(" SELECT ");
	if (Fields.Num() == 0)
	{
		SQL += TEXT("*");
	}
	else
	{
		for (auto& Tmp : Fields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(",");
	}
	SQL += TEXT(" FROM ") + SourceTable;
	if (!Conditions.IsEmpty())
	{
		SQL += TEXT(" WHERE ") + Conditions;
	}
	if (OrdersAndIsDesc.Num() > 0)
	{
		SQL += TEXT(" ORDER BY ");
		for (auto& Tmp : OrdersAndIsDesc)
		{
			SQL += TEXT("`") + Tmp.Key + (Tmp.Value ? TEXT("` DESC,") : TEXT("`,"));
		}
		SQL.RemoveFromEnd(",");
	}
	if (Limit.LimitNum >= 0)
	{
		SQL += Limit.ToString();
	}
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::InsertDataByRows(const FString& TableName, const TArray<TArray<FString>>& Rows, const TArray<FString>& Fields, 
								   const bool bIsIgnore, const bool bIsReplace)
{
	FString SQL = bIsReplace ? TEXT("REPLACE ") : TEXT("INSERT ");
	SQL += bIsIgnore && !bIsReplace ? TEXT("IGNORE INTO ") : TEXT("INTO ");
	SQL += TEXT("`") + TableName + TEXT("`");
	if (Fields.Num() > 0)
	{
		SQL += TEXT(" (");
		for (auto& Tmp : Fields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT(")");
	}
	SQL += TEXT(" VALUES");
	for (int i = 0; i < Rows.Num(); ++i)
	{
		SQL += TEXT(" (");
		for (auto& Tmp : Rows[i])
		{
			SQL += Tmp + TEXT(",");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT("),");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::InsertDataByFields(const FString& TableName, const TMap<FString, TArray<FString>>& DataToInsert, const bool bIsIgnore, const bool bIsReplace)
{
	FString SQL = bIsReplace ? TEXT("REPLACE ") : TEXT("INSERT ");
	SQL += bIsIgnore && !bIsReplace ? TEXT("IGNORE INTO ") : TEXT("INTO ");
	SQL += TEXT("`") + TableName + TEXT("` (");
	for (auto& Tmp : DataToInsert)
	{
		SQL += TEXT("`") + Tmp.Key + TEXT("`,");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(") VALUES");
	for (int i = 0; i < DataToInsert.Num(); ++i)
	{
		SQL += TEXT(" (");
		for (auto& Tmp : DataToInsert)
		{
			SQL += Tmp.Value[i] + TEXT(",");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT("),");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::InsertDataBySelect(const FString& TargetTable, const FString& SourceTable, const TArray<FString>& TargetFields, 
									 const TArray<FString>& SourceFields, const FString& Conditions, const TMap<FString, bool>& OrdersAndIsDesc, 
									 const FMySQL_LimitParams& Limit, const bool bIsDistinct, const bool bIsIgnore, const bool bIsReplace)
{
	FString SQL = bIsReplace ? TEXT("REPLACE ") : TEXT("INSERT ");
	SQL += bIsIgnore && !bIsReplace ? TEXT("IGNORE INTO ") : TEXT("INTO ");
	SQL += TEXT("`") + TargetTable + TEXT("`");
	if (TargetFields.Num() > 0)
	{
		SQL += TEXT(" (");
		for (auto& Tmp : TargetFields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(",");
		SQL += TEXT(")");
	}
	SQL += bIsDistinct ? TEXT(" SELECT DISTINCT ") : TEXT(" SELECT ");
	if (SourceFields.Num() == 0)
	{
		SQL += TEXT("*");
	}
	else
	{
		for (auto& Tmp : SourceFields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(",");
	}
	SQL += TEXT(" FROM ") + SourceTable;
	if (!Conditions.IsEmpty())
	{
		SQL += TEXT(" WHERE ") + Conditions;
	}
	if (OrdersAndIsDesc.Num() > 0)
	{
		SQL += TEXT(" ORDER BY ");
		for (auto& Tmp : OrdersAndIsDesc)
		{
			SQL += Tmp.Key + (Tmp.Value ? TEXT(" DESC,") : TEXT(","));
		}
		SQL.RemoveFromEnd(",");
	}
	if (Limit.LimitNum >= 0)
	{
		SQL += Limit.ToString();
	}
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::DeleteData(const FString& TableName, const FString& Conditions)
{
	// 遍历删除，效率低，可回滚
	FString SQL = TEXT("DELETE FROM `") + TableName + TEXT("`");
	if (!Conditions.IsEmpty())
	{
		// 多个条件需要通过 AND/OR/NOT 连接，通过 () 调整优先级
		SQL += TEXT(" WHERE ") + Conditions;
	}
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::SelectData(TArray<FMySQL_FieldsData>& Results, const FString& TableName, const TArray<FString>& Fields, const TArray<FMySQL_JoinParams>& Joins, 
							 const FString& Conditions, const TArray<FString>& Groups, const FString& Havings, const TMap<FString, bool>& OrdersAndIsDesc, 
							 const FMySQL_LimitParams& Limit, const bool bIsDistinct, const bool bIsStoreResults)
{
	FString SQL = bIsDistinct ? TEXT("SELECT DISTINCT ") : TEXT("SELECT ");
	if (Fields.Num() == 0)
	{
		SQL += TEXT("*");
	}
	else
	{
		for (auto& Tmp : Fields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(",");
	}
	SQL += TEXT(" FROM `") + TableName + TEXT("`");
	for (auto& Tmp : Joins)
	{
		SQL += Tmp.ToString();
	}
	if (!Conditions.IsEmpty())
	{
		SQL += TEXT(" WHERE ") + Conditions;
	}
	if (Groups.Num() > 0)
	{
		SQL += TEXT(" GROUP BY ");
		for (auto& Tmp : Groups)
		{
			SQL += Tmp + TEXT(",");
		}
		SQL.RemoveFromEnd(",");
	}
	if (!Havings.IsEmpty())
	{
		SQL += TEXT(" Having ") + Havings;
	}
	if (OrdersAndIsDesc.Num() > 0)
	{
		SQL += TEXT(" ORDER BY ");
		for (auto& Tmp : OrdersAndIsDesc)
		{
			SQL += TEXT("`") + Tmp.Key + (Tmp.Value ? TEXT("` DESC,") : TEXT("`,"));
		}
		SQL.RemoveFromEnd(",");
	}
	if (Limit.LimitNum >= 0)
	{
		SQL += Limit.ToString();
	}
	SQL += TEXT(";");
	if (QueryLink(SQL) && GetSelectResults(Results, bIsStoreResults))
	{
		return true;
	}
	UE_LOG(LogDSUDatabase, Error, TEXT("SelectData failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
	return false;
}

bool FMySQL_Link::UpdateData(const FString& TableName, const FMySQL_UpdateParams& Params)
{
	FString SQL = TEXT("UPDATE `") + TableName + TEXT("`");
	SQL += Params.ToString();
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::UpdateDataReplace(const FString& TableName, const FMySQL_UpdateReplaceParams& Params)
{
	FString SQL = TEXT("UPDATE `") + TableName + TEXT("`");
	SQL += Params.ToString();
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::GetSelectResults(TArray<FMySQL_FieldsData>& Results, const bool bIsStoreResults)
{
	MYSQL_RES* ResultValues = bIsStoreResults ? mysql_store_result(MySQL) : mysql_use_result(MySQL);
	if (ResultValues)
	{
		Results.Empty();
		int32 FieldNum = mysql_num_fields(ResultValues);
		MYSQL_FIELD* FieldProperties = mysql_fetch_field(ResultValues);
		for (int i = 0; i < FieldNum; ++i)
		{
			FMySQL_FieldsData FieldsData;
			FieldsData.Name = UTF8_TO_TCHAR(FieldProperties[i].name);
			FieldsData.Type = static_cast<EMySQL_VariableType>(FieldProperties[i].type);
			FieldsData.MaxLength = FieldProperties[i].max_length;
			Results.Add(FieldsData);
		}
		while (MYSQL_ROW Row = mysql_fetch_row(ResultValues))
		{
			for (int i = 0; i < FieldNum; ++i)
			{
				Results[i].DataValues.Add(Row[i] == nullptr ? FString(TEXT("NULL")) : FString(UTF8_TO_TCHAR(Row[i])));
			}
		}
		mysql_free_result(ResultValues);
		return true;
	}
	UE_LOG(LogDSUDatabase, Error, TEXT("mysql_store_result failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
	return false;
}

void FMySQL_Link::PrintResults(const TArray<FMySQL_FieldsData>& Results)
{
	FString HeadLine = TEXT("+");
	FString FieldsLine = TEXT("|");
	for (int i = 0; i < Results.Num(); ++i)
	{
		int MaxLen = FMath::Max(FMath::Max(Results[i].MaxLength, 4), Results[i].Name.Len());
		for (int j = 0; j < MaxLen + 2; ++j)
		{
			HeadLine += TEXT("-");
		}
		HeadLine += TEXT("+");

		int SpaceLen = 2 + (MaxLen - Results[i].Name.Len());
		for (int j = 0; j < (SpaceLen % 2 == 0 ? SpaceLen / 2 : SpaceLen / 2 + 1); ++j)
			FieldsLine += TEXT(" ");
		FieldsLine += Results[i].Name;
		for (int j = 0; j < SpaceLen / 2; ++j)
			FieldsLine += TEXT(" ");
		FieldsLine += TEXT("|");
	}
	UE_LOG(LogTemp, Display, TEXT("%s"), *HeadLine);
	UE_LOG(LogTemp, Display, TEXT("%s"), *FieldsLine);
	UE_LOG(LogTemp, Display, TEXT("%s"), *HeadLine);

	if (Results.Num() > 0 && Results[0].DataValues.Num() > 0)
	{
		int RowNum = Results[0].DataValues.Num();
		for (int i = 0; i < RowNum; ++ i)
		{
			FString RowLine = TEXT("|");
			for (auto& Tmp : Results)
			{
				int MaxLen = FMath::Max(FMath::Max(Tmp.MaxLength, 4), Tmp.Name.Len());
				int SpaceLen = 2 + (MaxLen - Tmp.DataValues[i].Len());
				for (int j = 0; j < (SpaceLen % 2 == 0 ? SpaceLen / 2 : SpaceLen / 2 + 1); ++j)
					RowLine += TEXT(" ");
				RowLine += Tmp.DataValues[i];
				for (int j = 0; j < SpaceLen / 2; ++j)
					RowLine += TEXT(" ");
				RowLine += TEXT("|");
			}
			UE_LOG(LogTemp, Display, TEXT("%s"), *RowLine);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("%s"), *HeadLine);
}

bool FMySQL_Link::ChangeDatabaseCharset(const FString& Database, const EMySQL_Charset NewCharset)
{
	FString SQL = TEXT("ALTER DATABASE `") + Database + TEXT("` DEFAULT CHARACTER SET = ");
	SQL += StaticEnum<EMySQL_Charset>()->GetNameStringByIndex((int32)NewCharset) + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::ChangeTableCharset(const FString& Table, const EMySQL_Charset NewCharset, const FString& Database)
{
	if (!Database.IsEmpty())
	{
		if (!SelectDatabase(Database)) return false;
	}
	FString SQL = TEXT("ALTER TABLE `") + Table + TEXT("` DEFAULT CHARACTER SET = ");
	SQL += StaticEnum<EMySQL_Charset>()->GetNameStringByIndex((int32)NewCharset) + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::ChangeTableEngine(const FString& Table, const EMySQL_SaveEngine NewEngine, const FString& Database)
{
	if (!Database.IsEmpty())
	{
		if (!SelectDatabase(Database)) return false;
	}
	FString SQL = TEXT("ALTER TABLE `") + Table + TEXT("` ENGINE=");
	SQL += StaticEnum<EMySQL_SaveEngine>()->GetNameStringByIndex((int32)NewEngine) + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::ChangeTableName(const FString& OldTable, const FString& NewTable, const FString& Database)
{
	if (!Database.IsEmpty())
	{
		if (!SelectDatabase(Database)) return false;
	}
	FString SQL = TEXT("ALTER TABLE `") + OldTable + TEXT("` RENAME TO ") + NewTable + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::AlterTableColumn(const FString& Table, const TArray<FMySQL_AlterColumnParams>& Params, const FString& Database)
{
	if (!Database.IsEmpty())
	{
		if (!SelectDatabase(Database)) return false;
	}
	FString SQL = TEXT("ALTER TABLE `") + Table + TEXT("` ");
	for (auto& Tmp : Params)
	{
		SQL += Tmp.ToString() + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::AlterTableIndex(const FString& Table, const TArray<FMySQL_AlterIndexParams>& Params, const FString& Database)
{
	if (!Database.IsEmpty())
	{
		if (!SelectDatabase(Database)) return false;
	}
	FString SQL = TEXT("ALTER TABLE `") + Table + TEXT("` ");
	for (auto& Tmp : Params)
	{
		SQL += Tmp.ToString() + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::AlterTableForeing(const FString& Table, const FMySQL_AlterForeingParams& Params, const FString& Database)
{
	if (!Database.IsEmpty())
	{
		if (!SelectDatabase(Database)) return false;
	}
	FString SQL = TEXT("ALTER TABLE `") + Table + TEXT("` ");
	SQL += Params.ToString();
	return QueryLink(SQL);
}

bool FMySQL_Link::StartTransaction()
{
	FString SQL = TEXT("START TRANSACTION;");
	return QueryLink(SQL);
}

bool FMySQL_Link::SetAutoCommit(const bool bAuto)
{
	FString SQL = TEXT("SET AUTOCOMMIT = ") + FString::Printf(TEXT("%i"), bAuto) + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::Commit()
{
	FString SQL = TEXT("COMMIT;");
	return QueryLink(SQL);
}

bool FMySQL_Link::SavePoint(const FString& PointName)
{
	FString SQL = TEXT("SAVEPOINT = ") + PointName + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::RollBack(const FString& PointName)
{
	FString SQL = TEXT("ROLLBACK");
	if (!PointName.IsEmpty())
	{
		SQL += TEXT(" ") + PointName;
	}
	SQL += TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::GetStatus(TMap<FString, FString>& Results)
{
	FString SQL = TEXT("SHOW STATUS;");
	if (QueryLink(SQL))
	{
		if (MYSQL_RES* QueryRes = mysql_store_result(MySQL))
		{
			Results.Empty();
			int32 FieldNum = mysql_num_fields(QueryRes);
			while (MYSQL_ROW Row = mysql_fetch_row(QueryRes))
			{
				Results.Add(FString(UTF8_TO_TCHAR(Row[0])), FString(UTF8_TO_TCHAR(Row[1])));
			}
			mysql_free_result(QueryRes);
			return true;
		}
	}
	return false;
}

bool FMySQL_Link::GetDatabases(TArray<FString>& Results)
{
	FString SQL = TEXT("SHOW DATABASES;");
	if (QueryLink(SQL))
	{
		if (MYSQL_RES* QueryRes = mysql_store_result(MySQL))
		{
			Results.Empty();
			while (MYSQL_ROW Row = mysql_fetch_row(QueryRes))
			{
				if (Row[0] != nullptr)
				{
					Results.Add(UTF8_TO_TCHAR(Row[0]));
				}
			}
			mysql_free_result(QueryRes);
			return true;
		}
	}
	return false;
}

bool FMySQL_Link::GetTables(TArray<FString>& Results, FString DatabaseName)
{
	FString SQL = DatabaseName.IsEmpty() ? TEXT("SHOW TABLES;") 
										 : TEXT("SHOW TABLES FROM `") + DatabaseName + TEXT("`;");
	if (QueryLink(SQL))
	{
		if (MYSQL_RES* QueryRes = mysql_store_result(MySQL))
		{
			Results.Empty();
			while (MYSQL_ROW Row = mysql_fetch_row(QueryRes))
			{
				if (Row[0] != nullptr)
				{
					Results.Add(UTF8_TO_TCHAR(Row[0]));
				}
			}
			mysql_free_result(QueryRes);
			return true;
		}
	}
	return false;
}

bool FMySQL_Link::GetTableStatus(TArray<FMySQL_FieldsData>& Results, FString DatabaseName)
{
	FString SQL = TEXT("SHOW TABLE STATUS FROM `") + DatabaseName + TEXT("`;");
	if (QueryLink(SQL))
	{
		return GetSelectResults(Results);
	}
	return false;
}

bool FMySQL_Link::GetTableDESC(TArray<FMySQL_FieldsData>& Results, FString TableName, FString DatabaseName)
{
	if (!DatabaseName.IsEmpty())
	{
		if (!QueryLink(TEXT("Use `") + DatabaseName + TEXT("`;"))) return false;
	}
	FString SQL = TEXT("DESC `") + TableName + TEXT("`;");
	if (QueryLink(SQL))
	{
		return GetSelectResults(Results);
	}
	return false;
}

bool FMySQL_Link::SetConnectTimeout(const int32& TimeoutSeconds)
{
	if (TimeoutSeconds < 0) return false;
	unsigned int Timeout = static_cast<unsigned int>(TimeoutSeconds);
	if (mysql_options(MySQL, MYSQL_OPT_CONNECT_TIMEOUT, &Timeout) != 0) 
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetReadTimeout(const int32& TimeoutSeconds)
{
	if (TimeoutSeconds < 0) return false;
	unsigned int Timeout = static_cast<unsigned int>(TimeoutSeconds);
	if (mysql_options(MySQL, MYSQL_OPT_READ_TIMEOUT, &Timeout) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetWriteTimeout(const int32& TimeoutSeconds)
{
	if (TimeoutSeconds < 0) return false;
	unsigned int Timeout = static_cast<unsigned int>(TimeoutSeconds);
	if (mysql_options(MySQL, MYSQL_OPT_WRITE_TIMEOUT, &Timeout) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetReconnect(const bool bReconnnect)
{
	if (mysql_options(MySQL, MYSQL_OPT_RECONNECT, &bReconnnect) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetInitCommand(const FString& Command)
{
	FTCHARToUTF8 Converted(*Command);
	const char* InitCmd = Converted.Get();
	if (mysql_options(MySQL, MYSQL_INIT_COMMAND, InitCmd) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetCharset(const EMySQL_Charset Charset)
{
	FTCHARToUTF8 Converted(*StaticEnum<EMySQL_Charset>()->GetNameStringByIndex((int32)Charset));
	const char* CharsetCstr = Converted.Get();
	if (mysql_options(MySQL, MYSQL_SET_CHARSET_NAME, CharsetCstr) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetReadDefaultFile(const FString& FilePath)
{
	FTCHARToUTF8 Converted(*FilePath);
	const char* FilePathCstr = Converted.Get();
	if (mysql_options(MySQL, MYSQL_READ_DEFAULT_FILE, FilePathCstr) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetReadDefaultGroup(const FString& Group)
{
	FTCHARToUTF8 Converted(*Group);
	const char* GroupCstr = Converted.Get();
	if (mysql_options(MySQL, MYSQL_READ_DEFAULT_GROUP, GroupCstr) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetLocalInFile(const bool bEnable)
{
	unsigned int EnableInFile = bEnable;
	if (mysql_options(MySQL, MYSQL_OPT_LOCAL_INFILE, &EnableInFile) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetSLLKey(const FString& InFile)
{
	FTCHARToUTF8 Converted(*InFile);
	const char* InFileCstr = Converted.Get();
	if (mysql_options(MySQL, MYSQL_OPT_SSL_KEY, InFileCstr) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetSLLCert(const FString& InFile)
{
	FTCHARToUTF8 Converted(*InFile);
	const char* InFileCstr = Converted.Get();
	if (mysql_options(MySQL, MYSQL_OPT_SSL_CERT, InFileCstr) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

bool FMySQL_Link::SetSLLCA(const FString& InFile)
{
	FTCHARToUTF8 Converted(*InFile);
	const char* InFileCstr = Converted.Get();
	if (mysql_options(MySQL, MYSQL_OPT_SSL_CA, InFileCstr) != 0)
	{
		UE_LOG(LogDSUDatabase, Error, TEXT("mysql_options failed: %s"), UTF8_TO_TCHAR(mysql_error(MySQL)));
		return false;
	}
	return true;
}

void FMySQL_Link::FBindAllocation::Free()
{
	if (Size != 0)
	{
		uint8* Byteode = (uint8*)StmtBind;
		for (uint32 i = 0; i < Size; i++)
		{
			MYSQL_BIND* TmpBind = (MYSQL_BIND*)Byteode;

			FMemory::Free(TmpBind->buffer);
			TmpBind->buffer = nullptr;

			FMemory::Free(TmpBind->length);
			TmpBind->length = nullptr;

			FMemory::Free(TmpBind);
			Byteode += sizeof(MYSQL_BIND);
		}
		Size = 0;
		StmtBind = nullptr;
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif