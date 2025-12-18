#include "NetConnectionUDP.h"
#include "../DS_NetChannel.h"
#include "../Channel/NetChannelBase.h"
#include "Sockets.h"
#include "IPAddress.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
bool FNetConnectionUDP::Send(TArray<uint8>& InData)
{
	if (!FNetConnectionBase::Send(InData)) return false;

	int32 BytesSend = 0;
	return Socket->SendTo(InData.GetData(), InData.Num(), BytesSend, *GetAddr());
}

bool FNetConnectionUDP::Recv(const FGuid& InChannelGUID, TArray<uint8>& InData)
{
	if (!FNetConnectionBase::Recv(InChannelGUID, InData)) return false;

	return true;
}

void FNetConnectionUDP::Verify()
{
	FNetConnectionBase::Verify();
}

void FNetConnectionUDP::Analysis(TArray<uint8>& InData)
{
	FNetConnectionBase::Analysis(InData);
}

#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif