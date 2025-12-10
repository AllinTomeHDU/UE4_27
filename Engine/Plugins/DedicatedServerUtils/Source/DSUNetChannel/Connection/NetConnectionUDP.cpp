#include "NetConnectionUDP.h"
#include "../DSUNetChannel.h"
#include "../Channel/NetChannelBase.h"
#include "Sockets.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
void FNetConnectionUDP::Send(const TArray<uint8>& InData)
{
	if (!Socket) return;

	int32 BytesSend = 0;
	if (!Socket->SendTo(InData.GetData(), InData.Num(), BytesSend, *GetAddr()))
	{
		UE_LOG(LogDSUNetChannel, Error, TEXT("Send Failed..."));
	}
}

void FNetConnectionUDP::Recv(const FGuid& InChannelGUID, TArray<uint8>& InData)
{
	FNetConnectionBase::Recv(InChannelGUID, InData);

}

void FNetConnectionUDP::Verify()
{
	FNetConnectionBase::Verify();
}

void FNetConnectionUDP::Analysis(uint8* InData, int32 BytesNum)
{
	FNetConnectionBase::Analysis(InData, BytesNum);


}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif