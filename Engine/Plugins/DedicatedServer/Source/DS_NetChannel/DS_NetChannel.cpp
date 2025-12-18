// Copyright Epic Games, Inc. All Rights Reserved.

#include "DS_NetChannel.h"

#define LOCTEXT_NAMESPACE "FDS_NetChannelModule"
DEFINE_LOG_CATEGORY(LogDS_NetChannel);


void FDS_NetChannelModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDS_NetChannelModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDS_NetChannelModule, DS_NetChannel)