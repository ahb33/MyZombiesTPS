// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Weapon.h"
#include "MyHUD.h"
#include "Animation/AnimInstance.h" 
#include "WeaponTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh()); /* attach to mesh not root component
	                                     	since we will be running and crouching */
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true; // set to true to be able rotate camera around player
	// now that we've created cameraboom we can create camera

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); /* This will attach
	camera to camera boom */

	myWeapon = CreateDefaultSubobject<AWeapon>(TEXT("Weapon")); /* this variable
	will handle all combat related functionality for our character */

	// pickUpHealth = CreateDefaultSubobject<AHealthPickUp>(TEXT("HealthBar")); /* this variable
	// will handle all health related functionality for our character */
	// Weapon->SetIsReplicated(true); // 


	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

} 


// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

    // Validate and cast the player controller
    MyPlayerController = Cast<AMyPlayerController>(Controller);
    if (MyPlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Main character's player controller is valid"));
        MyPlayerController->SetHUDHealth(PlayerHealth, MAXHealth);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Main character's player controller is null"));
    }

	
    // Validate and cast the HUD
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AHUD* HUD = PlayerController->GetHUD())
        {
            MyGameHUD = Cast<AMyHUD>(HUD);
            if (MyGameHUD)
            {
                MyGameHUD->AddCharacterStats();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("MyGameHUD is null"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HUD is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null"));
    }


	// pickUpHealth = Cast<AHealthPickUp>(UGameplayStatics::GetActorOfClass(this, AHealthPickUp::StaticClass()));

}



// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);


	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn Right"), this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMainCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMainCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Released, this, &AMainCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMainCharacter::PickUpButtonPressed);



}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AMainCharacter, PlayerHealth);
	DOREPLIFETIME(AMainCharacter, MAXHealth);




}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{				
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
				 
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(myWeapon)
	{
		myWeapon->MainCharacter = this;
	}
}

void AMainCharacter::MoveForward(float value)
{
	if(Controller != nullptr && value != 0.f)
	{	
		// we will change controller rotation which may 
		// have different rotation from root component. 
		// We want to move forward in controllers forward direction not character forward direction
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f); // we will use this FRotator to get forward vector

		const FVector Direction( FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		AddMovementInput(Direction, value);
	}
}

void AMainCharacter::MoveRight(float value)
{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f); // we will use this FRotator to get forward vector

		const FVector Direction( FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));		
		AddMovementInput(Direction, value);
}

void AMainCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AMainCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}	

void AMainCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	// if you overlap another weapon and myWeapon is valid, i.e weapon is already equipped then dont
	// set myWeapon just yet
	if(OverlappingWeapon) // OverlappingWeapon is pointer to AWeapon declared in header file
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}

	OverlappingWeapon = Weapon; // assigning the value of pointer passed into function to OverlappingWeapon

	if (IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}

}

void AMainCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}


void AMainCharacter::SetOverlappingItem(APickUp* PickUp)
{
	if (OverlappingItem)
	{
		OverlappingItem->ShowPickUpWidget(false);
	}

	OverlappingItem = PickUp;

	if(IsLocallyControlled())
	{
		if (PickUp)
		{
			PickUp->ShowPickUpWidget(true);

			pickUpHealth = Cast<AHealthPickUp>(PickUp); // Assign the value to the member variable
		}
	}

}

bool AMainCharacter::IsWeaponEquipped()
{
	return (myWeapon && myWeapon->EquippedWeapon);
}


void AMainCharacter::ServerEquipButtonPressed_Implementation()
{
    
}
void AMainCharacter::EquipButtonPressed()
{
	
	if(OverlappingWeapon)
	{
		if(myWeapon && HasAuthority())
		{
			myWeapon->EquipWeapon(OverlappingWeapon);
		}

	}

	else if (myWeapon->ShouldSwapWeapons())
	{
		myWeapon->SwapWeapons();
	}
}


void AMainCharacter::PickUpButtonPressed()
{
	if (pickUpHealth && IsValid(pickUpHealth))
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling AddHealth on pickUpHealth"));
		pickUpHealth->AddHealth(PlayerHealth, MAXHealth);
		// pickUpHealth->Destroy(); // Destroy the pickup after adding health
		// pickUpHealth = nullptr; // Set the pickup reference to null
	}
	else

	{
		UE_LOG(LogTemp, Warning, TEXT("No overlapping health pickup"));
	}

}


