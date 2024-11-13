// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombiesGameMode.h"
#include "EnemyCharacter.h"
#include "AI_EnemySpawner.h"
#include "Kismet/GameplayStatics.h"

AZombiesGameMode::AZombiesGameMode()
{
    CurrentLevel = 1; // Initialize with the first wave
    RemainingEnemies = 0; // Initialize enemy count
}

void AZombiesGameMode::BeginPlay()
{
    Super::BeginPlay();
    ApplyLevelModifiers();
    StartNextWave();
}
void AZombiesGameMode::OnZombieKilled()
{
    UE_LOG(LogTemp, Warning, TEXT("Zombie killed"));
    
    RemainingEnemies--;
    if (RemainingEnemies <= 0)
    {
        CurrentLevel++;
        ApplyLevelModifiers();
        StartNextWave();
    }
}



void AZombiesGameMode::CheckEnemiesAlive()
{
    if (RemainingEnemies <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("All enemies dead, starting next wave"));
        CurrentLevel++;
        ApplyLevelModifiers();
        StartNextWave();
    }
}


void AZombiesGameMode::ApplyLevelModifiers()
{
    UE_LOG(LogTemp, Warning, TEXT("Applying level modifiers"));
    if (AIDifficultyTable)
    {
        static const FString ContextString(TEXT("Difficulty Context"));
        FAICharacterStats* Stats = AIDifficultyTable->FindRow<FAICharacterStats>(FName(*FString::Printf(TEXT("Level%d"), CurrentLevel)), ContextString);
        if (Stats)
        {
            // Set the number of zombies to spawn for the current wave
            NumberOfZombiesForCurrentLevel = Stats->ZombiesPerLevel;
            UE_LOG(LogTemp, Warning, TEXT("ZombiesPerLevel retrieved from data table: %d"), Stats->ZombiesPerLevel);

            // Log the applied settings for debugging
            UE_LOG(LogTemp, Log, TEXT("Wave %d stats applied: Number of Zombies = %d"), CurrentLevel, NumberOfZombiesForCurrentLevel);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No stats found for wave %d in the data table."), CurrentLevel);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AI Difficulty Table is not assigned!"));
    }
}

void AZombiesGameMode::StartNextWave()
{
    UE_LOG(LogTemp, Warning, TEXT("Starting next wave"));

    // Retrieve all spawners in the game
    TArray<AActor*> Spawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAI_EnemySpawner::StaticClass(), Spawners);

    if (Spawners.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No spawners found in the level!"));
        return;
    }

    // Reset RemainingEnemies for the current wave
    RemainingEnemies = NumberOfZombiesForCurrentLevel;

    // Use the first spawner to handle all zombie initialization and spawning
    if (AAI_EnemySpawner* EnemySpawner = Cast<AAI_EnemySpawner>(Spawners[0]))
    {
        // Initialize the zombie array and spawn the required number of zombies
        EnemySpawner->InitZombieArray(NumberOfZombiesForCurrentLevel);
        EnemySpawner->SpawnZombies(NumberOfZombiesForCurrentLevel);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast the first spawner!"));
    }

    // Bind events for the newly spawned zombies
    BindZombieDeathEvents();

    UE_LOG(LogTemp, Warning, TEXT("Wave %d started with %d zombies."), CurrentLevel, NumberOfZombiesForCurrentLevel);
}
void AZombiesGameMode::BindZombieDeathEvents()
{
    TArray<AActor*> Zombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Zombies);

    for (AActor* ZombieActor : Zombies)
    {
        if (AEnemyCharacter* Zombie = Cast<AEnemyCharacter>(ZombieActor))
        {
            Zombie->OnZombieDeath.RemoveDynamic(this, &AZombiesGameMode::OnZombieKilled); // Avoid duplicate bindings
            Zombie->OnZombieDeath.AddDynamic(this, &AZombiesGameMode::OnZombieKilled);
        }
    }
}

