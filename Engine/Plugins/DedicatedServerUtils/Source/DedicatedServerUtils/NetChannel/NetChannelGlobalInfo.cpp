#include "NetChannelGlobalInfo.h"


FNetChannelGlobalInfo* FNetChannelGlobalInfo::Instance = nullptr;

FNetChannelGlobalInfo* FNetChannelGlobalInfo::Get()
{
	if (!Instance)
	{
		Instance = new FNetChannelGlobalInfo();
		Instance->Init();
	}

	return Instance;
}

void FNetChannelGlobalInfo::Destroy()
{
	delete Instance;
	Instance = nullptr;
}

void FNetChannelGlobalInfo::Init()
{

}
