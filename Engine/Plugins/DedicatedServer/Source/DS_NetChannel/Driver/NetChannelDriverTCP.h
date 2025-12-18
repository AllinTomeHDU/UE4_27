#pragma once

#include "CoreMinimal.h"
#include "../NetChannelManager.h"

class FSocket;
class FNetConnectionBase;


class FNetChannelDriverTCP : public FNetChannelManager
{
public:
	FNetChannelDriverTCP(const ENetLinkState InState);

	virtual bool Init(int32 InPort) override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;

};