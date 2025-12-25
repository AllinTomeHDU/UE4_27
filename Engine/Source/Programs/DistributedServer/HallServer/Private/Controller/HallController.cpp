#include "HallController.h"
#include "HallServer.h"
#include "DS_NetChannel/Definition/NetChannelProtocols.h"
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
				FString Account;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, Account);
				NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);

				FNetChannelAddrInfo AddrInfo;
				if (GetChannelAddrInfo(AddrInfo))
				{
					CLIENT_SEND(DatabaseClient, P_RequestUserAssets, AddrInfo, Account);
				}
				break;
			}
		}
	}
	else
	{
		switch (InProtocol)
		{
			case P_ResponseUserAssets:
			{
				FNetChannelAddrInfo GameAddrInfo;
				FNetUserAssets UserAssets;
				NETCHANNEL_PROTOCOLS_RECV(P_ResponseUserAssets, GameAddrInfo, UserAssets);
				SERVER_SEND(HallServer, GameAddrInfo, P_ResponseUserAssets, UserAssets);
				break;
			}
		}
	}
}
