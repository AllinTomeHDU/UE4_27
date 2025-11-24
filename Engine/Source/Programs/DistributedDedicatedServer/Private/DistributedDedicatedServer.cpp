// Copyright Epic Games, Inc. All Rights Reserved.


#include "DistributedDedicatedServer.h"

#include "RequiredProgramMainCPPInclude.h"

DEFINE_LOG_CATEGORY_STATIC(LogDistributedDedicatedServer, Log, All);

IMPLEMENT_APPLICATION(DistributedDedicatedServer, "DistributedDedicatedServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogDistributedDedicatedServer, Display, TEXT("Hello World"));
	FEngineLoop::AppExit();
	return 0;
}
