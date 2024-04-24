// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h" 
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h" 
#include "Kismet/GameplayStatics.h"
#include "MyHUD.h"
#include "MyPlayerController.h"
#include "WeaponTypes.h"
#include "CombatState.h"
#include "WeaponState.h"
#include "Casing.h"
#include "Weapon.generated.h"

#define Trace_Length 10000.f

UCLASS()
class MYZOMBIES_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	// Called every frame

	friend class AMainCharacter; /* This will enable MainCharacter to access any functions
	or variables in this class whether they are protected or private */

	virtual void Tick(float DeltaTime) override;

	void ShowPickUpWidget(bool bShowWidget);

	// Useful for accessing the actual mesh of the weapon itself
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}


	void SetAiming(bool bIsAiming);

	void EquipWeapon(class AWeapon* currentWeapon); /* this function take a pointer reference to the
	Weapon class and will be responsible for equipping the weapon */


	void EquipPrimaryWeapon(class AWeapon* FirstWeapon); /* this function take a pointer reference to the
	Weapon class and will be responsible for equipping the primary weapon */

	void EquipSecondaryWeapon(class AWeapon* SecondWeapon); /* this function take a pointer reference to the
	Weapon class and will be responsible for equipping the secondary weapon */

	void SwapWeapons();

	void AttachActorToRightHand(AActor* ActorToAttach);
	
	void AttachWeaponToWeaponSocket(AActor* WeaponToAttach);

	FVector CrossHairTrace(FHitResult& TraceHitResult); /*Structure containing information about one hit of a trace
	such as point of impact and surface normal at that point.*/ 


	/*Set Cross hair based on weapon we are using; crosshairs will be dynamic - they will be changing
	based on character's velocity and firing which will involve interpolation so pass DeltaTime */
	void SetHUDCrosshairs(float DeltaTime);

	/*Returns the properties used for network replication, 
	this needs to be overridden by all actor classes with native replicated properties*/ 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);


	/*Need to make sure owner replicated on client before we set HUD Ammo 
	as we need to access owner to set it controller.*/
	UFUNCTION()
	void OnRep_AmmoOnHand();

	/*Need to make sure owner replicated on client before we set HUD Ammo 
	as we need to access owner to set it controller.*/
	UFUNCTION()
	void OnRep_AmmoInMag();

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	// function will return true if weapon is empty and will not fire
	bool WeaponIsEmpty() const;


	void FireButtonPressed(bool bFire); // declaring this function as virtual so that child classes can override it

	virtual void Fire(const FVector& Hit); 


	// function will subtract one and check if weapon has valid owner so it can update HUD for owner
	void RoundFired(); 

	void Reload();

	void FinishReloading();


	void RefreshHUD();
	// function will get owner and player controller before displaying current Ammo 
	void SetHUDAmmo(int32);

	// function will get owner and player controller before displaying current Ammo 
	void SetHUDMagAmmo(int32);

	
	/*Call parent version of OnRep_Owner and check if Character and Controller are valid before setting
	  HUD*/
	virtual void OnRep_Owner();


	virtual void SetAmmo(int32 NewAmmoOnHand, int32 NewAmmoInMag) {};

    virtual int32 GetCurrentAmmoOnHand() const {return 0;}

    virtual int32 GetCurrentAmmoInMag() const {return 0;}

	virtual int32 GetMaxAmmoOnHand() const {return 0;}

	virtual int32 GetMagCapacity() const {return 0;}

	virtual void ReloadAmmo(int32 Ammo) {};

	virtual float GetDamage() const {return 20.f;}

	// This function will initialize ammo for all weapon types
	void SetAmmoInMag();

	void SetAmmoOnHand();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	// this function will calculate the amount to reload
	int32 AmmoToReload();


	// Function will be called within AmmoPickUp class
	void PickUpAmmo(EWeaponType WeaponType, int32 AmmoAmmount);

	EWeaponType GetWeaponType() const {return WeaponType;}


	void DropWeapon();


	UFUNCTION()
	void OnRep_CombatState();

	ECombatState SetCombatState();

	// function will return combat state
	ECombatState GetCombatState(){return CombatState;} 

	UFUNCTION()
	void OnRep_WeaponState();

	void SetWeaponState(EWeaponState WeaponState); // Create function to SetWeaponState

	EWeaponState GetWeaponState(){return WeaponState;}

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// you only want overlap events to happen on the server

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	

	
private:

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class UWidgetComponent* PickupWidget;

	UPROPERTY()
	class AMainCharacter* MainCharacter; /* variable that that will have access to MainCharacter's  
	function */ 

	UPROPERTY(Replicated)
	AMainCharacter* PlayerCharacter;

	UPROPERTY()
	class AMyPlayerController* PlayerController;


	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon; // variable that will store currently equipped weapon 

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* PrimaryWeapon; // variable that will store primary weapon

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	class AWeapon* SecondaryWeapon; // variable that will store secondary weapon


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = WeaponAnimation)
	class UAnimationAsset* FireAnim;

	UPROPERTY(Replicated)
	bool bAiming;

	bool bFireButtonPressed;

	bool bCanFire = true;

	UPROPERTY(VisibleAnywhere)
	FVector LocalHitTarget;

	/*Weapon class needs to know what class to spawn when ejecting a shell or casing*/
	UPROPERTY(EditAnywhere, Category = Casing)
	TSubclassOf<class ACasing> Casing;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsBottom;

	class AMyHUD* HUD; 

	FHUDPackage HUDPackage;

	float CrosshairVelocityFactor; // this will contribute to crosshair spread based on velocity
	
	/*When reloading there are 2 mathematical possiblities to consider: 
	1. When AmmoInMag is greater than difference between value AmmoOnHand 
	and the maximum amount AmmoOnHand can be and 
	2. When AmmoInMag is smaller than the difference between AmmoOnHand 
	and the maximum amount AmmoOnHand can be
	Therefore we need a variable that calculates the difference between the 2 
	and a function that knows how much to reload*/

	// Ammo on hand for currently equipped weapon
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_AmmoOnHand)
	int32 AmmoOnHand; 

	// Maximum Ammo that gun can hold before needing to reload; this will be different for each weapon
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_AmmoOnHand)
	int32 MaxAmmoOnHand; 


	// Amount that can be reloaded
	UPROPERTY(ReplicatedUsing = OnRep_AmmoOnHand)
	int32 ReloadAmount; 


	// Ammo in mag for currently equipped weapon
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_AmmoInMag)
    int32 AmmoInMag;

	// Maximum amount of ammo the magazine can store
	UPROPERTY(VisibleAnywhere)
	int32 MagCapacity;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState = EWeaponState::EWS_Unequipped; // Create and initialize variable of type EWeaponState
};
