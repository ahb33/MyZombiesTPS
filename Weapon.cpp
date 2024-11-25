// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h" 
#include "WeaponTypes.h"
#include "MyHUD.h"
#include "MyPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Components/WidgetComponent.h"



// Sets default values
AWeapon::AWeapon()
    : bAiming(false),
      AmmoOnHand(0),
      MaxAmmoOnHand(0),
      ReloadAmount(0),
      AmmoInMag(0),
      MagCapacity(0),
      CombatState(ECombatState::ECS_Unoccupied),
      WeaponState(EWeaponState::EWS_Unequipped),
      bReloading(false),
      bFireButtonPressed(false),
      bCanFire(true),
      LocalHitTarget(FVector::ZeroVector),
      CrosshairVelocityFactor(0.0f)
{
    // Enable replication and ticking
    bReplicates = true;
    SetReplicateMovement(true);
    PrimaryActorTick.bCanEverTick = true;

    // Weapon Mesh initialization
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh); // Set WeaponMesh as root component.
    WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Area Sphere initialization
    AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
    AreaSphere->SetupAttachment(RootComponent);
    AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Pickup Widget initialization
    PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
    PickupWidget->SetupAttachment(RootComponent);
    PickupWidget->SetVisibility(false);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, bAiming);

	/*We'd like ammo on hand and the ammo in the magazine to be replicated on all clients in case one player picked up a used weapon
	  then you want the ammo count on that weapon to remain unchanged
	  */
	DOREPLIFETIME_CONDITION(AWeapon, AmmoOnHand, COND_OwnerOnly);  // Ammo will only replicate to owning client

	DOREPLIFETIME_CONDITION(AWeapon, AmmoInMag, COND_OwnerOnly);
	DOREPLIFETIME(AWeapon, CombatState); /*Combat State registered for replication
	for all clients no need for condition*/

	DOREPLIFETIME(AWeapon, PlayerCharacter);
    DOREPLIFETIME(AWeapon, MaxAmmoOnHand);
    DOREPLIFETIME(AWeapon, ReloadAmount);
    DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, CombatState);
    DOREPLIFETIME(AWeapon, WeaponType);
	




}
// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);

	}
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	


    if(HasAuthority())
    {
        SetAmmoInMag();
        SetAmmoOnHand();
    }
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	SetHUDCrosshairs(DeltaTime);
}				

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* LocalMainCharacter = Cast<AMainCharacter>(OtherActor);
	if(LocalMainCharacter && PickupWidget)
	{
		LocalMainCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* LocalMainCharacter = Cast<AMainCharacter>(OtherActor);
	if(LocalMainCharacter)
	{
		LocalMainCharacter->SetOverlappingWeapon(nullptr);
	}

}

void AWeapon::InitializeCharacterAndController()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWeapon::InitializeCharacterAndController - Owner is null"));
        return;
    }

    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OwnerActor is valid"));
    }

    MainCharacter = Cast<AMainCharacter>(OwnerActor);
    if (!MainCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWeapon::InitializeCharacterAndController - MainCharacter is null"));
        return;
    }

    // Ensure HUD updates for the owning player
    RefreshHUD();
}


// void AWeapon::OnRep_Aiming()
// {
// 	if (MainCharacter && MainCharacter->IsLocallyControlled())
// 	{
// 		bAiming = true;
// 	}
// }


void AWeapon::EquipPrimaryWeapon(AMainCharacter* Character)
{
    if (!Character) return;

    SetOwner(MainCharacter);
    MainCharacter = Character;
    WeaponState = EWeaponState::EWS_Equipped;

    // Attach to the right hand socket
    const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
    if (HandSocket)
    {
        HandSocket->AttachActor(this, Character->GetMesh());
    }

    // Hide the pickup widget
    ShowPickUpWidget(false);

    // Update HUD
    RefreshHUD();
}

void AWeapon::EquipSecondaryWeapon(AMainCharacter* Character)
{
    if (!Character) return;

    SetOwner(Character);
    MainCharacter = Character; // Set MainCharacter here
    WeaponState = EWeaponState::EWS_EquippedSecondary;

    // Attach to the secondary weapon socket (e.g., on the back)
    const USkeletalMeshSocket* SecondaryWeaponSocket = Character->GetMesh()->GetSocketByName(FName("SecondaryWeaponSocket"));
    if (SecondaryWeaponSocket)
    {
        SecondaryWeaponSocket->AttachActor(this, Character->GetMesh());
    }

    // Hide the pickup widget
    ShowPickUpWidget(false);
}



bool AWeapon::ShouldSwapWeapons() const
{
    if (MainCharacter)
    {
        return (MainCharacter->GetPrimaryWeapon() != nullptr && MainCharacter->GetSecondaryWeapon() != nullptr);
    }
    return false;
}

