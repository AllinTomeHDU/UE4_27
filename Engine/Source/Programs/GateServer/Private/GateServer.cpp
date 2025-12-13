// Copyright Epic Games, Inc. All Rights Reserved.


#include "GateServer.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Log/GateServerLog.h"
#include "Login/LoginController.h"
#include "DSUNetChannel/NetChannelGlobalInfo.h"
#include "DSUThreadPool/ServerThreadManager.h"
#include "DSUNetChannel/Channel/NetChannelBase.h"


INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	FNetChannelGlobalInfo::Get()->Init();
	FNetChannelManager* LoginServer = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Listen, ENetSocketType::UDP);
	DatabaseClient = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Connect, ENetSocketType::UDP);

	FNetChannelBase::SimpleControllerDelegate.BindLambda(
		[]()->UClass* { return ULoginController::StaticClass(); }
	);

	if (!LoginServer || !LoginServer->Init())
	{
		delete LoginServer;
		UE_LOG(LogGateServer, Error, TEXT("Server Create Failed"));
		return -1;
	}
	if (!DatabaseClient || !DatabaseClient->Init(12234))
	{
		delete DatabaseClient;
		UE_LOG(LogGateServer, Error, TEXT("Client Create Failed"));
		return -1;
	}

	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		FPlatformProcess::Sleep(0.03f);
		double Now = FPlatformTime::Seconds();
		float DeltaTime = Now - LastTime;

		DSUThreadPool::FThreadManagement::Get()->Tick(DeltaTime);
		LoginServer->Tick(DeltaTime);
		DatabaseClient->Tick(DeltaTime);

		LastTime = Now;
	}

	FNetChannelManager::Destroy(LoginServer);
	FNetChannelManager::Destroy(DatabaseClient);
	DSUThreadPool::FThreadManagement::Destroy();


	FEngineLoop::AppExit();
	return 0;
}
	