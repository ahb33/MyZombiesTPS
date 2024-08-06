// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyZombiesGameMode.h"
#include "MyZombiesCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "UObject/ConstructorHelpers.h"


// MODIFY GAMEMODE CLASSES TO HANDLE PVP & PVE
AMyZombiesGameMode::AMyZombiesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Class'/Script/MyZombies.MainCharacter'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMyZombiesGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Call initialization logic
    InitializeGameplay();

    // Ensure game is not paused
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        PlayerController->SetPause(false);
        PlayerController->SetInputMode(FInputModeGameOnly());
        PlayerController->bShowMouseCursor = false;
    }
}

void AMyZombiesGameMode::InitializeGameplay()
{
    // Initialize your game state here
    // Example: Set initial number of enemies, spawn actors, etc.
    UE_LOG(LogTemp, Warning, TEXT("Gameplay Initialized"));

    // Example: Spawn initial AI enemies
    // TSubclassOf<AEnemyCharacter> EnemyClass = ...;
    // FActorSpawnParameters SpawnParams;
    // GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void AMyZombiesGameMode::CheckEnemiesAlive()
{
	// decrement Number of AIs
	// Check Enemies alive
	// if none left end game

    TArray<AActor*> FoundEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), FoundEnemies);
    int32 EnemyCount = FoundEnemies.Num();

	UE_LOG(LogTemp, Warning, TEXT("EnemyCount = %d"), EnemyCount);
    // Do something with EnemyCount
    if (EnemyCount == 0)
    {        
		// No enemies left
        EndGame(true);
    }
}

void AMyZombiesGameMode::EndGame(bool bPlayerWon)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

    if (bPlayerWon)
    {
        UUserWidget* YouWonWidget = CreateWidget<UUserWidget>(GetWorld(), YouWonWidgetClass);
        if (YouWonWidget)
        {
            YouWonWidget->AddToViewport();
            // Pause the game
            if (PlayerController)
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(YouWonWidget->TakeWidget());
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = true;
            }
        }
    }
    // Pause the game
    if (PlayerController)
    {
            PlayerController->SetPause(true);
    }
}
