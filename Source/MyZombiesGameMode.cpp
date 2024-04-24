// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyZombiesGameMode.h"
#include "MyZombiesCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyZombiesGameMode::AMyZombiesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Class'/Script/MyZombies.MainCharacter'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
