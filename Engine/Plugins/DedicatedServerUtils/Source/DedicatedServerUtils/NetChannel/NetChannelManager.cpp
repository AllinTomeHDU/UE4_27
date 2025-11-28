#include "NetChannelManager.h"
#include "NetChannelGlobalInfo.h"
#include "Core/NetChannelProtocols.h"
#include "Driver/NetChannelDriverUDP.h"
#include "Driver/NetChannelDriverTCP.h"
#include "Connection/Base/NetConnectionBase.h"
#include "UObject/NetChannelController.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"
#include "DedicatedServerUtils/Thread/ServerThreadManager.h"
#include "IPAddress.h"
using namespace DedicatedServerUtils;


FNetChannelManager* FNetChannelManager::CreateNetChannelManager(ENetLinkState InState, ENetSocketType InType)
{
	switch (InType)
	{
	case ENetSocketType::UDP:
		return new FNetChannelDriverUDP(InState);
	case ENetSocketType::TCP:
		return new FNetChannelDriverTCP(InState);
	}

	return nullptr;
}

void FNetChannelManager::Destroy(FNetChannelManager* InInstance)
{
	if (InInstance)
	{
		InInstance->Close();
		delete InInstance;
		InInstance = nullptr;
	}
}

UNetChannelController* FNetChannelManager::GetController()
{
	auto Channel = Connections.LocalConnection->GetMainChannel();
	return Channel ? Channel->GetNetObject<UNetChannelController>() : nullptr;
}

bool FNetChannelManager::Init()
{
	return true;
}

void FNetChannelManager::Tick(float DeltaTime)
{
}

void FNetChannelManager::Close()
{
}

#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
void FNetChannelManager::VerifyConnectionInfo(TSharedPtr<FNetConnectionBase> InConnection, uint8* InData, int32 BytesNum)
{
	if (!InConnection.IsValid() || !InConnection->GetMainChannel()) return;

	TArray<uint8> NewData(InData, BytesNum);
	auto Channel = InConnection->GetMainChannel();
	if (BytesNum > sizeof(FNetBunchHead))
	{
		Channel->AddMsg(NewData);
	}

	FNetBunchHead Head = *(FNetBunchHead*)InData;
	if (LinkState == ENetLinkState::Listen)
	{
		switch (Head.ProtocolsNumber)
		{
			case P_Hello:
			{
				FString RemoteVersion;
				NETCHANNEL_PROTOCOLS_RECV(P_Hello, RemoteVersion);
				if (!RemoteVersion.IsEmpty())
				{
					if (FNetChannelGlobalInfo::Get()->GetInfo().Version == RemoteVersion)
					{
						NETCHANNEL_PROTOCOLS_SEND(P_Challenge);
						InConnection->SetState(ENetConnectionState::Verify);
						InConnection->ResetHeartBeat();
					}
					else
					{
						FString ErrorInfo = FString::Printf(
							TEXT("The server vertion is [%s], current client version is [%s], need upgrade"),
							*FNetChannelGlobalInfo::Get()->GetInfo().Version, *RemoteVersion
						);
						NETCHANNEL_PROTOCOLS_SEND(P_Upgrade, ErrorInfo);
						InConnection->Close();
					}
				}
				else
				{
					FString ErrorInfo = FString::Printf(TEXT("the version passed by client is empty."));
					NETCHANNEL_PROTOCOLS_SEND(P_Upgrade, ErrorInfo);
					InConnection->Close();
				}
				break;
			}
			case P_Login:
			{
				FGuid MainGUID;
				TArray<FGuid> ChannelGUIDs;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, MainGUID, ChannelGUIDs);
				if (MainGUID != FGuid())
				{
					InConnection->GetMainChannel()->SetGUID(MainGUID);
					NETCHANNEL_PROTOCOLS_SEND(P_Welcom);
					InConnection->SetState(ENetConnectionState::Login);
				}
				else
				{
					FString ErrorInfo = TEXT("Login failure, GUID is NULL");
					NETCHANNEL_PROTOCOLS_SEND(P_Failure, ErrorInfo);
					InConnection->Close();
				}
				break;
			}
			case P_Join:
			{
				InConnection->SetState(ENetConnectionState::Join);
				break;
			}
		}
	}
	else
	{
		switch (Head.ProtocolsNumber)
		{
			case P_Challenge:
			{
				FGuid MainGUID = Connections.LocalConnection->GetMainChannel()->GetGUID();
				TArray<FGuid> ChannelGUIDs;
				Connections.LocalConnection->GetActiveChannelGUIDs(ChannelGUIDs);
				NETCHANNEL_PROTOCOLS_SEND(P_Login, MainGUID, ChannelGUIDs);
				Connections.LocalConnection->SetState(ENetConnectionState::Login);
				break;
			}
			case P_Welcom:
			{
				Connections.LocalConnection->SetState(ENetConnectionState::Join);
				NETCHANNEL_PROTOCOLS_SEND(P_Join);
				Connections.LocalConnection->LoopHeartBeat();
				break;
			}
			case P_Upgrade:
			{
				FString ErrorInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_Upgrade, ErrorInfo);
				UE_LOG(LogDedicatedServerUtils, Error, TEXT("Error: %s"), *ErrorInfo);
				InConnection->Close();
				break;
			}
			case P_Failure:
			{
				FString ErrorInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_Failure, ErrorInfo);
				UE_LOG(LogDedicatedServerUtils, Error, TEXT("Error: %s"), *ErrorInfo);
				InConnection->Close();
				break;
			}
		}
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif

