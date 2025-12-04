#pragma once

#include "CoreMinimal.h"
#include "MySQL_LinkType.h"

#ifdef TEXT
#undef TEXT
#include "../MySQL/mysql.h"
#endif



struct FMySQL_Link
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

	bool InitOptions();
	bool ConnectMySQL();

	bool QueryLink(const FString& SQL);

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

private:
	const FString Host;
	const FString User;
	const FString Password;
	const uint32 Port;
	const FString UnixSocket;
	const TSet<EMySQL_ClientFlag> ClientFlags;

	FString DB;
	MYSQL MySQL;
};