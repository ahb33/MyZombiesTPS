// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Weapon.h"
#include "Misc/App.h"
#include "Kismet/KismetMathLibrary.h"


void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation(); // this calls the parent function to ensure code in it gets called
    if(Pawn == nullptr) // the following code is to ensure we the function does not return a nullptr which could cause a crash
    {               
        Pawn = TryGetPawnOwner(); // this function will return the Pawn belonging to this animinstance
        if (Pawn)
        {        
            MainCharacter = Cast<AMainCharacter>(Pawn);
        }/* if both checks above succeed, we will have a variable with access to the pawn in
        the forms of APawn and AMainCharacter */
    }
}

/* 
Check if Pawn returns nullptr
   If so Get Pawn Owner
Check if Pawn is possessed  
*/    

// UpdateAnimationProperties()
void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if(MainCharacter == nullptr)
    {
            MainCharacter = Cast<AMainCharacter>(Pawn); 
    }
    if(MainCharacter)

        if (MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()>0)
        {
            bIsAccelerating = true;
        }
        else
        {
            bIsAccelerating = false;
        }

    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
    }

    if(Pawn)
    {
        FVector Speed = Pawn->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f); // movement in Z does not matter
        MovementSpeed = LateralSpeed.Size();

        bIsInAir = Pawn->GetMovementComponent()->IsFalling();
        bWeaponEquipped = MainCharacter->IsWeaponEquipped(); // variable that checks if weapon is equipped
        EquippedWeapon = MainCharacter->GetEquippedWeapon(); // MainCharacter is pointer to MainCharacter  
                                                            // and EquippedWeapon is pointer to AWeapon

        bIsCrouched = MainCharacter->bIsCrouched; // variable that holds true when MainCharacter is crouched
        bAiming = MainCharacter->IsAiming();
        TurningInPlace = MainCharacter->GetTurningInPlace();

        // in order to get offset we need to know which direction we are moving and which direction we are aiming
        FRotator AimRotation = MainCharacter->GetBaseAimRotation(); // Rotation that corresponds to aim
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MainCharacter->GetVelocity()); /* Rotation
        that corresponds to movement*/
        FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
        DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
        YawOffset = DeltaRotation.Yaw;
        

        CharacterRotationLastFrame = CharacterRotation;
        CharacterRotation = MainCharacter->GetActorRotation();
        const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
        const float Target = Delta.Yaw / DeltaTime;
        const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
        Lean = FMath::Clamp(Interp, -90.f, 90.f);

        AO_Yaw = MainCharacter->GetCharacterYaw();
        AO_Pitch = MainCharacter->GetCharacterPitch();
        
        //we need to check if we have an equipped weapon, but also access to the weapon itself as well
        // as weapon and main character mesh
        if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MainCharacter->GetMesh())
        {
            LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),
            ERelativeTransformSpace::RTS_World);
            // this is in worldspace but we need to transform it to bonespace on skeleton on our mesh
            //TransformToBoneSpace: Transform a location/rotation from world space to bone relative space. 
            //This is handy if you know the location in world space for a bone attachment, as AttachComponent takes 
            //location/rotation in bone-relative space. 
            FVector OutPosition;
            FRotator OutRotation;
            MainCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(),
            FRotator::ZeroRotator, OutPosition, OutRotation);
            LeftHandTransform.SetLocation(OutPosition);
            LeftHandTransform.SetRotation(FQuat(OutRotation));

            	if (MainCharacter->IsLocallyControlled())
		        {
			        bLocallyControlled = true;
			        FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), 
                    ERelativeTransformSpace::RTS_World);
			        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), 
                    RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - MainCharacter->GetHitTarget()));


                    if(MainCharacter->GetCombatState() == ECombatState::ECS_Reloading)
                    {
                        bReloading = true;
                    }
                }
        }
    }

    if(MainCharacter)
    {
        bUseFABRIK = MainCharacter->GetCombatState() != ECombatState::ECS_Reloading;
    }


}



