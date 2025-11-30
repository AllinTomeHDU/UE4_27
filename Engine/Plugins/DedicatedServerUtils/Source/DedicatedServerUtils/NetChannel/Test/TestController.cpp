#include "TestController.h"
#include "TestProtocols.h"


void UTestController::Init()
{
	Super::Init();

}

void UTestController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (GetLinkState() == ENetLinkState::Connect)
	//{
	//	FString Content = TEXT("Hello, i'm client!");
	//	NETCHANNEL_PROTOCOLS_SEND(P_Test, Content);
	//}
}

void UTestController::Close()
{
	Super::Close();
}

void UTestController::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	if (GetLinkState() == ENetLinkState::Listen)
	{
		switch (InProtocol)
		{
			case P_Test:
			{
				FString Content;
				NETCHANNEL_PROTOCOLS_RECV(P_Test, Content);
				UE_LOG(LogTemp, Display, TEXT("Server recv Msg: %s"), *Content);

				Content = TEXT("Welcome Client");
				NETCHANNEL_PROTOCOLS_SEND(P_Test, Content);
				break;
			}
		}
	}
	else
	{
		switch (InProtocol)
		{
			case P_Test:
			{
				FString Content;
				NETCHANNEL_PROTOCOLS_RECV(P_Test, Content);
				UE_LOG(LogTemp, Display, TEXT("Client recv Msg: %s"), *Content);
				break;
			}
		}
	}
}