void AWeapon::AttachActorToRightHand(AActor* ActorToAttach)
{
	if (MainCharacter == nullptr || MainCharacter->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = MainCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, MainCharacter->GetMesh());
	}
}

void AWeapon::AttachWeaponToWeaponSocket(AActor* WeaponToAttach)
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


void AWeapon::DropWeapon()
{
    // Detach the weapon from the character's hand socket
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // Set the weapon's owner to nullptr
    SetOwner(nullptr);

    // Show the pickup widget for the dropped weapon
    ShowPickUpWidget(true);
}



void AWeapon::FireButtonPressed(bool bFire)
{
	bFireButtonPressed = bFire;
	if (MainCharacter && bFireButtonPressed)
	{
		FHitResult TraceHitResult;
		FVector HitTarget = CrossHairTrace(TraceHitResult);
        Fire(HitTarget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
    if (!WeaponIsEmpty()) // if weapon is not empty and bCanFire is true
    {
        RoundFired();

        // Perform line trace from muzzle to HitTarget
        FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("MuzzleFlash"));
        FVector Direction = (HitTarget - MuzzleLocation).GetSafeNormal();
        FVector End = MuzzleLocation + (Direction * Trace_Length);

        FHitResult WeaponTraceHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(MainCharacter);
        Params.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, MuzzleLocation, End, ECC_Visibility, Params))
        {
            // Process hit (apply damage, spawn impact effects)
        }
        else
        {
            // No hit, bullet continues to max range
        }

        // Rest of firing logic (e.g., spawning projectiles)
    }
}
/*
	Function should decrement ammo everytime you fire
	Function should check if weapon has valid owner
	Function should make sure ammo count doesn't go below 0

*/

bool AWeapon::WeaponIsEmpty() const
{
    return GetCurrentAmmoOnHand() == 0;
}

void AWeapon::RoundFired()
{
	int32 CurrentAmmoOnHand = GetCurrentAmmoOnHand();

    if (CurrentAmmoOnHand > 0) 
    {
        UE_LOG(LogTemp, Warning, TEXT("RoundFired called"));
        // Reduce the ammo count in the specific weapon instance
        SetAmmo(CurrentAmmoOnHand - 1, GetCurrentAmmoInMag());

        // Now, update the HUD with the new ammo count
        RefreshHUD();
    } 
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon is out of ammo"));
    }
}

/*
If on client should then you should send RPC to server so 
that the server knows to check to make sure 
we can reload before informing clients they can 
reload before its time to play reload animation	
*/

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::SetAiming(bool bIsAiming)
{
    if (!HasAuthority())
    {
        // We are on the client, so request the server to set the value
        ServerSetAiming(bIsAiming);
    }

    // The server will set the value here or in ServerSetAiming_Implementation
    bAiming = bIsAiming;
}


void AWeapon::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void AWeapon::Reload()
{

    UE_LOG(LogTemp, Warning, TEXT("Reload() function called"));

    // Check if ammo is greater than 0
    if (AmmoInMag > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Calling reload functions"));
        ServerReload();
    }
}

void AWeapon::ServerReload_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Server Reload function called"));

    if (MainCharacter == nullptr) return;

    CombatState = ECombatState::ECS_Reloading;

    MainCharacter->PlayReloadMontage();

    int32 AmmoReload = AmmoToReload();

    // Update ammo counts
    ReloadAmmo(AmmoReload);
    RefreshHUD();
    bReloading = true;

    float ReloadDuration = MainCharacter->GetReloadDuration();
    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeapon::FinishReloading, ReloadDuration, false);
}

void AWeapon::FinishReloading()
{
    UE_LOG(LogTemp, Warning, TEXT("FinishReloading() function called"));
    if (MainCharacter == nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("MainCharacter is nullptr"));
        return;
    }
    if (MainCharacter->HasAuthority())
    {
        CombatState = ECombatState::ECS_Unoccupied;
        UE_LOG(LogTemp, Warning, TEXT("CombatState set to ECS_Unoccupied"));
    }
    if (bFireButtonPressed)
    {
        bool bFire = true;
        FireButtonPressed(bFire);
        UE_LOG(LogTemp, Warning, TEXT("FireButtonPressed called with bFire = true"));
    }

	bReloading = false;
	UE_LOG(LogTemp, Warning, TEXT("bReloading set to false in FinishReloading"));
}


