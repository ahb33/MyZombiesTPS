// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "EnemyCharacter.h"
#include "MyAIController.h"



void UAI_AnimInstance::NativeInitializeAnimation()
{

    Super::NativeInitializeAnimation(); // this calls the parent function to ensure code in it gets called
    if(Pawn == nullptr) // the following code is to ensure we the function does not return a nullptr which could cause a crash
    {               
        Pawn = TryGetPawnOwner(); // this function will return the Pawn belonging to this animinstance
        if (Pawn)
        {        
            EnemyCharacter = Cast<AEnemyCharacter>(Pawn);
        }

        /* if both checks above succeed, we will have a variable with access to the pawn in
        the forms of APawn and AMainCharacter */
    }
}

void UAI_AnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    // Ensure that we always have a valid reference to Pawn.
    if (Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        // Attempt to cast once and cache the result for future frames.
        EnemyCharacter = Cast<AEnemyCharacter>(Pawn);
    }

    // Once Pawn is valid, we proceed to update the animation properties.
    if (Pawn)
    {
        // Update MovementSpeed using only the lateral components of the velocity.
        FVector LateralSpeed = FVector(Pawn->GetVelocity().X, Pawn->GetVelocity().Y, 0.f);
        float TargetSpeed = LateralSpeed.Size();

        // Directly set bIsAccelerating based on whether the current acceleration is greater than zero.
        // This assumes that EnemyCharacter and its Movement Component are valid.
        bIsAccelerating = EnemyCharacter && EnemyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;

        // Interpolate the MovementSpeed towards the TargetSpeed
        MovementSpeed = FMath::Lerp(MovementSpeed, TargetSpeed, FMath::Clamp(DeltaTime * 0.1f, 0.0f, 1.0f));

        // UE_LOG(LogTemp, Warning, TEXT("Movement Speed: %f"), MovementSpeed);


    }
}

void UAI_AnimInstance::SetRandomDyingAnimation(bool bShouldRandomize)
{
    bRandomDyingAnimation = bShouldRandomize;
}

void UAI_AnimInstance::SetIsDead(bool bDead)
{
    bIsDead = bDead;
}

void UAI_AnimInstance::SetPlayerVisibility(bool bVisible)
{
    bPlayerVisible = bVisible;
}

   