// Copyright Epic Games, Inc. All Rights Reserved.


#include "DatabaseServer.h"

#include "RequiredProgramMainCPPInclude.h"
#include "MySQL/MysqlProtocols.h"
#include "DedicatedServerUtils/Database/Core/MySQL_Link.h"
#include "DedicatedServerUtils/NetChannel/NetChannelGlobalInfo.h"
#include "DedicatedServerUtils/NetChannel/NetChannelManager.h"
#include "DedicatedServerUtils/Thread/ServerThreadManager.h"
#include "DedicatedServerUtils/NetChannel/Channel/NetChannelBase.h"
#include "DedicatedServerUtils/NetChannel/Test/TestController.h"


DEFINE_LOG_CATEGORY_STATIC(LogDDServer, Log, All);

IMPLEMENT_APPLICATION(DatabaseServer, "DatabaseServer");

static int32 Num = 0;

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	//FMySQL_Link MySQL(TEXT("127.0.0.1"), TEXT("root"), TEXT(""));
	//if (MySQL.SelectDatabase(TEXT("test")))
	//{
	//	TArray<FMySQL_FieldsData> Results;
	//	MySQL.SelectData(Results, TEXT("players"), { TEXT("id"), TEXT("name"), TEXT("register_dt") });
	//	MySQL.PrintResults(Results);
	//}
	//system("pause");


	//FNetChannelGlobalInfo::Get()->Init();

	//FNetChannelManager* Server = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Listen, ENetSocketType::UDP);

	//FNetChannelBase::SimpleControllerDelegate.BindLambda(
	//	[]()->UClass* { return UTestController::StaticClass(); }
	//);

	//if (!Server || !Server->Init())
	//{
	//	delete Server;
	//	UE_LOG(LogDDServer, Error, TEXT("Server Create Failed"));
	//	return -1;
	//}

	//double LastTime = FPlatformTime::Seconds();
	//while (!IsEngineExitRequested())
	//{
	//	FPlatformProcess::Sleep(0.03f);
	//	double Now = FPlatformTime::Seconds();
	//	float DeltaTime = Now - LastTime;

	//	DedicatedServerUtils::FThreadManagement::Get()->Tick(DeltaTime);
	//	Server->Tick(DeltaTime);

	//	LastTime = Now;

	//	//UE_LOG(LogDDServer, Display, TEXT("TickNum:%d"), Num++);
	//}

	//FNetChannelManager::Destroy(Server);
	//DedicatedServerUtils::FThreadManagement::Destroy();

	FEngineLoop::AppExit();
	return 0;
}
	