// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h" 
#include "WeaponTypes.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Components/WidgetComponent.h"


// Sets default values
AWeapon::AWeapon()
{
	bReplicates = true;
	SetReplicateMovement(true);
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh); // sets the WeaponMesh variable to be the root component

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); /*
	if you want the pawn to run through the weapon without colliding with it */
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent); // attaches Area sphere to Weapon Mesh
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); /*Disable collision
	fore area sphere in constructor but enable it in begin play for the server */
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);


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

	DOREPLIFETIME(AWeapon, EquippedWeapon);
	DOREPLIFETIME(AWeapon, SecondaryWeapon);



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

	

	// Find the player's character in the world by iterating over all instances of the AMainCharacter class,
	// and store a pointer to the first one found in the MainCharacter variable.
	// This is necessary because some of the subsequent code requires a reference to the player's character.
	for (TActorIterator<AMainCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		MainCharacter = *ActorItr;
		break;
	}

	// Get the controller of the MainCharacter
	AController* Controller = MainCharacter->GetController();
	// modify for multiplayer
	AMyPlayerController* MyController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (MyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully retrieved player controller"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve player controller"));
	}

    if(MainCharacter->HasAuthority())
    {
        SetAmmoInMag();
        SetAmmoOnHand();
    }
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	CrossHairTrace(HitResult);

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

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}
void AWeapon::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming; 
	ServerSetAiming(bIsAiming);
}

void AWeapon::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

// void AWeapon::OnRep_Aiming()
// {
// 	if (MainCharacter && MainCharacter->IsLocallyControlled())
// 	{
// 		bAiming = true;
// 	}
// }

void AWeapon::EquipWeapon(class AWeapon* currentWeapon)
{

	if (!MainCharacter)
	{
		return;
	}

	/*
		If weapon is already equipped, equip secondary weapon else equip primary weapon else if both
		primary and secondary weapons are equipped then swap weapons
	*/

	// if no primary weapon is equipped, EquipSecondary
	// if secondary weapon is equipped, EquipPrimary
	// else swap weapon

    // If there's no primary weapon equipped, equip this one as primary.
    if (!EquippedWeapon)
    {
        EquipPrimaryWeapon(currentWeapon);
    }
    // If the primary weapon is equipped and the secondary is not, and the current weapon is not the same as the primary weapon, equip as secondary.
    else if (!SecondaryWeapon && EquippedWeapon != currentWeapon)
    {
        EquipSecondaryWeapon(currentWeapon);
    }
    // If both primary and secondary weapons are equipped, swap them.
    else if (EquippedWeapon && SecondaryWeapon)
    {
        SwapWeapons();
    }

}

void AWeapon::EquipPrimaryWeapon(class AWeapon* FirstWeapon)
{
	if(FirstWeapon == nullptr) return;
	EquippedWeapon = FirstWeapon;
	WeaponState = EWeaponState::EWS_Equipped;
    EquippedWeapon->SetOwner(MainCharacter);
    EquippedWeapon->ShowPickUpWidget(false);

    const USkeletalMeshSocket* HandSocket = MainCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
    if (HandSocket) HandSocket->AttachActor(EquippedWeapon, MainCharacter->GetMesh());


	//    // Assuming SetAmmoOnHand is setting the ammo counts properly
	//     EquippedWeapon->SetAmmoOnHand();

	//     // Make sure to update the HUD after the weapon is equipped and ammo is set
	//     EquippedWeapon->RefreshHUD();
	
	EquippedWeapon->RefreshHUD();


    SetHUDCrosshairs(0.f);
}

void AWeapon::EquipSecondaryWeapon(class AWeapon* SecondWeapon)
{
	if(SecondWeapon == nullptr) return;
	SecondaryWeapon = SecondWeapon;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachWeaponToWeaponSocket(SecondaryWeapon);
	SecondaryWeapon->SetOwner(MainCharacter);
	// Assuming SetAmmoOnHand is setting the ammo counts properly
    EquippedWeapon->SetAmmoOnHand();


	
}

// register for replicatrion
void AWeapon::SwapWeapons()
{
	UE_LOG(LogTemp, Warning, (TEXT("SwapWeapons called")));
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachActorToRightHand(EquippedWeapon);

	EquippedWeapon->RefreshHUD();

	AttachWeaponToWeaponSocket(SecondaryWeapon);

}

