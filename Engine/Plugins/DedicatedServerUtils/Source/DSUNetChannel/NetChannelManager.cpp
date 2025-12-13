#include "NetChannelManager.h"
#include "NetChannelGlobalInfo.h"
#include "DSUNetChannel.h"
#include "Core/NetChannelProtocols.h"
#include "Driver/NetChannelDriverUDP.h"
#include "Driver/NetChannelDriverTCP.h"
#include "Connection/Base/NetConnectionBase.h"
#include "UObject/NetChannelController.h"
#include "IPAddress.h"


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
	if (!Connections.LocalConnection) return nullptr;
	auto Channel = Connections.LocalConnection->GetMainChannel();
	return Channel ? Channel->GetNetObject<UNetChannelController>() : nullptr;
}

bool FNetChannelManager::Init(int32 InPort)
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
				FGuid MainGUID;
				TArray<FGuid> ChannelGUIDs;
				NETCHANNEL_PROTOCOLS_RECV(P_Hello, RemoteVersion, MainGUID, ChannelGUIDs);
				if (!RemoteVersion.IsEmpty())
				{
					if (FNetChannelGlobalInfo::Get()->GetInfo().Version == RemoteVersion)
					{
						NETCHANNEL_PROTOCOLS_SEND(P_Welcom);
						InConnection->GetMainChannel()->SetGUID(MainGUID);
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
			case P_Join:
			{
				InConnection->SetState(ENetConnectionState::Join);
				//InConnection->GetMainChannel()->InitController();
				break;
			}
		}
	}
	else
	{
		switch (Head.ProtocolsNumber)
		{
			case P_Welcom:
			{
				Connections.LocalConnection->SetState(ENetConnectionState::Join);
				NETCHANNEL_PROTOCOLS_SEND(P_Join);
				Connections.LocalConnection->SetLoopHeartBeat(true);
				break;
			}
			case P_Upgrade:
			{
				FString ErrorInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_Upgrade, ErrorInfo);
				UE_LOG(LogDSUNetChannel, Error, TEXT("Error: %s"), *ErrorInfo);
				InConnection->Close();
				break;
			}
			case P_Failure:
			{
				FString ErrorInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_Failure, ErrorInfo);
				UE_LOG(LogDSUNetChannel, Error, TEXT("Error: %s"), *ErrorInfo);
				InConnection->Close();
				break;
			}
		}
	}
}

ENetConnectionState FNetChannelManager::GetLocalConnectState() const
{
	return Connections.LocalConnection->GetState();
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
				//RemoteConnections[i]->Init();
				AliveRemoveConnectionsIndex.AddUnique(i);
				return RemoteConnections[i];
			}
		}
	}
	return nullptr;
}

