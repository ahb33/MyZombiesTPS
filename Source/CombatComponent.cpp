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

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    // DOREPLIFETIME(UCombatComponent, CombatState); /*Combat State registered for replication
	// for all clients no need for condition*/


	DOREPLIFETIME(UCombatComponent, bAiming);    
    DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
    DOREPLIFETIME(UCombatComponent, EquippedWeapon);

}

void UCombatComponent::Fire()
{
	// check weapon type
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	// bFireButtonPressed = bPressed;
	// if (bFireButtonPressed)
	// {
	// 	Fire();
	// }
}


void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
    if (!MainCharacter || !WeaponToEquip)
    {
        UE_LOG(LogTemp, Warning, TEXT("EquipWeapon failed: Invalid MainCharacter or WeaponToEquip."));
        return;
    }

    if (!EquippedWeapon)
    {
        EquippedWeapon = WeaponToEquip;
        AttachActorToRightHand(EquippedWeapon);
        EquippedWeapon->SetOwner(MainCharacter);

    }
    else if (!SecondaryWeapon)
    {
        SecondaryWeapon = WeaponToEquip;
        AttachWeaponToWeaponSocket(SecondaryWeapon);
        SecondaryWeapon->SetOwner(MainCharacter);
    }
    else
    {
        SwapWeapons();
    }
}

void UCombatComponent::ServerEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
    EquipWeapon(WeaponToEquip);
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


void UCombatComponent::ServerSwapWeapons_Implementation()
{
    SwapWeapons();
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




