// Copyright Epic Games, Inc. All Rights Reserved.

#include "DS_ThreadPool.h"

#define LOCTEXT_NAMESPACE "FDS_ThreadPoolModule"
DEFINE_LOG_CATEGORY(LogDS_ThreadPool);


void FDS_ThreadPoolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDS_ThreadPoolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDS_ThreadPoolModule, DS_ThreadPool)