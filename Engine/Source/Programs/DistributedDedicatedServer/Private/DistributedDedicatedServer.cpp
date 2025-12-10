// Copyright Epic Games, Inc. All Rights Reserved.


#include "DistributedDedicatedServer.h"
#include "RequiredProgramMainCPPInclude.h"



DEFINE_LOG_CATEGORY_STATIC(LogDDServer, Log, All);

IMPLEMENT_APPLICATION(DistributedDedicatedServer, "DistributedDedicatedServer");

static int32 Num = 0;

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);



	FEngineLoop::AppExit();
	return 0;
}
	