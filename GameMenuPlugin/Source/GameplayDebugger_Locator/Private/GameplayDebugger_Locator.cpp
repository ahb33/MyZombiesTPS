// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayDebugger_Locator.h"

#define LOCTEXT_NAMESPACE "FGameplayDebugger_LocatorModule"

void FGameplayDebugger_LocatorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGameplayDebugger_LocatorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameplayDebugger_LocatorModule, GameplayDebugger_Locator)