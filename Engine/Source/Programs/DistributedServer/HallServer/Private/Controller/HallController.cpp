#include "HallController.h"
#include "HallServer.h"
#include "DS_NetChannel/Core/NetChannelProtocols.h"
#include "DS_NetChannel/Connection/Base/NetConnectionBase.h"


void UHallController::Init()
{
	Super::Init();

}

void UHallController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UHallController::Close()
{
	Super::Close();
}

void UHallController::RecvProtocol(uint32 InProtocol)
{
	//Super::RecvProtocol(InProtocol);

	if (GetLinkState() == ENetLinkState::Listen)
	{
		switch (InProtocol)
		{
			case P_Login:
			{
				FString SteamID;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, SteamID);
				//NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);

				FNetChannelAddrInfo AddrInfo;
				if (GetChannelAddrInfo(AddrInfo))
				{
					CLIENT_SEND(DatabaseClient, P_RequestUserAssetInfo, AddrInfo, SteamID);
				}
				break;
			}
		}
	}
	else
	{
		switch (InProtocol)
		{
			case P_ResponseUserAssetInfo:
			{
				FNetChannelAddrInfo GameAddrInfo;
				FNetUserAssetInfo UserAssets;
				NETCHANNEL_PROTOCOLS_RECV(P_ResponseUserAssetInfo, GameAddrInfo, UserAssets);
				SERVER_SEND(HallServer, GameAddrInfo, P_LoginSuccess, UserAssets);
				break;
			}
		}
	}
}
