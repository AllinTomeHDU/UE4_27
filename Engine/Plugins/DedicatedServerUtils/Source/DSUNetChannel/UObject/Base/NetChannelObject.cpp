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
		RecvDelegate.Broadcast(InProtocol, Channel);
	}
}
