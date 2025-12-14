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
			if (DatabaseClient && DatabaseClient->GetController())
			{
				if (auto ClientChannel = DatabaseClient->GetController()->GetChannel())
				{
					TArray<uint8> Buffer;
					FNetChannelIOStream Stream(Buffer);
					FNetBunchHead Head;
					Head.ProtocolsNumber = (uint32)P_Login; 
					Head.ChannelGUID = ClientChannel->GetGUID();
					Stream << Head;
					Stream << UserID;
					Stream << UserName;
					ClientChannel->Send(Buffer);
				}
			}
			//NETMANAGER_SEND(DatabaseClient, P_Login, UserID, UserName);
			//{
			//	auto Fun_LoginProtocol = [UserID, UserName]()
			//	{
			//		if (DatabaseClient && DatabaseClient->GetController())
			//		{
			//			if (auto Channel = DatabaseClient->GetController()->GetChannel())
			//			{
			//				NETCHANNEL_PROTOCOLS_SEND(P_Login, UserID, UserName);
			//			}
			//		}
			//	};
			//	Fun_LoginProtocol();
			//}
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
