#pragma once

#include "CoreMinimal.h"
#include "../NetChannelManager.h"

class FSocket;
class FNetConnectionBase;


class FNetChannelDriverUDP : public FNetChannelManager
{
public:
	FNetChannelDriverUDP(const ENetLinkState InState);

	virtual bool Init() override;
	virtual void Tick(float DeltaTime) override;
	void RunThread();
	void Listen();
	virtual void Close() override;

protected:
	FSocket* Socket;
	bool bStopThread;
};