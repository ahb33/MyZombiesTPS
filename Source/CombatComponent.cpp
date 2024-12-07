// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MainCharacter.h"
#include "MyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"   
#include "DrawDebugHelpers.h"           
#include "Engine/World.h"      
#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"         
#include "Math/UnrealMathUtility.h"
#include "GameFramework/CharacterMovementComponent.h" 

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
    MainCharacter = Cast<AMainCharacter>(GetOwner());


	// ...
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    SetHUDCrosshairs(DeltaTime);

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    DOREPLIFETIME(UCombatComponent, CombatState); /*Combat State registered for replication
	for all clients no need for condition*/


	DOREPLIFETIME(UCombatComponent, bAiming);    
    DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
    DOREPLIFETIME(UCombatComponent, EquippedWeapon);

}


void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
    if (EquippedWeapon && !EquippedWeapon->WeaponIsEmpty())
    {
        bCanFire = false;
        EquippedWeapon->Fire(HitTarget);
        // Handle firing rate and cooldown if needed
    }
}
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
    if (!MainCharacter || !WeaponToEquip)
    {
        UE_LOG(LogTemp, Warning, TEXT("EquipWeapon failed: Invalid MainCharacter or WeaponToEquip."));
        return;
    }

    if (CombatState != ECombatState::ECS_Unoccupied) return;

    // if no weapon is currently equipped set WeaponToEquip to value of EquippedWeapon
    // #1 address the nulling of overlappingWeapon and #2 refactor checking for equippedweapon and so on

    // if EquippedWeapon is valid but SecondaryWeapon is not equip Secondary weapon
    if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
    {

        SecondaryWeapon = WeaponToEquip;
        AttachWeaponToWeaponSocket(SecondaryWeapon);
        SecondaryWeapon->SetOwner(MainCharacter);
        SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);

    }
    else
    {
        
        EquippedWeapon = WeaponToEquip;
        AttachActorToRightHand(EquippedWeapon);
        EquippedWeapon->SetOwner(MainCharacter);
        EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
    }
}



void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
    if (!WeaponToEquip || !MainCharacter) return;

    SecondaryWeapon = WeaponToEquip;

    AttachWeaponToWeaponSocket(WeaponToEquip);

    // Hide the weapon's pickup widget
    WeaponToEquip->ShowPickUpWidget(false);
    WeaponToEquip->SetOwner(MainCharacter);

    UE_LOG(LogTemp, Log, TEXT("Secondary weapon equipped: %s"), *WeaponToEquip->GetName())

}

void UCombatComponent::SwapWeapons()
{
    if (!EquippedWeapon || !SecondaryWeapon) return;

    // Swap primary and secondary
    AWeapon* Temp = EquippedWeapon;
    EquippedWeapon = SecondaryWeapon;
    SecondaryWeapon = Temp;

    // Attach primary weapon to the right hand
    AttachActorToRightHand(EquippedWeapon);

    // Attach secondary weapon to the weapon socket
    AttachWeaponToWeaponSocket(SecondaryWeapon);
    EquippedWeapon->SetOwner(MainCharacter);

}

bool UCombatComponent::ShouldSwapWeapons() const
{
    if(EquippedWeapon && SecondaryWeapon) return true;

    return false;
}


// 2 issues: #1 weapon not attaching #2
void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
    if (!MainCharacter || !ActorToAttach) return;

    const USkeletalMeshSocket* HandSocket = MainCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
    if (HandSocket)
    {
        HandSocket->AttachActor(ActorToAttach, MainCharacter->GetMesh());
    }
}

void UCombatComponent::AttachWeaponToWeaponSocket(AActor* WeaponToAttach)
{
	// check if character or weapon to attach are nullptr
	// create and define const ref to USkeletalMeshSocket 
	// check if ref is valid
	// AttachActor 
	if(MainCharacter == nullptr || MainCharacter->GetMesh() == nullptr || WeaponToAttach == nullptr) return;
	const USkeletalMeshSocket* WeaponSocket  = MainCharacter->GetMesh()->GetSocketByName(FName("SecondaryWeaponSocket"));
	if(WeaponSocket)
	{
		WeaponSocket ->AttachActor(WeaponToAttach, MainCharacter->GetMesh());
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
    if(MainCharacter == nullptr || EquippedWeapon == nullptr) return;

    // The server will set the value here or in ServerSetAiming_Implementation
    bAiming = bIsAiming;
    ServerSetAiming(bAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UCombatComponent::OnRep_Aiming()
{
	if (MainCharacter && MainCharacter->IsLocallyControlled())
	{
		bAiming = true;
	}
}

void UCombatComponent::SetCombatState(ECombatState State)
{
    CombatState = State;
    OnRep_CombatState();
}

void UCombatComponent::OnRep_CombatState()
{
    if (!MainCharacter) return;

    switch (CombatState)
    {
        case ECombatState::ECS_Unoccupied:
            MainCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
            break;

        case ECombatState::ECS_Reloading:
            MainCharacter->GetCharacterMovement()->DisableMovement();
            break;

        case ECombatState::ECS_Equipping:
            break;

        default:
            break;
    }
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
    if (!MainCharacter) return;

    AMyPlayerController* PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());
    if (!PlayerController) return;

    HUD = HUD ? HUD : Cast<AMyHUD>(PlayerController->GetHUD());
    if (!HUD) return;

    HUDPackage.CrosshairsCenter = this->CrosshairsCenter;
    HUDPackage.CrosshairsLeft = this->CrosshairsLeft;
    HUDPackage.CrosshairsRight = this->CrosshairsRight;
    HUDPackage.CrosshairsTop = this->CrosshairsTop;
    HUDPackage.CrosshairsBottom = this->CrosshairsBottom;

    FVector2D SpeedRange(0.f, MainCharacter->GetCharacterMovement()->MaxWalkSpeed);
    FVector2D VelocityRange(0.f, 1.f);
    FVector Velocity = MainCharacter->GetVelocity();
    Velocity.Z = 0.f;

    CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(SpeedRange, VelocityRange, Velocity.Size());
    HUDPackage.CrosshairSpread = 0.0f + CrosshairVelocityFactor; // Adjusted base value

    HUD->SetHUDPackage(HUDPackage);
    
}
