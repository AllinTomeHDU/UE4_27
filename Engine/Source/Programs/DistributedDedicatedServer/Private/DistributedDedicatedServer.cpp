// Copyright Epic Games, Inc. All Rights Reserved.


#include "DistributedDedicatedServer.h"

#include "RequiredProgramMainCPPInclude.h"
#include "DedicatedServerUtils/NetChannel/NetChannelGlobalInfo.h"
#include "DedicatedServerUtils/NetChannel/NetChannelManager.h"
#include "DedicatedServerUtils/Thread/ServerThreadManager.h"
#include "DedicatedServerUtils/NetChannel/Channel/NetChannelBase.h"
#include "DedicatedServerUtils/NetChannel/Test/TestController.h"


DEFINE_LOG_CATEGORY_STATIC(LogDDServer, Log, All);

IMPLEMENT_APPLICATION(DistributedDedicatedServer, "DistributedDedicatedServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	FNetChannelGlobalInfo::Get()->Init();

	FNetChannelManager* Server = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Listen, ENetSocketType::UDP);
	FNetChannelManager* Client = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Connect, ENetSocketType::UDP);

	FNetChannelBase::SimpleControllerDelegate.BindLambda(
		[]()->UClass* { return UTestController::StaticClass(); }
	);

	if (!Server || !Server->Init())
	{
		delete Server;
		UE_LOG(LogDDServer, Error, TEXT("Server Create Failed"));
		return -1;
	}
	if (!Client || !Client->Init())
	{
		delete Client;
		delete Server;
		UE_LOG(LogDDServer, Error, TEXT("Client Create Failed"));
		return -1;
	}

	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		FPlatformProcess::Sleep(0.03f);
		double Now = FPlatformTime::Seconds();
		float DeltaTime = Now - LastTime;

		DedicatedServerUtils::FThreadManagement::Get()->Tick(DeltaTime);

		Server->Tick(DeltaTime);
		Client->Tick(DeltaTime);

		LastTime = Now;
	}

	FNetChannelManager::Destroy(Server);
	FNetChannelManager::Destroy(Client);
	DedicatedServerUtils::FThreadManagement::Destroy();

	FEngineLoop::AppExit();
	return 0;
}
	