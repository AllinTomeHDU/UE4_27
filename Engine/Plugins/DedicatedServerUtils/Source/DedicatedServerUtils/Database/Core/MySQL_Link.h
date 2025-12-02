#pragma once

#include "CoreMinimal.h"
#include "MySQL_LinkType.h"

#ifdef TEXT
#undef TEXT
#include "../MySQL/mysql.h"
#endif

#define TEST_PASSWORD TEXT("20212526Hdu.")

struct FMySQL_Link
{
	const FString Host;
	const FString User;
	const FString Password;
	const uint32 Port;
	const FString UnixSocket;
	const uint32 ClientFlag;

	FString DB;
	MYSQL MySQL;

public:
	FMySQL_Link(
		const FString& InHost,
		const FString& InUser,
		const FString& InPassword,
		const uint32& InPort,
		const FString& InUnixSocket = TEXT(""),
		const uint32& InClientFlag = 0
	);

	~FMySQL_Link();

	void InitOptions();

	bool QueryLink(const FString& SQL);

	bool CreateDatabase(const FString& DatabaseName, EMySQL_Charset Charset = EMySQL_Charset::utf8);
	bool DropDatabase(const FString& DatabaseName);
	bool SelectDatabase(const FString& DatabaseName);

	bool CreateTable(
		const FString& TableName, 
		const TMap<FString, FMySQL_FieldType>& InFields,
		const TArray<FString>& InPrimaryKeys,
		const FMySQL_TableOptions& InTableOptions
	);

private:

};