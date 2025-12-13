#pragma once

#include "CoreMinimal.h"
#include "../NetChannelManager.h"

class FSocket;
class FNetConnectionBase;


class FNetChannelDriverUDP : public FNetChannelManager
{
public:
	FNetChannelDriverUDP(const ENetLinkState InState);

	virtual bool Init(int32 InPort) override;
	virtual void Tick(float DeltaTime) override;
	void RunThread();
	void Listen();
	virtual void Close() override;

protected:
	FSocket* Socket = nullptr;
	bool bStopThread;
};