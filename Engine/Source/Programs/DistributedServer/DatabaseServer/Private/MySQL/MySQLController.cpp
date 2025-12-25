#include "MySQLController.h"
#include "DatabaseGlobalInfo.h"
#include "DatabaseManager.h"
#include "DS_NetChannel/Definition/NetChannelProtocols.h"
#include "DS_NetChannel/Connection/Base/NetConnectionBase.h"
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
		case P_Register:
		{
			FNetChannelAddrInfo GameAddrInfo;
			FString Account, Password, Name, Country, Platform;
			NETCHANNEL_PROTOCOLS_RECV(P_Register, GameAddrInfo, Account, Password, Name, Country, Platform);
			DealWithRegisterRequest(GameAddrInfo, Account, Password, Name, Country, Platform);
			break;
		}
		case P_Login:
		{
			FNetChannelAddrInfo GameAddrInfo;
			FString Account, Password;
			NETCHANNEL_PROTOCOLS_RECV(P_Login, GameAddrInfo, Account, Password);
			DealWithLoginRequest(GameAddrInfo, Account, Password);
			break;
		}
		case P_RequestUserAssets:
		{
			FNetChannelAddrInfo GameAddrInfo;
			FString Account;
			NETCHANNEL_PROTOCOLS_RECV(P_RequestUserAssets, GameAddrInfo, Account);
			DealWithUserAssetsRequest(GameAddrInfo, Account);
			break;
		}
	}
}

void UMySQLController::DealWithRegisterRequest(const FNetChannelAddrInfo& GameAddrInfo, 
	const FString& Account, const FString& Password, const FString& Name, const FString& Country, const FString& Platform)
{
	FString TableName = TEXT("player_info");
	FString SQL = FString::Printf(TEXT("SELECT account FROM %s WHERE account='%s';"), *TableName, *Account);
	TArray<FMySQL_FieldsData> Results;
	if (Get(SQL, Results))
	{
		if (Results.Num() > 0 && Results[0].DataValues.Num() > 0)
		{
			// 账号已存在
			NETCHANNEL_PROTOCOLS_SEND(P_AccountAlreadyExits, GameAddrInfo);
			return;
		}
		else
		{
			// 注册
			FString NowDate = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
			SQL = FString::Printf(
				TEXT("INSERT INTO %s VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s');"), 
				*TableName, *Account, *Password, *Name, *Country, *Platform, *NowDate, *NowDate
			);
			if (Post(SQL))
			{
				NETCHANNEL_PROTOCOLS_SEND(P_RegisterSuccess, GameAddrInfo);
				return;
			}
		}
	}
	FString ErrorMsg = TEXT("Get Data From Database Failed...");
	NETCHANNEL_PROTOCOLS_SEND(P_RegisterFailure, GameAddrInfo, ErrorMsg);
}

void UMySQLController::DealWithLoginRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& Account, const FString& Password)
{
	FString TableName = TEXT("player_info");
	FString SQL = FString::Printf(TEXT("SELECT password FROM %s WHERE account='%s';"), *TableName, *Account);
	TArray<FMySQL_FieldsData> Results;
	if (Get(SQL, Results))
	{
		FString NowDate = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
		if (Results.Num() > 0 && Results[0].DataValues.Num() > 0)
		{
			// 登录
			// 账号存在，比较密码是否正确，可能需要做加解密处理password
			if (Password == Results[0].DataValues[0])
			{
				SQL = FString::Printf(TEXT("UPDATE %s SET login_date='%s' WHERE account='%s';"), *TableName, *NowDate, *Account);
				if (Post(SQL))
				{
					SendHallServerInfo(GameAddrInfo);
					return;
				}
			}
			else
			{
				// 密码错误
				NETCHANNEL_PROTOCOLS_SEND(P_IncorrectPassword, GameAddrInfo);
				return;
			}
		}
		else
		{
			// 未注册
			NETCHANNEL_PROTOCOLS_SEND(P_AbsentAccount, GameAddrInfo);
			return;
		}
	}

	FString ErrorMsg = TEXT("Get Data From Database Failed...");
	NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, GameAddrInfo, ErrorMsg);
}

/*
* 若是本地部署的专用服务器需要维持游戏会话，可以根据传入的账号地区给玩家分配服务器
*/
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

void UMySQLController::DealWithUserAssetsRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& Account)
{
	FString TableName = TEXT("player_assets");
	FString SQL = FString::Printf(TEXT("SELECT * FROM %s WHERE steam_id='%s';"), *TableName, *Account);
	TArray<FMySQL_FieldsData> Results;
	if (Get(SQL, Results))
	{
		FString NowDate = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
		if (Results.Num() > 0 && Results[0].DataValues.Num() > 0)
		{
			if (Results.Num() == 5 && Results[0].DataValues.Num() > 0)
			{
				SendUserAssets(GameAddrInfo, Results);
				return;
			}
		}
		else
		{
			// 注册
			SQL = FString::Printf(TEXT("INSERT INTO `%s` (`steam_id`,`update_time`) VALUES ('%s', '%s');"), *TableName, *Account, *NowDate);
			if (Post(SQL))
			{
				// 插入之后再次查找，获取某些字段的默认构造值
				SQL = FString::Printf(TEXT("SELECT * FROM %s WHERE steam_id='%s';"), *TableName, *Account);
				if (Get(SQL, Results))
				{
					if (Results.Num() == 5 && Results[0].DataValues.Num() > 0)
					{
						SendUserAssets(GameAddrInfo, Results);
						return;
					}
				}
			}
		}
	}

	FString ErrorMsg = TEXT("DealWithUserAssetsRequest Failed...");
	NETCHANNEL_PROTOCOLS_SEND(P_FailedRequestUserAssets, GameAddrInfo, ErrorMsg);
}

void UMySQLController::SendUserAssets(const FNetChannelAddrInfo& GameAddrInfo, const TArray<FMySQL_FieldsData>& Results)
{
	FNetUserAssets UserAssets;
	FCStringAnsi::Strncpy(UserAssets.Rank, TCHAR_TO_UTF8(*Results[1].DataValues[0]), 20);
	UserAssets.SpiritStone = FCString::Atoi(*Results[2].DataValues[0]);
	UserAssets.ImmortalJade = FCString::Atoi(*Results[3].DataValues[0]);
	NETCHANNEL_PROTOCOLS_SEND(P_ResponseUserAssets, GameAddrInfo, UserAssets);
}
