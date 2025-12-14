#include "MySQLController.h"
#include "DatabaseGlobalInfo.h"
#include "DatabaseManager.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"
#include "MySQL/Object/MySQL_Object.h"
#include "MySQL/Link/MySQL_Link.h"
#include "Misc/DateTime.h"


bool UMySQLController::Post(const FString& InSQL)
{
	//if (InSQL.IsEmpty()) return false;
	//if (!ObjectWrite)
	//{
	//	ObjectWrite = UDatabaseManager::CreateMySQL_Object(nullptr, FMySQLGlobalInfo::Get()->GetInfo());
	//	if (!ObjectWrite) return false;
	//}
	//return ObjectWrite->GetLink()->QueryLink(InSQL);

	if (InSQL.IsEmpty()) return false;
	if (!WriteLink)
	{
		WriteLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetInfo());
		if (!WriteLink) return false;
	}
	return WriteLink->QueryLink(InSQL);
}

bool UMySQLController::Get(const FString& InSQL, TArray<FMySQL_FieldsData>& Results)
{
	//if (InSQL.IsEmpty()) return false;
	//if (!ObjectRead)
	//{
	//	ObjectRead = UDatabaseManager::CreateMySQL_Object(nullptr, FMySQLGlobalInfo::Get()->GetInfo());
	//	if (!ObjectRead) return false;
	//}
	//if (ObjectRead->GetLink()->QueryLink(InSQL))
	//{
	//	return ObjectRead->GetLink()->GetSelectResults(Results);
	//}
	//return false;

	if (InSQL.IsEmpty()) return false;
	if (!ReadLink)
	{
		ReadLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetInfo());
		if (!ReadLink) return false;
	}
	if (ReadLink->QueryLink(InSQL))
	{
		return ReadLink->GetSelectResults(Results);
	}
	return false;
}

void UMySQLController::Init()
{
	Super::Init();

	ObjectRead = UDatabaseManager::CreateMySQL_Object(nullptr, FMySQLGlobalInfo::Get()->GetInfo());
	ObjectWrite = UDatabaseManager::CreateMySQL_Object(nullptr, FMySQLGlobalInfo::Get()->GetInfo());

	ReadLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetInfo());
	WriteLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetInfo());
}

void UMySQLController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UMySQLController::Close()
{
	Super::Close();

	if (ObjectRead)
	{
		ObjectRead->MarkPendingKill();
		ObjectRead = nullptr;
	}
	if (ObjectWrite)
	{
		ObjectWrite->MarkPendingKill();
		ObjectWrite = nullptr;
	}
}

void UMySQLController::RecvProtocol(uint32 InProtocol)
{
	//Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case P_Login:
		{
			FString UserID;
			FString UserName;
			NETCHANNEL_PROTOCOLS_RECV(P_Login, UserID, UserName);
			DealWithLoginRequest(UserID, UserName);
			break;
		}
	}
}

void UMySQLController::DealWithLoginRequest(FString& UserID, FString& UserName)
{
	FString TableName = TEXT("player_info");
	FString SQL = FString::Printf(TEXT("SELECT user_name FROM %s WHERE user_id='%s';"), *TableName, *UserID);
	TArray<FMySQL_FieldsData> Results;
	if (Get(SQL, Results))
	{
		FString NowDate = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
		if (Results.Num() > 0 && Results[0].DataValues.Num() > 0)
		{
			// 登录
			if (UserName == Results[0].DataValues[0])
			{
				SQL = FString::Printf(TEXT("UPDATE %s SET login_date='%s' WHERE user_id='%s';"),
					*TableName, *NowDate, *UserID);
				if (Post(SQL))
				{
					NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);
				}
				else
				{
					FString ErrorMsg = TEXT("Update Login Date Failed...");
					NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, ErrorMsg);
				}
			}
			else
			{
				// 更新用户名
				SQL = FString::Printf(TEXT("UPDATE %s SET user_name='%s',login_date='%s' WHERE user_id='%s';"),
					*TableName, *UserName, *NowDate, *UserID);
				if (Post(SQL))
				{
					NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);
				}
				else
				{
					FString ErrorMsg = TEXT("Update UserName Failed...");
					NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, ErrorMsg);
				}
			}
		}
		else
		{
			// 注册
			TArray<FString> Fields = { TEXT("user_id"), TEXT("user_name"), TEXT("login_date"), TEXT("register_date") };
			SQL = FString::Printf(
				TEXT("INSERT INTO %s (%s, %s, %s, %s) VALUES ('%s', '%s', '%s', '%s');"),
				*TableName, *Fields[0], *Fields[1], *Fields[2], *Fields[3],
				*UserID, *UserName, *NowDate, *NowDate
			);
			if (Post(SQL))
			{
				NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);
			}
			else
			{
				FString ErrorMsg = TEXT("Register Failed...");
				NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, ErrorMsg);
			}
		}
	}
	else
	{
		FString ErrorMsg = TEXT("Datebase Get Failed...");
		NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, ErrorMsg);
	}
}
