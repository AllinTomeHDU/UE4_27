// Copyright Epic Games, Inc. All Rights Reserved.


#include "GateServer.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Login/LoginController.h"
#include "DS_NetChannel/NetChannelGlobalInfo.h"
#include "DS_ThreadPool/ServerThreadManager.h"
#include "DS_NetChannel/Channel/NetChannelBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogGateServer, Log, All);
IMPLEMENT_APPLICATION(GateServer, "GateServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	FNetChannelGlobalInfo::Get()->Init();
	LoginServer = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Listen, ENetSocketType::UDP);
	DatabaseClient = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Connect, ENetSocketType::UDP);

	FNetChannelBase::SimpleControllerDelegate.BindLambda(
		[]()->UClass* { return ULoginController::StaticClass(); }
	);

	if (!LoginServer || !LoginServer->Init())
	{
		delete LoginServer;
		UE_LOG(LogTemp, Error, TEXT("Server Create Failed"));
		return -1;
	}
	if (!DatabaseClient || !DatabaseClient->Init(13306))
	{
		delete DatabaseClient;
		UE_LOG(LogTemp, Error, TEXT("Client Create Failed"));
		return -1;
	}

	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		FPlatformProcess::Sleep(0.03f);
		double Now = FPlatformTime::Seconds();
		float DeltaTime = Now - LastTime;

		DS_ThreadPool::FThreadManagement::Get()->Tick(DeltaTime);
		LoginServer->Tick(DeltaTime);
		DatabaseClient->Tick(DeltaTime);

		LastTime = Now;
	}

	FNetChannelManager::Destroy(LoginServer);
	FNetChannelManager::Destroy(DatabaseClient);
	DS_ThreadPool::FThreadManagement::Destroy();

	FEngineLoop::AppExit();
	return 0;
}
	