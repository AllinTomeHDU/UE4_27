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
		const uint32& InPort,
		const FString& InUnixSocket = TEXT(""),
		const TSet<EMySQL_ClientFlag>& InClientFlags = TSet<EMySQL_ClientFlag>()
	);

	~FMySQL_Link();

	bool InitOptions();
	bool ConnectMySQL();

	bool QueryLink(const FString& SQL);

	bool CreateDatabase(const FString& DatabaseName, EMySQL_Charset Charset = EMySQL_Charset::utf8, bool bAutoSelect = true);
	bool DropDatabase(const FString& DatabaseName);
	bool SelectDatabase(const FString& DatabaseName);

	bool CreateTable(const FString& TableName, const TMap<FString, FMySQL_FieldTypeProperties>& InFields, const TArray<FString>& InPrimaryKeys, const FMySQL_TableOptions& InTableOptions);
	bool OptimiseTable(const FString& TableName);
	bool DropTable(const FString& TableName);
	bool TruncateTable(const FString& TableName);

	bool InsertDataByRows(const FString& TableName, const TArray<TArray<FString>>& Rows, const TArray<FString>& Fields = {});
	bool InsertDataByFields(const FString& TableName, const TMap<FString, TArray<FString>>& DataToInsert);

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

	bool GetStoreResults(TArray<FMySQL_FieldsData>& Results);
	bool GetUseResults(TArray<FMySQL_FieldsData>& Results);
	void PrintResults(const TArray<FMySQL_FieldsData>& Results, const int Length = 10);

protected:
	void GetSelectResults(MYSQL_RES* ResultValues, TArray<FMySQL_FieldsData>& Results);

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