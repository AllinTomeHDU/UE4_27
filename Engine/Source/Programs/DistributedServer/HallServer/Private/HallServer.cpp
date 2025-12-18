// Copyright Epic Games, Inc. All Rights Reserved.


#include "HallServer.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Controller/HallController.h"
#include "DS_NetChannel/NetChannelGlobalInfo.h"
#include "DS_ThreadPool/ServerThreadManager.h"
#include "DS_NetChannel/Channel/NetChannelBase.h"


DEFINE_LOG_CATEGORY_STATIC(LogHall, Log, All);
IMPLEMENT_APPLICATION(Hall, "HallServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	FNetChannelGlobalInfo::Get()->Init();
	HallServer = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Listen, ENetSocketType::UDP);
	CenterClient = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Connect, ENetSocketType::UDP);

	FNetChannelBase::SimpleControllerDelegate.BindLambda(
		[]()->UClass* { return UHallController::StaticClass(); }
	);

	if (!HallServer || !HallServer->Init())
	{
		delete HallServer;
		UE_LOG(LogTemp, Error, TEXT("Server Create Failed"));
		return -1;
	}
	if (!CenterClient || !CenterClient->Init(10100))
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

		DS_ThreadPool::FThreadManagement::Get()->Tick(DeltaTime);
		HallServer->Tick(DeltaTime);
		CenterClient->Tick(DeltaTime);

		LastTime = Now;
	}

	FNetChannelManager::Destroy(HallServer);
	FNetChannelManager::Destroy(CenterClient);
	DS_ThreadPool::FThreadManagement::Destroy();

	FEngineLoop::AppExit();
	return 0;
}
	