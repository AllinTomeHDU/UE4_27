// Copyright Epic Games, Inc. All Rights Reserved.

#include "DS_FileHelper.h"

#define LOCTEXT_NAMESPACE "FDS_FileHelperModule"
DEFINE_LOG_CATEGORY(LogDS_FileHelper);


void FDS_FileHelperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDS_FileHelperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDS_FileHelperModule, DS_FileHelper)