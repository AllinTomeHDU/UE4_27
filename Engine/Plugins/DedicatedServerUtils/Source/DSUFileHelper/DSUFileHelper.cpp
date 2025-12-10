// Copyright Epic Games, Inc. All Rights Reserved.

#include "DSUFileHelper.h"

#define LOCTEXT_NAMESPACE "FDSUFileHelperModule"
DEFINE_LOG_CATEGORY(LogDSUFileHelper);


void FDSUFileHelperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDSUFileHelperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDSUFileHelperModule, DSUFileHelper)