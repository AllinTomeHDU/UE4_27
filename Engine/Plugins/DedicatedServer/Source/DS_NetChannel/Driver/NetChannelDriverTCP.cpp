#include "NetChannelDriverTCP.h"


FNetChannelDriverTCP::FNetChannelDriverTCP(const ENetLinkState InState)
{
	LinkState = InState;
}

bool FNetChannelDriverTCP::Init(int32 InPort)
{
	if (!FNetChannelManager::Init()) return false;

	return true;
}

void FNetChannelDriverTCP::Tick(float DeltaTime)
{
	FNetChannelManager::Tick(DeltaTime);
}

void FNetChannelDriverTCP::Close()
{
	FNetChannelManager::Close();
}
