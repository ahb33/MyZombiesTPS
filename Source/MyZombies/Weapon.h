// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h" 
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponState.h"
#include "MyHUD.h"
#include "Delegates/DelegateCombinations.h"
#include "Casing.h"
#include "Weapon.generated.h"


/*
    worry about equipping properly then firing
    overlapping weapon should set it in SetOverlappingWeapon
*/



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



    void DropWeapon();

    /** HUD and Crosshair */
    void SetHUDCrosshairs(float DeltaTime);
    void RefreshHUD();
    void SetHUDAmmo(int32 Ammo);
    void SetHUDMagAmmo(int32 MagAmmo);

    
    // reload 
    /** Weapon Firing */
    virtual void Fire(const FVector& Hit); // Virtual for child class overrides.
    void RoundFired();
    void SpawnMuzzleEffect(const FVector& MuzzleLocation);
    FVector GetMuzzleLocation() const;


    /** Weapon Reloading */
    void Reload();
    void FinishReloading();
    int32 AmmoToReload();
	virtual void ReloadAmmo(int32 Ammo) {};


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
    void DealDamage(const FHitResult& HitResult);

    /** Utilities */
    bool WeaponIsEmpty() const;
    bool ShouldSwapWeapons() const;
	// Useful for accessing the actual mesh of the weapon itself
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	void SetAiming(bool bIsAiming);
	void InitializeCharacterAndController();
    void SetWeaponState(EWeaponState NewState);
    EWeaponState GetWeaponState() const { return WeaponState; }




    /** Replication */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnRep_Owner();
    UFUNCTION()
    void OnRep_AmmoOnHand();
    UFUNCTION()
    void OnRep_AmmoInMag();
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

    virtual void HandleFire(const FVector& HitTarget, const FVector& MuzzleLocation);

    /** Overlap Events */
    UFUNCTION()
    virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
    bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AProjectile> Projectile; /*we can populate this variable with projectile or anything derived of  projectile*/

    UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;


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



    UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
    EWeaponState WeaponState = EWeaponState::EWS_Unequipped;

    /** Other Utility Variables */
    bool bReloading; // Indicates if the weapon is reloading.
    bool bFireButtonPressed;
    bool bCanFire = true; // True when the weapon can fire.

    /** Casing and Effects */
    UPROPERTY(EditAnywhere, Category = Casing)
    TSubclassOf<class ACasing> Casing; // Ejectable shell casing class.

    /** Timer Handles */
    FTimerHandle ReloadTimerHandle;

    UPROPERTY(EditAnywhere)
	float Damage;


	
};
