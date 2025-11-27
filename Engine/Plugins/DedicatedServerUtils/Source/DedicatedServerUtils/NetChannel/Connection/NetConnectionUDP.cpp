#include "NetConnectionUDP.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"
#include "DedicatedServerUtils/NetChannel/Channel/NetChannelBase.h"
#include "Sockets.h"


void FNetConnectionUDP::Send(const TArray<uint8>& InData)
{
	if (!Socket) return;

	int32 BytesSend = 0;
	if (Socket->SendTo(InData.GetData(), InData.Num(), BytesSend, *GetAddr()))
	{
		UE_LOG(LogDedicatedServerUtils, Display, TEXT("Send Success!"));
	}
	else
	{
		UE_LOG(LogDedicatedServerUtils, Error, TEXT("Send Failed..."));
	}
}

void FNetConnectionUDP::Recv(const FGuid& InChannelGUID, TArray<uint8> InData)
{
	FNetConnectionBase::Recv(InChannelGUID, InData);

}

void FNetConnectionUDP::Verify()
{
	FNetConnectionBase::Verify();
}

void FNetConnectionUDP::Analysis(void* Data, int32 BytesNum)
{
	UE_LOG(LogDedicatedServerUtils, Display, TEXT("Analysis Data: %s, length = %d"), UTF8_TO_TCHAR((char*)Data), BytesNum);
}