void AMainCharacter::FireButtonPressed()
{
	if(myWeapon)
	{
		myWeapon->FireButtonPressed(true);
	}

}

void AMainCharacter::FireButtonReleased()
{
	if(myWeapon)
	{
		myWeapon->FireButtonPressed(false);
	}
}

void AMainCharacter::ReloadButtonPressed()
{
	if(!myWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Weapon is equipped"));
		return;
	}

	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("myWeapon is valid"));
		myWeapon->Reload();
	}


}

void AMainCharacter::AimButtonPressed()
{	
	if(myWeapon)
	{
		myWeapon->SetAiming(true);
	}	
}


void AMainCharacter::AimButtonReleased()
{
	if(myWeapon)
	{
		myWeapon->SetAiming(false);
	}	
}

bool AMainCharacter::IsAiming()
{
	return (myWeapon && myWeapon->bAiming);
}

bool AMainCharacter::IsReloading()
{
	bool reloading = (myWeapon && myWeapon->bReloading);
    UE_LOG(LogTemp, Warning, TEXT("IsReloading() called, bReloading = %s"), reloading ? TEXT("true") : TEXT("false"));
    return reloading;

}


void AMainCharacter::PlayFireMontage(bool bAiming)
{
	if (myWeapon == nullptr || myWeapon->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireMontage)
	{

		FName SectionName;
		// SectionName = bAiming ? FName("FireIronSight") : FName("RifleHip");
		if(bAiming)
		{
			SectionName = FName("FireIronSight");

		}
		else
		{
			SectionName = FName("RifleHip");			
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMainCharacter::PlayReloadMontage()
{
	if (myWeapon == nullptr || myWeapon->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ReloadMontage)
	{
	
		FName SectionName;
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance And ReloadMontage correct") );
		AnimInstance->Montage_Play(ReloadMontage);

		switch (myWeapon->EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("HipReload");
				break;
			// case EWeaponType::EWT_Pistol:
			// 	SectionName = FName("Pistol");
			// 	break;
			// case EWeaponType::EWT_Shotgun:
			// 	SectionName = FName("Shotgun");
			// 	break;
			// Add more cases for other weapon types as needed
			default:
				// Handle unknown weapon types here
				break;
		}	
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


float AMainCharacter::GetReloadDuration()
{
    if (ReloadMontage)
    {
        // Return the duration of the reload montage
        return ReloadMontage->GetPlayLength();
    }
    return 0.0f;
}


AWeapon *AMainCharacter::GetEquippedWeapon()
{
    if(myWeapon == nullptr) return nullptr; 
	return myWeapon->GetWeapon();

}

void AMainCharacter::CrouchButtonPressed()
{
	if(bIsCrouched) 
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AMainCharacter::AimOffset(float DeltaTime)
{
	// First check if you are equipping the weapon
	if(myWeapon && myWeapon->EquippedWeapon == nullptr) return;
	/*get vector for velocity using Unreal's GetVelocity function
	get vector for lateral speed using the x and y of vector you just created
	now you can create a float for the character's lateral speed by obtaining the size of the vector
	you just created */ 

	    FVector Velocity = GetVelocity();
        FVector LateralSpeed = FVector(Velocity.X, Velocity.Y, 0.f); // movement in Z does not matter
        float MovementSpeed = LateralSpeed.Size();

		/*create variable to check if in air */
		bool bIsInAir = GetCharacterMovement()->IsFalling();

		/*check if standing still and not falling*/
		if (MovementSpeed == 0.f && !bIsInAir) // standing still, not jumping
		{
			/*Need to be able to calculate Delta Rotation
			create variable for starting rotation and aiming rotation*/

			FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
			FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
			AO_Yaw = DeltaAimRotation.Yaw;
			bUseControllerRotationYaw = false;
			TurnInPlace(DeltaTime);
	
		}

		/*check if running or jumping*/
		if(MovementSpeed > 0 || bIsInAir)
		{
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
			//We need to keep AO_Yaw 0 as we keep moving
			AO_Yaw = 0.f;
			bUseControllerRotationYaw = true;
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		AO_Pitch = GetBaseAimRotation().Pitch;
}

void AMainCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AMainCharacter::OnRep_Health()
{

}


FVector AMainCharacter::GetHitTarget() const
{
	if(myWeapon == nullptr) return FVector();
	return myWeapon->LocalHitTarget;
}

