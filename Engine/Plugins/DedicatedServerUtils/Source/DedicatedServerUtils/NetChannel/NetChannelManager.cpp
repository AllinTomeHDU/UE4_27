#include "NetChannelManager.h"
#include "NetChannelGlobalInfo.h"
#include "Core/NetChannelProtocols.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"
#include "Driver/NetChannelDriverUDP.h"
#include "Connection/Base/NetConnectionBase.h"
#include "IPAddress.h"


FNetChannelManager* FNetChannelManager::CreateNetChannelManager(ENetLinkState InState, ENetSocketType InType)
{
	switch (InType)
	{
	case ENetSocketType::UDP:
		return new FNetChannelDriverUDP(InState);
	case ENetSocketType::TCP:
		break;
	}

	return nullptr;
}

bool FNetChannelManager::Init()
{
	return true;
}

void FNetChannelManager::Tick(float DeltaTime)
{
}

#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
void FNetChannelManager::VerifyConnectionInfo(TSharedPtr<FNetConnectionBase> InConnection, uint8* InData, int32 InBytesNum)
{
	if (!InConnection.IsValid()) return;

	FNetBunchHead Head = *(FNetBunchHead*)InData;
	if (InBytesNum < sizeof(FNetBunchHead) || !InConnection->GetMainChannel()) return;

	TArray<uint8> NewData(InData, InBytesNum);
	auto Channel = InConnection->GetMainChannel();
	Channel->AddMsg(NewData);

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
					}
				}
				break;
			}
			case P_Login:
			{
				int32 ConnectID = -1;
				FGuid MainGUID;
				TArray<FGuid> ChannelGUIDs;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, ConnectID, MainGUID, ChannelGUIDs);
				if (ConnectID != -1 && MainGUID != FGuid())
				{
					InConnection->SetID(ConnectID);
					InConnection->GetMainChannel()->SetGUID(MainGUID);
					NETCHANNEL_PROTOCOLS_SEND(P_Welcom);
					InConnection->SetState(ENetConnectionState::Login);
				}
				break;
			}
			case P_Join:
			{
				NETCHANNEL_PROTOCOLS_RECV(P_Join);
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
				NETCHANNEL_PROTOCOLS_RECV(P_Challenge);
				int32 ConnectID = Connections.LocalConnection->GetID();
				FGuid MainGUID = Connections.LocalConnection->GetMainChannel()->GetGUID();
				TArray<FGuid> ChannelGUIDs;
				Connections.LocalConnection->GetActiveChannelGUIDs(ChannelGUIDs);
				NETCHANNEL_PROTOCOLS_SEND(P_Login, ConnectID, MainGUID, ChannelGUIDs);
				Connections.LocalConnection->SetState(ENetConnectionState::Login);
				break;
			}
			case P_Welcom:
			{
				NETCHANNEL_PROTOCOLS_RECV(P_Welcom);
				Connections.LocalConnection->SetState(ENetConnectionState::Join);
				NETCHANNEL_PROTOCOLS_SEND(P_Join);
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

