#include "MySQLController.h"
#include "DatabaseGlobalInfo.h"
#include "DatabaseManager.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"
#include "DSUNetChannel/Connection/Base/NetConnectionBase.h"
#include "MySQL/Object/MySQL_Object.h"
#include "MySQL/Link/MySQL_Link.h"
#include "Misc/DateTime.h"
#include "SocketSubsystem.h"


bool UMySQLController::Post(const FString& InSQL)
{
	if (InSQL.IsEmpty()) return false;
	if (!WriteLink)
	{
		WriteLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetMySQLInfo());
		if (!WriteLink) return false;
	}
	return WriteLink->QueryLink(InSQL);
}

bool UMySQLController::Get(const FString& InSQL, TArray<FMySQL_FieldsData>& Results)
{
	if (InSQL.IsEmpty()) return false;
	if (!ReadLink)
	{
		ReadLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetMySQLInfo());
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

	ReadLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetMySQLInfo());
	WriteLink = UDatabaseManager::CreateMySQL_Link(FMySQLGlobalInfo::Get()->GetMySQLInfo());

	if (Channel->GetConnection()->GetLinkState() == ENetLinkState::Listen &&
		Channel->GetConnection()->GetIsMainListen())
	{
		FString SQL = TEXT("UPDATE server_info SET state='ONLINE' WHERE server_name='DatabaseServer';");
		if (!Post(SQL))
		{
			UE_LOG(LogTemp, Display, TEXT("Init DatabaseServer Failed"));
		}
	}
}

void UMySQLController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UMySQLController::Close()
{
	Super::Close();
}

void UMySQLController::RecvProtocol(uint32 InProtocol)
{
	//Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case P_Login:
		{
			FNetChannelAddrInfo GameAddrInfo;
			FString UserID;
			FString UserName;
			NETCHANNEL_PROTOCOLS_RECV(P_Login, GameAddrInfo, UserID, UserName);
			DealWithLoginRequest(GameAddrInfo, UserID, UserName);
			break;
		}
	}
}

void UMySQLController::DealWithLoginRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& UserID, const FString& UserName)
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
					SendHallServerInfo(GameAddrInfo);
					return;
				}
			}
			else
			{
				// 更新用户名
				SQL = FString::Printf(TEXT("UPDATE %s SET user_name='%s',login_date='%s' WHERE user_id='%s';"),
					*TableName, *UserName, *NowDate, *UserID);
				if (Post(SQL))
				{
					SendHallServerInfo(GameAddrInfo);
					return;
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
				SendHallServerInfo(GameAddrInfo);
				return;
			}

		}
	}

	FString ErrorMsg = TEXT("DealWithLoginRequest Failed...");
	NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, GameAddrInfo, ErrorMsg);
}

void UMySQLController::SendHallServerInfo(const FNetChannelAddrInfo& GameAddrInfo)
{
	FNetServerInfo ServerInfo;
	FString SQL = TEXT("SELECT i.server_id, i.server_name, i.ip, i.port ")
				  TEXT("FROM server_info AS i ")
				  TEXT("JOIN server_state AS s ")
				  TEXT("ON i.server_id = s.server_id ")
				  TEXT("WHERE i.state = 'ONLINE' AND i.server_name LIKE 'HallServer%' ")
				  TEXT("ORDER BY s.player_num ASC LIMIT 1;");
	TArray<FMySQL_FieldsData> Results;
	if (Get(SQL, Results))
	{
		if (Results.Num() == 4 && Results[0].DataValues.Num() > 0)
		{
			ServerInfo.ID = FCString::Atoi(*Results[0].DataValues[0]);
			FCStringAnsi::Strncpy(ServerInfo.Name, TCHAR_TO_UTF8(*Results[1].DataValues[0]), 20);
			FString IpStr = Results[2].DataValues[0];
			uint32 Port = FCString::Atoi(*Results[3].DataValues[0]);
			ServerInfo.Addr = FNetAddr(IpStr, Port);

			NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess, GameAddrInfo, ServerInfo);
			return;
		}
	}

	FString ErrorMsg = TEXT("GetHallServerInfo Failed...");
	NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, GameAddrInfo, ErrorMsg);
}
