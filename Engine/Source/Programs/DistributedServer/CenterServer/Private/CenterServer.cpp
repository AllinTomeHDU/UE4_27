// Copyright Epic Games, Inc. All Rights Reserved.


#include "CenterServer.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Controller/CenterController.h"
#include "DSUNetChannel/NetChannelGlobalInfo.h"
#include "DSUThreadPool/ServerThreadManager.h"
#include "DSUNetChannel/Channel/NetChannelBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogCenter, Log, All);
IMPLEMENT_APPLICATION(Center, "CenterServer");


INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	FNetChannelGlobalInfo::Get()->Init();
	CenterServer = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Listen, ENetSocketType::UDP);
	CenterClient = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Connect, ENetSocketType::UDP);

	FNetChannelBase::SimpleControllerDelegate.BindLambda(
		[]()->UClass* { return UCenterController::StaticClass(); }
	);

	if (!CenterServer || !CenterServer->Init())
	{
		delete CenterServer;
		UE_LOG(LogTemp, Error, TEXT("Server Create Failed"));
		return -1;
	}
	if (!CenterClient || !CenterClient->Init(13306))
	{
		delete CenterClient;
		UE_LOG(LogTemp, Error, TEXT("Client Create Failed"));
		return -1;
	}

	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		FPlatformProcess::Sleep(0.03f);
		double Now = FPlatformTime::Seconds();
		float DeltaTime = Now - LastTime;

		DSUThreadPool::FThreadManagement::Get()->Tick(DeltaTime);
		CenterServer->Tick(DeltaTime);
		CenterClient->Tick(DeltaTime);

		LastTime = Now;
	}

	FNetChannelManager::Destroy(CenterServer);
	FNetChannelManager::Destroy(CenterClient);
	DSUThreadPool::FThreadManagement::Destroy();

	FEngineLoop::AppExit();
	return 0;
}
	