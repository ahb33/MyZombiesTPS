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
#include "HealthPickUp.h"
#include "AmmoPickUp.h"
#include "MyPlayerController.h"
#include "Animation/AnimInstance.h" 
#include "WeaponTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"

AMainCharacter::AMainCharacter()
    : CameraBoom(CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")))
    , FollowCamera(CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")))
    , PrimaryWeapon(nullptr)
    , SecondaryWeapon(nullptr)
    , OverlappingWeapon(nullptr)
    , FireMontage(nullptr)
    , ReloadMontage(nullptr)
    , AO_Yaw(0.f)
    , AO_Pitch(0.f)
    , InterpAO_Yaw(0.f)
    , TurningInPlace(ETurningInPlace::ETIP_NotTurning)
    , PlayerHealth(100.f)
    , MaxHealth(100.f)
    , OverlappingItem(nullptr)
    , pickUpHealth(nullptr)
    , MyPlayerController(nullptr)
    , MyGameHUD(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    // Attach the camera boom to the mesh to allow it to follow the character's movements (e.g., crouching)
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 400.f;
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Attach the follow camera to the camera boom
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Character movement settings
    bUseControllerRotationYaw = false; // Let the character movement handle rotation
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true; // Enable crouching
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize Player Controller
    MyPlayerController = Cast<AMyPlayerController>(GetController());
    if (!MyPlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to initialize MyPlayerController"));
    }
    else
    {
        MyPlayerController->SetHUDHealth(PlayerHealth, MaxHealth);
    }

    // Initialize HUD
    MyGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    if (!MyGameHUD)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to initialize MyGameHUD"));
    }
    else
    {
        MyGameHUD->AddCharacterStats();
    }
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

    DOREPLIFETIME(AMainCharacter, PrimaryWeapon);
    DOREPLIFETIME(AMainCharacter, SecondaryWeapon);
    DOREPLIFETIME(AMainCharacter, PlayerHealth);




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
	if(PrimaryWeapon)
	{
		PrimaryWeapon->MainCharacter = this;
	}
}

// Movement Functions
void AMainCharacter::MoveForward(float value)
{
    if (Controller != nullptr && value != 0.f)
    {
        const FRotator yawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(direction, value);
    }
}

void AMainCharacter::MoveRight(float value)
{
    if (Controller != nullptr && value != 0.f)
    {
        const FRotator yawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(direction, value);
    }
}

void AMainCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AMainCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}	

void AMainCharacter::EquipButtonPressed()
{
    if (OverlappingWeapon)
    {
        if (HasAuthority())
        {
            EquipWeapon(OverlappingWeapon);
        }
        else
        {
            ServerEquipButtonPressed();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon to equip."));
    }
}



void AMainCharacter::ServerEquipButtonPressed_Implementation()
{
    if (OverlappingWeapon)
    {
        EquipWeapon(OverlappingWeapon);
    }
}

void AMainCharacter::EquipWeapon(AWeapon* NewWeapon)
{
    if (!NewWeapon) return;

    // Prevent equipping the same weapon again
    if (NewWeapon == PrimaryWeapon || NewWeapon == SecondaryWeapon) return;

    NewWeapon->SetOwner(this);

    if (!PrimaryWeapon)
    {
        // Equip as primary weapon
        PrimaryWeapon = NewWeapon;
        NewWeapon->EquipPrimaryWeapon(this);
    }
    else if (!SecondaryWeapon)
    {
        // Equip as secondary weapon
        SecondaryWeapon = NewWeapon;
        NewWeapon->EquipSecondaryWeapon(this);
    }
    else
    {
        // Swap weapons
        SwapWeapons();
    }

    // Clear overlapping weapon after equipping
    SetOverlappingWeapon(nullptr);
}

void AMainCharacter::OnRep_PrimaryWeapon()
{
    // Handle UI updates or logic specific to the equipped weapon
    if (PrimaryWeapon)
    {
        PrimaryWeapon->ShowPickUpWidget(false);
        PrimaryWeapon->SetOwner(this);
    }

    // Example: Refresh HUD with new weapon stats
    if (IsLocallyControlled())
    {
        PrimaryWeapon->RefreshHUD();
    }
}

void AMainCharacter::SwapWeapons()
{
    if (!PrimaryWeapon || !SecondaryWeapon) return;

    // Swap primary and secondary
    AWeapon* Temp = PrimaryWeapon;
    PrimaryWeapon = SecondaryWeapon;
    SecondaryWeapon = Temp;

    PrimaryWeapon->EquipPrimaryWeapon(this);
    SecondaryWeapon->EquipSecondaryWeapon(this);
}

bool AMainCharacter::IsWeaponEquipped() const
{
    return (PrimaryWeapon != nullptr);
}


void AMainCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
    if (OverlappingWeapon)
    {
        OverlappingWeapon->ShowPickUpWidget(false);
    }

    OverlappingWeapon = Weapon;

    if (IsLocallyControlled() && OverlappingWeapon)
    {
        OverlappingWeapon->ShowPickUpWidget(true);
    }
}

