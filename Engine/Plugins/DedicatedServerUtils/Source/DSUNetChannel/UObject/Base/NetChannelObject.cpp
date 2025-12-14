#include "NetChannelObject.h"
#include "../../Channel/NetChannelBase.h"


bool UNetChannelObject::GetAddrInfo(FNetAddrInfo& AddrInfo)
{
	return Channel ? Channel->GetAddrInfo(AddrInfo) : false;
}

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
