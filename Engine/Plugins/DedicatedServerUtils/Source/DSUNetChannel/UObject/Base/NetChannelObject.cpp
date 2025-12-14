#include "NetChannelObject.h"


void UNetChannelObject::Init()
{
}

void UNetChannelObject::Tick(float DeltaTime)
{

}

void UNetChannelObject::Close()
{
}

void UNetChannelObject::RecvProtocol(uint32 InProtocol)
{
	if (RecvDelegate.IsBound())
	{
		UE_LOG(LogTemp, Display, TEXT("RecvDelegate.Broadcast"));
		RecvDelegate.Broadcast(InProtocol, Channel);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RecvDelegate is not bind"));
	}
}