void AMainCharacter::OnRep_OverlappingWeapon()
{
    if (OverlappingWeapon)
    {
        OverlappingWeapon->ShowPickUpWidget(true);
    }
	else
	{
		OverlappingWeapon->ShowPickUpWidget(false);
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



void AMainCharacter::PickUpButtonPressed()
{
	if (pickUpHealth && IsValid(pickUpHealth))
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling AddHealth on pickUpHealth"));
		pickUpHealth->AddHealth(PlayerHealth, MaxHealth);
		pickUpHealth = nullptr; // Set the pickup reference to null
	}
	else

	{
		UE_LOG(LogTemp, Warning, TEXT("No overlapping health pickup"));
	}

}


void AMainCharacter::FireButtonPressed()
{
	if(PrimaryWeapon)
	{
		PrimaryWeapon->FireButtonPressed(true);
	}

}

void AMainCharacter::FireButtonReleased()
{
	if(PrimaryWeapon)
	{
		PrimaryWeapon->FireButtonPressed(false);
	}
}

void AMainCharacter::ReloadButtonPressed()
{
	if(!PrimaryWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Weapon is equipped"));
		return;
	}

	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimaryWeaponis valid"));
		PrimaryWeapon->Reload();
	}


}

void AMainCharacter::AimButtonPressed()
{	
	if(PrimaryWeapon)
	{
		PrimaryWeapon->SetAiming(true);
	}	
}


void AMainCharacter::AimButtonReleased()
{
	if(PrimaryWeapon)
	{
		PrimaryWeapon->SetAiming(false);
	}	
}

bool AMainCharacter::IsAiming() const
{
	return (PrimaryWeapon&& PrimaryWeapon->bAiming);
}

bool AMainCharacter::IsReloading() const
{
	bool reloading = (PrimaryWeapon&& PrimaryWeapon->bReloading);
    UE_LOG(LogTemp, Warning, TEXT("IsReloading() called, bReloading = %s"), reloading ? TEXT("true") : TEXT("false"));
    return reloading;

}


void AMainCharacter::PlayFireMontage(bool bAiming)
{
    if (PrimaryWeapon == nullptr) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && FireMontage)
    {
        FName SectionName = bAiming ? FName("FireIronSight") : FName("RifleHip");
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void AMainCharacter::PlayReloadMontage()
{
	if (PrimaryWeapon == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ReloadMontage)
	{
	
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance And ReloadMontage correct") );
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

        EWeaponType WeaponType = PrimaryWeapon->GetWeaponType();
        if (WeaponType != EWeaponType::EWT_AssaultRifle && WeaponType != EWeaponType::EWT_Shotgun)
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid Weapon Type!"));
            return;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon Type is valid"));
        }
        switch (WeaponType)
        {
            case EWeaponType::EWT_AssaultRifle:
                UE_LOG(LogTemp, Warning, TEXT("Assault Rifle enum called"));
                SectionName = FName("Rifle");
                break;
            case EWeaponType::EWT_Shotgun:
                SectionName = FName("Shotgun");
                break;
            // Add cases for other weapon types
            default:
                UE_LOG(LogTemp, Error, TEXT("Unknown Weapon Type: %d"), static_cast<int32>(WeaponType));
                return; // Early exit to avoid calling with an invalid SectionName
        }

        AnimInstance->Montage_JumpToSection(SectionName);
	}
}


float AMainCharacter::GetReloadDuration() const
{
    if (this->ReloadMontage)
    {
        return this->ReloadMontage->GetPlayLength();
    }
    return 0.0f;
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

// Aim Offset Functions
void AMainCharacter::AimOffset(float DeltaTime)
{
    if (PrimaryWeapon == nullptr) return;

    FVector velocity = GetVelocity();
    FVector lateralSpeed = FVector(velocity.X, velocity.Y, 0.f);
    float movementSpeed = lateralSpeed.Size();

    bool bIsInAir = GetCharacterMovement()->IsFalling();

    if (movementSpeed == 0.f && !bIsInAir)
    {
        FRotator currentAimRotation(0.f, GetBaseAimRotation().Yaw, 0.f);
        FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, StartingAimRotation);
        AO_Yaw = deltaAimRotation.Yaw;
        bUseControllerRotationYaw = false;
        TurnInPlace(DeltaTime);
    }
    else
    {
        StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
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
	if(PrimaryWeapon== nullptr) return FVector();
	return PrimaryWeapon->LocalHitTarget;
}

