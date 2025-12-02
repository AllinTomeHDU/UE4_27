#include "MySQL_Link.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"


FMySQL_Link::FMySQL_Link(const FString& InHost, const FString& InUser, const FString& InPassword, 
						 const uint32& InPort, const FString& InUnixSocket, const uint32& InClientFlag)
	: Host(InHost)
	, User(InUser)
	, Password(InPassword)
	, Port(InPort)
	, UnixSocket(InUnixSocket)
	, ClientFlag(InClientFlag)
{
	mysql_library_init(0, NULL, NULL);
	mysql_init(&MySQL);
}

FMySQL_Link::~FMySQL_Link()
{
	mysql_close(&MySQL);
	mysql_library_end();
}

void FMySQL_Link::InitOptions()
{

}

bool FMySQL_Link::QueryLink(const FString& SQL)
{
	int32 Ret = mysql_ping(&MySQL);
	if (Ret == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *SQL);
		if ((Ret = mysql_query(&MySQL, TCHAR_TO_UTF8(*SQL))) != 0)
		{
			UE_LOG(LogDedicatedServerUtils, Error, TEXT("mysql_query failed: %s"), ANSI_TO_TCHAR(mysql_error(&MySQL)));
			return false;
		}
		return true;
	}
	else
	{
		if (mysql_real_connect(&MySQL, TCHAR_TO_UTF8(*Host), TCHAR_TO_UTF8(*User), TCHAR_TO_UTF8(*Password), TCHAR_TO_UTF8(*DB),
			Port, UnixSocket == TEXT("") ? 0 : TCHAR_TO_UTF8(*UnixSocket), ClientFlag))
		{
			Ret = mysql_ping(&MySQL);
			if (Ret == 0)
			{
				UE_LOG(LogTemp, Display, TEXT("%s"), *SQL);
				if ((Ret = mysql_query(&MySQL, TCHAR_TO_UTF8(*SQL))) != 0)
				{
					UE_LOG(LogDedicatedServerUtils, Error, TEXT("mysql_query failed: %s"), ANSI_TO_TCHAR(mysql_error(&MySQL)));
					return false;
				}
			}
			else
			{
				UE_LOG(LogDedicatedServerUtils, Error, TEXT("mysql_ping failed: %s"), ANSI_TO_TCHAR(mysql_error(&MySQL)));
				return false;
			}
		}
		else
		{
			UE_LOG(LogDedicatedServerUtils, Error, TEXT("mysql_real_connect failed: %s"), ANSI_TO_TCHAR(mysql_error(&MySQL)));
			return false;
		}
	}
	return true;
}

bool FMySQL_Link::CreateDatabase(const FString& DatabaseName, EMySQL_Charset Charset)
{
	FString SQL = TEXT("CREATE DATABASE IF NOT EXISTS ") + DatabaseName;
	if (Charset != EMySQL_Charset::None)
	{
		SQL += (TEXT(" DEFAULT CHARSET=") + StaticEnum<EMySQL_Charset>()->GetNameStringByIndex((int32)Charset));
	}
	SQL += +TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::DropDatabase(const FString& DatabaseName)
{
	FString SQL = TEXT("DROP DATABASE ") + DatabaseName + TEXT(";");
	return QueryLink(SQL);
}

bool FMySQL_Link::SelectDatabase(const FString& DatabaseName)
{
	int32 Ret = mysql_ping(&MySQL);
	if (Ret == 0)
	{
		if (mysql_select_db(&MySQL, TCHAR_TO_UTF8(*DatabaseName)) == 0)
		{
			DB = DatabaseName;
			return true;
		}
	}
	else
	{
		if (mysql_real_connect(&MySQL, TCHAR_TO_UTF8(*Host), TCHAR_TO_UTF8(*User), TCHAR_TO_UTF8(*Password), TCHAR_TO_UTF8(*DB),
			Port, UnixSocket == TEXT("") ? 0 : TCHAR_TO_UTF8(*UnixSocket), ClientFlag))
		{
			if (mysql_select_db(&MySQL, TCHAR_TO_UTF8(*DatabaseName)) == 0)
			{
				DB = DatabaseName;
				return true;
			}
		}
	}
	UE_LOG(LogDedicatedServerUtils, Error, TEXT("SelectDatabase failed: %s"), ANSI_TO_TCHAR(mysql_error(&MySQL)));
	return false;
}

bool FMySQL_Link::CreateTable(const FString& TableName, const TMap<FString, FMySQL_FieldType>& InFields, const TArray<FString>& InPrimaryKeys, const FMySQL_TableOptions& InTableOptions)
{
	FString SQL = TEXT("CREATE TABLE ") + TableName + TEXT("(");
	for (auto& Tmp : InFields)
	{
		SQL += TEXT("`") + Tmp.Key + TEXT("` ") + Tmp.Value.ToString() + TEXT(",");
	}
	if (InPrimaryKeys.Num() > 0)
	{
		SQL += TEXT("PRIMARY KEY(`");
		for (auto& Tmp : InPrimaryKeys)
		{
			SQL += Tmp + TEXT(",");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT("`),");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(")");

	SQL += InTableOptions.ToString();

	SQL += TEXT(";");
	return QueryLink(SQL);
}



