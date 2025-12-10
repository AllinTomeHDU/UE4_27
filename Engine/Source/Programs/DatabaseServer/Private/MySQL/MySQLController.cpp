#include "MySQLController.h"
#include "../Core/DatabaseProtocols.h"
#include "DedicatedServerUtils/Database/DatabaseGlobalInfo.h"
#include "DedicatedServerUtils/Database/DatabaseManager.h"
#include "DedicatedServerUtils/Database/MySQL/Object/MySQL_Object.h"
#include "DedicatedServerUtils/Database/MySQL/Link/MySQL_Link.h"


bool UMySQLController::Post(const FString& InSQL)
{
	if (!InSQL.IsEmpty() && ObjectWrite)
	{
		return ObjectWrite->GetLink()->QueryLink(InSQL);
	}
	return false;
}

bool UMySQLController::Get(const FString& InSQL, TArray<FMySQL_FieldsData>& Results)
{
	if (!InSQL.IsEmpty() && ObjectRead)
	{
		if (ObjectRead->GetLink()->QueryLink(InSQL))
		{
			return ObjectRead->GetLink()->GetSelectResults(Results);
		}
	}
	return false;
}

void UMySQLController::Init()
{
	Super::Init();

	ObjectRead = UDatabaseManager::CreateMySQL_Object(nullptr, FMySQLGlobalInfo::Get()->GetInfo());
	ObjectWrite = UDatabaseManager::CreateMySQL_Object(nullptr, FMySQLGlobalInfo::Get()->GetInfo());
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
	if (GetLinkState() == ENetLinkState::Listen)
	{
		switch (InProtocol)
		{
			case P_MySQL:
			{
				FString Content;
				NETCHANNEL_PROTOCOLS_RECV(P_MySQL, Content);
				UE_LOG(LogTemp, Display, TEXT("Server recv Msg: %s"), *Content);

				Content = TEXT("Welcome Client");
				NETCHANNEL_PROTOCOLS_SEND(P_MySQL, Content);
				break;
			}
		}
	}
	else
	{
		switch (InProtocol)
		{
			case P_MySQL:
			{
				FString Content;
				NETCHANNEL_PROTOCOLS_RECV(P_MySQL, Content);
				UE_LOG(LogTemp, Display, TEXT("Client recv Msg: %s"), *Content);
				break;
			}
		}
	}
}
