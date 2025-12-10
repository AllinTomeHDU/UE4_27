#pragma once

#include "CoreMinimal.h"
#include "MySQL_LinkType.h"

struct MYSQL;
struct MYSQL_STMT;
struct MYSQL_BIND;


struct FMySQL_Link : public TSharedFromThis<FMySQL_Link>
{
public:
	FMySQL_Link(
		const FString& InHost,
		const FString& InUser,
		const FString& InPassword,
		const uint32& InPort = 3306,
		const FString& InUnixSocket = TEXT(""),
		const TArray<EMySQL_ClientFlag>& InClientFlags = {EMySQL_ClientFlag::Client_Multi_Statements}
	);

	~FMySQL_Link();

	bool ConnectMySQL();
	bool QueryLink(const FString& SQL);

	bool QueryLinkStmt(const FString& SQL, const TArray<FMySQL_StmtValue>& InQueryStmtParams);
	bool PrepareStmtSQL(const FString& SQL);
	bool PrepareStmtParams(const TArray<FMySQL_StmtValue>& InQueryStmtParams);
	bool ExecuteStmt();

	bool CreateDatabase(const FString& DatabaseName, EMySQL_Charset Charset = EMySQL_Charset::utf8, bool bAutoSelect = true);
	bool DropDatabase(const FString& DatabaseName);
	bool SelectDatabase(const FString& DatabaseName);

	bool CreateTable(
		const FString& TableName,
		const TMap<FString, FMySQL_FieldTypeProperties>& InFields,
		const TArray<FString>& InPrimaryKeys,
		const FMySQL_TableOptions& InTableOptions,
		const bool bIsTemporary = false
	);
	bool OptimiseTable(const FString& TableName);
	bool DropTable(const FString& TableName);
	bool TruncateTable(const FString& TableName);

	bool CreateTableLike(const FString& NewTable, const FString& SourceTableconst, const bool bIsTemporary = false);
	bool CopyTable(const FString& NewTable, const FString& SourceTable, const bool bIsTemporary = false);
	bool CopyTableSelect(
		const FString& NewTable,
		const FString& SourceTable,
		const TArray<FString>& Fields = {},
		const FString& Conditions = TEXT(""),
		const TMap<FString, bool>& OrdersAndIsDesc = TMap<FString, bool>(),
		const FMySQL_LimitParams& Limit = FMySQL_LimitParams(),
		const bool bIsDistinct = false,
		const bool bIsTemporary = false
	);

	bool InsertDataByRows(
		const FString& TableName, 
		const TArray<TArray<FString>>& Rows, 
		const TArray<FString>& Fields = {},
		const bool bIsIgnore = false,
		const bool bIsReplace = false
	);
	bool InsertDataByFields(
		const FString& TableName, 
		const TMap<FString, TArray<FString>>& DataToInsert,
		const bool bIsIgnore = false,
		const bool bIsReplace = false
	);
	bool InsertDataBySelect(
		const FString& TargetTable,
		const FString& SourceTable,
		const TArray<FString>& TargetFields = {},
		const TArray<FString>& SourceFields = {},
		const FString& Conditions = TEXT(""),
		const TMap<FString, bool>& OrdersAndIsDesc = TMap<FString, bool>(),
		const FMySQL_LimitParams& Limit = FMySQL_LimitParams(),
		const bool bIsDistinct = false,
		const bool bIsIgnore = false,
		const bool bIsReplace = false
	);

	bool DeleteData(const FString& TableName, const FString& Conditions = TEXT(""));

	bool SelectData(
		TArray<FMySQL_FieldsData>& Results,
		const FString& TableName,
		const TArray<FString>& Fields = {},
		const TArray<FMySQL_JoinParams>& Joins = {},
		const FString& Conditions = TEXT(""),
		const TArray<FString>& Groups = {},
		const FString& Havings = TEXT(""),
		const TMap<FString, bool>& OrdersAndIsDesc = TMap<FString, bool>(),
		const FMySQL_LimitParams& Limit = FMySQL_LimitParams(),
		const bool bIsDistinct = false,
		const bool bIsStoreResults = true
	);

	bool UpdateData(const FString& TableName, const FMySQL_UpdateParams& Params);
	bool UpdateDataReplace(const FString& TableName, const FMySQL_UpdateReplaceParams& Params);

	bool GetSelectResults(TArray<FMySQL_FieldsData>& Results, const bool bIsStore = true);
	void PrintResults(const TArray<FMySQL_FieldsData>& Results);

	// Alter
	bool ChangeDatabaseCharset(const FString& Database, const EMySQL_Charset NewCharset);
	bool ChangeTableCharset(const FString& Table, const EMySQL_Charset NewCharset, const FString& Database = TEXT(""));
	bool ChangeTableEngine(const FString& Table, const EMySQL_SaveEngine NewEngine, const FString& Database = TEXT(""));
	bool ChangeTableName(const FString& OldTable, const FString& NewTable, const FString& Database = TEXT(""));
	bool AlterTableColumn(const FString& Table, const TArray<FMySQL_AlterColumnParams>& Params, const FString& Database = TEXT(""));
	bool AlterTableIndex(const FString& Table, const TArray<FMySQL_AlterIndexParams>& Params, const FString& Database = TEXT(""));
	bool AlterTableForeing(const FString& Table, const FMySQL_AlterForeingParams& Params, const FString& Database = TEXT(""));

	// 事务
	bool StartTransaction();
	bool SetAutoCommit(const bool bAuto);
	bool Commit();
	bool SavePoint(const FString& PointName);
	bool RollBack(const FString& PointName = TEXT(""));

	// 元数据：可以根据 Select结果 结合 GetSelectResults和PrintResult 进行获取和显示
	bool GetStatus(TMap<FString, FString>& Results);
	bool GetDatabases(TArray<FString>& Results);
	bool GetTables(TArray<FString>& Results, FString DatabaseName = TEXT(""));
	bool GetTableStatus(TArray<FMySQL_FieldsData>& Results, FString DatabaseName);
	bool GetTableDESC(TArray<FMySQL_FieldsData>& Results, FString TableName, FString DatabaseName = TEXT(""));

	// 连接设置
	bool SetConnectTimeout(const int32& TimeoutSeconds);
	bool SetReadTimeout(const int32& TimeoutSeconds);
	bool SetWriteTimeout(const int32& TimeoutSeconds);
	bool SetReconnect(const bool bReconnnect = true);
	bool SetInitCommand(const FString& Command);
	bool SetCharset(const EMySQL_Charset Charset);
	bool SetReadDefaultFile(const FString& FilePath);
	bool SetReadDefaultGroup(const FString& Group);
	bool SetLocalInFile(const bool bEnable);
	bool SetSLLKey(const FString& InFile);
	bool SetSLLCert(const FString& InFile);
	bool SetSLLCA(const FString& InFile);

private:
	FString Host;
	FString User;
	FString Password;
	uint32 Port;
	FString UnixSocket;
	TArray<EMySQL_ClientFlag> ClientFlags;

	FString DB;
	MYSQL* MySQL;

	MYSQL_STMT* MysqlStmt;

	struct FBindAllocation
	{
		FBindAllocation() : Size(0), StmtBind(NULL) {}
		void Free();
		uint32 Size;
		MYSQL_BIND* StmtBind;
	} BindAllocation;
};