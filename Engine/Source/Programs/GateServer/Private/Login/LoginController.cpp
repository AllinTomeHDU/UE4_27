#include "LoginController.h"
#include "GateServer.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"


void ULoginController::Init()
{
	Super::Init();
}

void ULoginController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ULoginController::Close()
{
	Super::Close();
}

void ULoginController::RecvProtocol(uint32 InProtocol)
{
	//Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case P_Login:
		{
			FString UserID;
			FString UserName;
			NETCHANNEL_PROTOCOLS_RECV(P_Login, UserID, UserName);
			NETMANAGER_SEND(DatabaseClient, P_Login, UserID, UserName);
			break;
		}
		case P_LoginSuccess:
		{
			NETCHANNEL_PROTOCOLS_SEND(P_LoginSuccess);
			break;
		}
		case P_LoginFailure:
		{
			FString ErrorMsg;
			NETMANAGER_RECV(DatabaseClient, P_LoginFailure, ErrorMsg);
			break;
		}
	}
}