bool AWeapon::ShouldSwapWeapons() const
{
	return(EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
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
	if(WeaponSocket )
	{
		WeaponSocket ->AttachActor(WeaponToAttach, MainCharacter->GetMesh());
	}

}


void AWeapon::DropWeapon()
{
	if(EquippedWeapon)
	{
		// Detach the weapon from the character's hand socket
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Set the weapon's owner to nullptr
		EquippedWeapon->SetOwner(nullptr);

		// Show the pickup widget for the dropped weapon
		EquippedWeapon->ShowPickUpWidget(true);

		// Clear the equipped weapon reference
		EquippedWeapon = nullptr;
	}
}



void AWeapon::FireButtonPressed(bool bFire)
{
	bFireButtonPressed = bFire;
	if(EquippedWeapon == nullptr) return;
	if (MainCharacter && bFireButtonPressed)
	{
		FHitResult TraceHitResult;
		FVector HitTarget = CrossHairTrace(TraceHitResult);

		if(HitTarget!= FVector::ZeroVector)
		{  
			EquippedWeapon->Fire(HitTarget);

		}
	}
}

void AWeapon::Fire(const FVector& Hit)
{	
    UE_LOG(LogTemp, Warning, TEXT("Ammo Count: %d"), AmmoOnHand);
    if (!WeaponIsEmpty())
    {
        RoundFired();
        bCanFire = false;
        UE_LOG(LogTemp, Warning, TEXT("Firing Weapon"));


		// If Animation is valid and CombatState unoccupied then play animation and montage
        if (FireAnim)
        {
			UE_LOG(LogTemp, Warning, TEXT("Fire Anim is valid"));
            WeaponMesh->PlayAnimation(FireAnim, false);
            MainCharacter->PlayFireMontage(bAiming);
        }

        if (Casing)
        {
            const USkeletalMeshSocket* CasingSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
            if (CasingSocket)
            {
                FTransform CasingMeshTransform = CasingSocket->GetSocketTransform(WeaponMesh);
                UWorld* World = GetWorld();
                if (World)
                {
                    World->SpawnActor<ACasing>(Casing, CasingMeshTransform.GetLocation(), CasingMeshTransform.GetRotation().Rotator());
                    UE_LOG(LogTemp, Warning, TEXT("Bullet Casing Spawned"));
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon Cannot Fire while reloading"));
        // Unable to fire weapon
        return;
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
        // Reduce the ammo count in the specific weapon instance
        SetAmmo(CurrentAmmoOnHand - 1, GetCurrentAmmoInMag());

        // Now, update the HUD with the new ammo count
        RefreshHUD();
        UE_LOG(LogTemp, Warning, TEXT("RoundFired() ammo on hand = %d"), GetCurrentAmmoOnHand());
    } 
    else
    {
		// check if weapon is empty and log it
		if(WeaponIsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon is out of ammo"));
		}
    }
}

/*
If on client should then you should send RPC to server so 
that the server knows to check to make sure 
we can reload before informing clients they can 
reload before its time to play reload animation	
*/
void AWeapon::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Reload() function called"));

	// Check if ammo is greater than 0 and Combat State is not reloading 
	if(EquippedWeapon->AmmoInMag > 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling 2 reload functions"));

		ServerReload();
		FinishReloading(); 
	}
}

// not done yet
void AWeapon::FinishReloading()
{
	if(MainCharacter == nullptr) return;
	if(MainCharacter->HasAuthority())
	{
		// Reset combat state to allow reloading again
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (bFireButtonPressed)
	{
		bool bFire = true;
		FireButtonPressed(bFire);
	}
}

void AWeapon::ServerReload_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server Reload function called"));

	// Check If MainCharacter or equipped weapon is nullptr
	// Play Reload Montage
	// Use FMath Clamp to make clamp ammo between 0 and value between ammo in Mag and Mag capacity
	if(MainCharacter == nullptr || EquippedWeapon == nullptr) return;

    CombatState = ECombatState::ECS_Reloading;
    MainCharacter->PlayReloadMontage();

    int32 AmmoReload = AmmoToReload(); // Calculate the ammo to reload

    // Use the GetCurrentAmmoOnHand and GetCurrentAmmoInMag methods to get the correct ammo values
    // Use a new method or directly call a method on the EquippedWeapon to update its ammo counts
    EquippedWeapon->ReloadAmmo(AmmoReload);
	EquippedWeapon->RefreshHUD();
}

int32 AWeapon::AmmoToReload()
{
    if(EquippedWeapon == nullptr) 
    {
        return 0;
    }

    int32 CurrentAmmoOnHand = EquippedWeapon->GetCurrentAmmoOnHand();
    int32 CurrentAmmoInMag = EquippedWeapon->GetCurrentAmmoInMag();
    MaxAmmoOnHand = EquippedWeapon->GetMaxAmmoOnHand();

    int32 MaxReloadAmount = MaxAmmoOnHand - CurrentAmmoOnHand;
    int32 AmmoToReload = FMath::Min(MaxReloadAmount, CurrentAmmoInMag);

    return AmmoToReload;
}


void AWeapon::PickUpAmmo(EWeaponType PickupType, int32 AmmoAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("PickUpAmmo function called"));

}


void AWeapon::RefreshHUD()
{
	PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());

    if (PlayerController)
    {
        PlayerController->SetHUDAmmo(GetCurrentAmmoOnHand());
        PlayerController->SetHUDMagAmmo(GetCurrentAmmoInMag());
    }
}
void AWeapon::SetHUDAmmo(int32 Ammo)
{
    if (MainCharacter)
    {
        PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());
        if (PlayerController)
        {
            PlayerController->SetHUDAmmo(GetCurrentAmmoOnHand());
			// UE_LOG(LogTemp, Warning, TEXT("SetHUDAmmo called. AmmoOnHand: %d"), Ammo);
        }
    }
}

