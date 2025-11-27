#include "NetChannelBase.h"
#include "../Connection/Base/NetConnectionBase.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"


FNetChannelBase::FNetChannelBase()
{

}

void FNetChannelBase::Init()
{

}

void FNetChannelBase::Close()
{
	GUID = FGuid();
}

void FNetChannelBase::AddMsg(TArray<uint8>& InData)
{
	MsgQueue.Add(MoveTemp(InData));
}


void FNetChannelBase::Send(const TArray<uint8>& InData)
{
	if (Connection.IsValid())
	{
		Connection.Pin()->Send(InData);
	}
	else
	{
		UE_LOG(LogDedicatedServerUtils, Display, TEXT("Channel Connection is not Valid"));
	}
}

bool FNetChannelBase::Recv(TArray<uint8>& InData)
{
	if (MsgQueue.IsValidIndex(0))
	{
		InData = MoveTemp(MsgQueue[0]);
		MsgQueue.RemoveAt(0);
		return true;
	}
	return false;
}

void FNetChannelBase::SpawnController()
{
	GUID = FGuid::NewGuid();


}

void FNetChannelBase::SpawnPlayer()
{
}
