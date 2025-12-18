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
	virtual bool Bind(const FNetAddr& InAddr);
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;

protected:
	void RunThread();
	void Listen();

protected:
	FSocket* Socket = nullptr;
	bool bStopThread;
};