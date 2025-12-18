#include "NetChannelObject.h"
#include "../../Channel/NetChannelBase.h"


bool UNetChannelObject::GetChannelAddrInfo(FNetChannelAddrInfo& AddrInfo)
{
	return Channel ? Channel->GetChannelAddrInfo(AddrInfo) : false;
}

void UNetChannelObject::Init()
{
}

void UNetChannelObject::Tick(float DeltaTime)
{

}

void UNetChannelObject::Close()
{
	RecvDelegate.Clear();
	JoinDelegate.Clear();
}

void UNetChannelObject::RecvProtocol(uint32 InProtocol)
{
	if (RecvDelegate.IsBound())
	{
		RecvDelegate.Broadcast(InProtocol, Channel);
	}
}