void AWeapon::SetHUDMagAmmo(int32 MagAmmo)
{
    if (MainCharacter)
    {
        PlayerController = Cast<AMyPlayerController>(MainCharacter->GetController());
        if (PlayerController)
        {
            PlayerController->SetHUDMagAmmo(GetCurrentAmmoInMag());
			// UE_LOG(LogTemp, Warning, TEXT("SetHUDAmmo called. Ammo in Mag is: %d"), MagAmmo);
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
	if(Owner == nullptr)
	{
		MainCharacter = nullptr;
		PlayerController = nullptr;
	}
	else
	{
		SetHUDAmmo(GetCurrentAmmoOnHand());
	}
}

void AWeapon::OnRep_EquippedWeapon()
{

}

void AWeapon::OnRep_SecondaryWeapon()
{
	
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

void AWeapon::OnRep_WeaponState()
{

}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

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

FVector AWeapon::CrossHairTrace(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
		FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;

		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition,
		CrosshairWorldDirection
		);

	if (bScreenToWorld)
	{	
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * Trace_Length;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if(TraceHitResult.bBlockingHit)  // if you get a blocking hit set result to impact point of trace
		{
			LocalHitTarget = TraceHitResult.ImpactPoint;
		}

		else
		{
			LocalHitTarget = End;
		}
	}

	return TraceHitResult.ImpactPoint;
}

void AWeapon::SetHUDCrosshairs(float Deltatime)
{
	// PlayerController = PlayerController == nullptr ? Cast<AMyPlayerController>(MainCharacter->Controller) : PlayerController;
	PlayerController = PlayerController == nullptr ? Cast<AMyPlayerController>(MainCharacter->GetController()) : PlayerController;	

	const FString ControllerClassName = MainCharacter->Controller->GetClass()->GetName();
	// UE_LOG(LogTemp, Warning, TEXT("MainCharacter's controller class name: %s"), *ControllerClassName);
	if (PlayerController == nullptr)
	{
    	// UE_LOG(LogTemp, Warning, TEXT("Failed to cast MainCharacter's controller to AMyPlayerController"));
    	return;
	}	
		if (PlayerController)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Player Controller valid"));

				
			HUD = HUD == nullptr ? Cast<AMyHUD>(PlayerController->GetHUD()) : HUD;
			if (HUD)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Weapon HUD valid"));

				if (EquippedWeapon == nullptr)
				{
					// UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon not valid in set HUD"));
					return;
				}

				// UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon valid"));

				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;

				// Calculate crosshair spread
				/*
				[0,600] -> [0,1]
				Create FVector2D representing range of motion from 0-> max walk speed
				Create FVector2D representing the velocity range from 0-> 1
				Map velocity to the FVector2D representing velocity range
				Set velocity in Z to 0.f
				FMath::GetMappedRangeValueClamped()
				set HUDs crosshair spread based on velocity factor value declared
				*/
				
				FVector2D SpeedRange(0.f, MainCharacter->GetCharacterMovement()->MaxWalkSpeed);
				FVector2D VelocityRange(0.f , 1.f);
				FVector Velocity = MainCharacter->GetVelocity();
				Velocity.Z = 0.f;

				CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(SpeedRange, VelocityRange, Velocity.Size());

				HUDPackage.CrosshairSpread = 0.5f + CrosshairVelocityFactor;

				HUD->SetHUDPackage(HUDPackage);
			}
		}

	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("Player Controller is null"));
	}
}


/*
 

Declare function EquipButton Pressed in MainCharacter that will call Equip Weapon function
Declare Equip Weapon function in Combat Functionality
Call Equip Weapon in MainCharacter.cpp and pass in variable that is holding overlapping weapon
Declare function in MainCharacter.cpp that sets the variable that will hold the overlapping weapon
This function will take pointer to Weapon function 
Set overlapping weapon variable to this pointer
This pointer can access overlapping function in Weapon class

*/