TSharedPtr<FNetConnectionBase> FNetChannelManager::FNetConnections::operator[](TSharedPtr<FInternetAddr> InternetAddr)
{
	for (auto& Index : AliveRemoveConnectionsIndex)
	{
		TSharedPtr<FNetConnectionBase> Connection = RemoteConnections[Index];
		if (*Connection->GetAddr() == *InternetAddr) return Connection;
	}
	return nullptr;
}

//bool FNetChannelManager::FNetConnections::IsAddr(TSharedPtr<FInternetAddr> InternetAddr)
//{
//	for (auto& Index : AliveRemoveConnectionsIndex)
//	{
//		TSharedPtr<FNetConnectionBase> Connection = RemoteConnections[Index];
//		if (*Connection->GetAddr() == *InternetAddr) return true;
//	}
//	return false;
//}

//int32 FNetChannelManager::FNetConnections::Add(TSharedPtr<FInternetAddr> InternetAddr)
//{
//	for (int i = 0; i < RemoteConnections.Num(); ++i)
//	{
//		if (RemoteConnections[i]->GetState() == ENetConnectionState::UnInit)
//		{
//			RemoteConnections[i]->Init();
//
//			bool bBindAddr = false;
//			RemoteConnections[i]->GetAddr()->SetIp(*InternetAddr->ToString(false), bBindAddr);
//			RemoteConnections[i]->GetAddr()->SetPort(InternetAddr->GetPort());
//			RemoteConnections[i]->SetState(ENetConnectionState::Login);
//			
//			AliveRemoveConnectionsIndex.AddUnique(i);
//			UE_LOG(LogDedicatedServerUtils, Display, TEXT("Add a client Connection: %s:%d"), 
//				*InternetAddr->ToString(false), InternetAddr->GetPort());
//			return i;
//		}
//	}
//	return INDEX_NONE;
//}

void FNetChannelManager::FNetConnections::Close(int32 Index)
{
	if (Index >= 0 && Index < RemoteConnections.Num())
	{
		RemoteConnections[Index]->SetState(ENetConnectionState::UnInit);
		AliveRemoveConnectionsIndex.Remove(Index);
	}
}

TSharedPtr<FNetConnectionBase> FNetChannelManager::FNetConnections::GetEmptyConnection(TSharedPtr<FInternetAddr> InternetAddr)
{
	for (int i = 0; i < RemoteConnections.Num(); ++i)
	{
		if (RemoteConnections[i]->GetState() == ENetConnectionState::UnInit)
		{
			if (!RemoteConnections[i]->IsLock())
			{
				bool bBindAddr;
				RemoteConnections[i]->GetAddr()->SetIp(*InternetAddr->ToString(false), bBindAddr);
				RemoteConnections[i]->GetAddr()->SetPort(InternetAddr->GetPort());

				RemoteConnections[i]->Lock();
				AliveRemoveConnectionsIndex.AddUnique(i);
				return RemoteConnections[i];
			}
		}
	}
	return nullptr;
}