int32 AWeapon::AmmoToReload()
{
    int32 CurrentAmmoOnHand = GetCurrentAmmoOnHand();
    int32 CurrentAmmoInMag = GetCurrentAmmoInMag();
    MaxAmmoOnHand = GetMaxAmmoOnHand();

    int32 MaxReloadAmount = MaxAmmoOnHand - CurrentAmmoOnHand;
    int32 AmmoToReload = FMath::Min(MaxReloadAmount, CurrentAmmoInMag);

    return AmmoToReload;
}


void AWeapon::PickUpAmmo(EWeaponType PickupType, int32 AmmoPickupAmount)
{
    UE_LOG(LogTemp, Warning, TEXT("PickUpAmmo function called"));

    int32 CurrentAmmoOnHand = GetCurrentAmmoOnHand();
    MaxAmmoOnHand = GetMaxAmmoOnHand();

    // Calculate the new ammo amount on hand
    int32 NewAmmoOnHand = FMath::Min(CurrentAmmoOnHand + AmmoPickupAmount, MaxAmmoOnHand);

    SetAmmo(NewAmmoOnHand, AmmoInMag);
    RefreshHUD();
}


void AWeapon::RefreshHUD()
{
    if (MainCharacter)
    {
        AMyPlayerController* PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());
        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController valid within RefreshHUD"));
            PlayerController->SetHUDAmmo(GetCurrentAmmoOnHand());
            PlayerController->SetHUDMagAmmo(GetCurrentAmmoInMag());
        }
    }
}
void AWeapon::SetHUDAmmo(int32 Ammo)
{
    if (MainCharacter)
    {
        AMyPlayerController* PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());
        if (PlayerController)
        {
            PlayerController->SetHUDAmmo(Ammo);
        }
    }
}

void AWeapon::SetHUDMagAmmo(int32 MagAmmo)
{
    if (MainCharacter)
    {
        AMyPlayerController* PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());
        if (PlayerController)
        {
            PlayerController->SetHUDMagAmmo(MagAmmo);
        }
    }
}

// replicating ammo on client
void AWeapon::OnRep_AmmoOnHand()
{	
	if(MainCharacter == nullptr)
	{
		Cast<AMainCharacter>(GetOwner());
		SetHUDAmmo(GetCurrentAmmoOnHand());
	}
}

// replicating ammo on client
void AWeapon::OnRep_AmmoInMag()
{	
	if(MainCharacter == nullptr)
	{
		Cast<AMainCharacter>(GetOwner());
		SetHUDMagAmmo(AmmoInMag);
	}


}

void AWeapon::SetAmmoInMag()
{
	AmmoInMag = GetCurrentAmmoInMag();
}

void AWeapon::SetAmmoOnHand()
{
	AmmoOnHand = GetCurrentAmmoOnHand();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
    if (GetOwner() == nullptr)
    {
        MainCharacter = nullptr;
    }
    else
    {
        MainCharacter = Cast<AMainCharacter>(GetOwner());
        if (MainCharacter)
        {
            // Initialize any other necessary references
            RefreshHUD();
        }
    }
}


void AWeapon::SetCombatState(ECombatState NewState)
{
	CombatState = NewState;
	OnRep_CombatState();

}

void AWeapon::OnRep_CombatState()
{
	switch (CombatState)
	{
		case ECombatState::ECS_Reloading:
			Reload();
			break;
		
		case ECombatState::ECS_Unoccupied:
			if(bFireButtonPressed)
			{
				bool bFire = true;
				FireButtonPressed(bFire);
			}
			break;
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
    OnRep_WeaponState();

}
void AWeapon::OnRep_WeaponState()
{

	switch(WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetEnableGravity(false);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Perform additional actions when the weapon is equipped
			break;

		case EWeaponState::EWS_EquippedSecondary:
			// could refactor block of code below and above into one function -
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetEnableGravity(false);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            break;

		case EWeaponState::EWS_Dropped:
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			WeaponMesh->SetSimulatePhysics(true);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			// Perform additional actions when the weapon is unequipped
			break;
		//case EWeaponState::EWS_EquippedSecondary:
		//	OnEquippedSecondary();
		//	break;

	}
}

/*
In this function we will use the camera's location as starting point of line trace



*/
FVector AWeapon::CrossHairTrace(FHitResult& TraceHitResult)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController == nullptr) return FVector::ZeroVector;

    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // Offset the start location to avoid clipping into geometry
    FVector Start = CameraLocation + (CameraRotation.Vector() * 50.0f); // Offset by 10 units forward
    FVector End = Start + (CameraRotation.Vector() * Trace_Length);

    // Ignore self and weapon
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(MainCharacter);
    Params.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
    if (GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility, Params))
    {
        return TraceHitResult.ImpactPoint;
    }
    else
    {
        // If no hit, return the end point
        return End;
    }

}

void AWeapon::SetHUDCrosshairs(float DeltaTime)
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