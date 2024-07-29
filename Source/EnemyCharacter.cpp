// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "MyZombiesGameMode.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Ensure AnimInstanceRef is of the type UAI_AnimInstance
    AnimInstanceRef = Cast<UAI_AnimInstance>(GetMesh()->GetAnimInstance());
    if (!AnimInstanceRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("Animation instance is not of type UAI_AnimInstance!"));
    }
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called to bind functionality to input
void AEnemyCharacter::EnemyDamage(float Damage)
{
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Enemy health is now: %f"), Health);
	if(Health <= 0.0f)
	{
        UE_LOG(LogTemp, Warning, TEXT("Enemy health is now: %f"), Health);
		// Check if the AnimInstanceRef is valid before calling SetIsDead
        if (AnimInstanceRef)
        {
            AnimInstanceRef->SetIsDead(true);
        }
        // Set a timer to destroy the zombie after a delay
        GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &AEnemyCharacter::Die, 1.0f, false);
    }
}


void AEnemyCharacter::Die()
{
    Destroy();
	myGameMode = GetWorld()->GetAuthGameMode<AMyZombiesGameMode>();
    if (myGameMode)
    {
        myGameMode->CheckEnemiesAlive();
    }
}

