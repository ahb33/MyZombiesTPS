// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h" 
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponTypes.h"
#include "CombatState.h"
#include "WeaponState.h"
#include "MyHUD.h"
#include "Delegates/DelegateCombinations.h"
#include "Casing.h"
#include "Weapon.generated.h"

#define Trace_Length 10000.f




UCLASS()
class MYZOMBIES_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
    /** Constructor and Lifecycle */
    AWeapon();  // Sets default values for this actor's properties.
    virtual void Tick(float DeltaTime) override;  // Called every frame.

    /** Friend Class Access */
    friend class AMainCharacter; // Allows AMainCharacter to access private/protected members.

    /** UI Interaction */
    void ShowPickUpWidget(bool bShowWidget);

    /** Weapon Equipping */
	void EquipPrimaryWeapon(AMainCharacter* Character);
	void EquipSecondaryWeapon(AMainCharacter* Character);


    void DropWeapon();

    /** HUD and Crosshair */
    void SetHUDCrosshairs(float DeltaTime);
    void RefreshHUD();
    void SetHUDAmmo(int32 Ammo);
    void SetHUDMagAmmo(int32 MagAmmo);

    /** Weapon Firing */
    void FireButtonPressed(bool bFire);
    virtual void Fire(const FVector& Hit); // Virtual for child class overrides.
    void RoundFired();

    /** Weapon Reloading */
    void Reload();
    void FinishReloading();
    int32 AmmoToReload();
	virtual void ReloadAmmo(int32 Ammo) {};

    /** Weapon States and Combat */
    void SetCombatState(ECombatState NewState);
    ECombatState GetCombatState() const { return CombatState; }
    void SetWeaponState(EWeaponState NewState);
    EWeaponState GetWeaponState() const { return WeaponState; }

    /** Ammo and Damage */
    void SetAmmoInMag();
    void SetAmmoOnHand();
    virtual void SetAmmo(int32 NewAmmoOnHand, int32 NewAmmoInMag)
    {
        AmmoOnHand = NewAmmoOnHand;
        AmmoInMag = NewAmmoInMag;
    }
    virtual int32 GetCurrentAmmoOnHand() const { return AmmoOnHand; }
    virtual int32 GetCurrentAmmoInMag() const { return AmmoInMag; }
    virtual int32 GetMaxAmmoOnHand() const { return MaxAmmoOnHand; }
    virtual int32 GetMagCapacity() const { return MagCapacity; }
    virtual float GetDamage() const { return 20.f; }

    /** Utilities */
    bool WeaponIsEmpty() const;
    bool ShouldSwapWeapons() const;
    FVector CrossHairTrace(FHitResult& TraceHitResult);
    void AttachActorToRightHand(AActor* ActorToAttach);
    void AttachWeaponToWeaponSocket(AActor* WeaponToAttach);
    void PickUpAmmo(EWeaponType WeaponType, int32 AmmoAmount);
    EWeaponType GetWeaponType() const { return WeaponType; }
	// Useful for accessing the actual mesh of the weapon itself
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	void SetAiming(bool bIsAiming);
	void InitializeCharacterAndController();


    /** Replication */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnRep_Owner();
    UFUNCTION()
    void OnRep_AmmoOnHand();
    UFUNCTION()
    void OnRep_AmmoInMag();
    UFUNCTION()
    void OnRep_CombatState();
    UFUNCTION()
    void OnRep_WeaponState();

    /** Server Functions */
    UFUNCTION(Server, Reliable)
    void ServerSetAiming(bool bIsAiming);
    UFUNCTION(Server, Reliable)
    void ServerReload();

protected:

    /** Lifecycle */
    virtual void BeginPlay() override;

    /** Overlap Events */
    UFUNCTION()
    virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
    bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
private:


    /** Core Components */
    UPROPERTY(VisibleAnywhere, Category = Weapon)
    USphereComponent* AreaSphere;

    UPROPERTY(VisibleAnywhere, Category = Weapon)
    UWidgetComponent* PickupWidget;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Weapon, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponMesh;

    /** Animation */
    UPROPERTY(EditAnywhere, Category = WeaponAnimation)
    UAnimationAsset* FireAnim;

    /** Player and Character References */
    UPROPERTY()
    class AMainCharacter* MainCharacter; // Access to MainCharacter functions.

    UPROPERTY(Replicated)
    AMainCharacter* PlayerCharacter; // Replicated reference to the owning player character.


    /** Replicated States */
    UPROPERTY(Replicated)
    bool bAiming;

    UPROPERTY(ReplicatedUsing = OnRep_AmmoOnHand)
    int32 AmmoOnHand; // Current ammo available for the weapon.

    UPROPERTY(ReplicatedUsing = OnRep_AmmoOnHand)
    int32 MaxAmmoOnHand; // Maximum ammo capacity for the weapon.

    UPROPERTY(ReplicatedUsing = OnRep_AmmoOnHand)
    int32 ReloadAmount; // Ammo reloaded per action.

    UPROPERTY(ReplicatedUsing = OnRep_AmmoInMag)
    int32 AmmoInMag; // Ammo in the magazine.

	/** Ammo and Capacity */
    UPROPERTY()
    int32 MagCapacity; // Maximum magazine capacity.

    UPROPERTY(ReplicatedUsing = OnRep_CombatState)
    ECombatState CombatState = ECombatState::ECS_Unoccupied;

    UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
    EWeaponState WeaponState = EWeaponState::EWS_Unequipped;

    /** Crosshair Details */
    UPROPERTY(EditAnywhere, Category = Crosshairs)
    UTexture2D* CrosshairsCenter;

    UPROPERTY(EditAnywhere, Category = Crosshairs)
    UTexture2D* CrosshairsLeft;

    UPROPERTY(EditAnywhere, Category = Crosshairs)
    UTexture2D* CrosshairsRight;

    UPROPERTY(EditAnywhere, Category = Crosshairs)
    UTexture2D* CrosshairsTop;

    UPROPERTY(EditAnywhere, Category = Crosshairs)
    UTexture2D* CrosshairsBottom;

    /** Other Utility Variables */
    bool bReloading; // Indicates if the weapon is reloading.
    bool bFireButtonPressed; // True when the fire button is pressed.
    bool bCanFire = true; // True when the weapon can fire.

    FVector LocalHitTarget; // The local target for weapon aiming.

    /** HUD Management */
    class AMyHUD* HUD; // Cached HUD reference.
    FHUDPackage HUDPackage;

    float CrosshairVelocityFactor; // Affects crosshair spread based on player velocity.

    /** Casing and Effects */
    UPROPERTY(EditAnywhere, Category = Casing)
    TSubclassOf<class ACasing> Casing; // Ejectable shell casing class.


    UPROPERTY(Replicated, EditAnywhere, Category = "Weapon Type")
    EWeaponType WeaponType; // The type of weapon (e.g., Rifle, Pistol).

    /** Timer Handles */
    FTimerHandle ReloadTimerHandle;
};
