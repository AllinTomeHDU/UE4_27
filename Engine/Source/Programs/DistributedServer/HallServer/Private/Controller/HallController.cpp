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
				FString UserID;
				FString UserName;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, UserID, UserName);


				NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);
				break;
			}
		}
	}
	else
	{

	}
}
