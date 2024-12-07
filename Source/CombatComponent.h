// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyHUD.h"
#include "CombatState.h"       
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYZOMBIES_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend class AMainCharacter;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/* Firing Weapon   */
	void FireButtonPressed(bool bPressed);
	void Fire();

		/** Weapon Equipping */
	void EquipWeapon(class AWeapon*);
	void EquipSecondaryWeapon(class AWeapon*);
	void SwapWeapons();

	/** Utilities */
    void AttachActorToRightHand(AActor* ActorToAttach);
    void AttachWeaponToWeaponSocket(AActor* WeaponToAttach);
	class AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
    void SetAiming(bool bIsAiming);
    bool ShouldSwapWeapons() const;
    void SetCombatState(ECombatState State);
    ECombatState GetCombatState() const { return CombatState; }

    UFUNCTION()
    void OnRep_CombatState();

    void SetHUDCrosshairs(float DeltaTime);


protected:
    virtual void BeginPlay() override;

    UFUNCTION(Server, Reliable)
    void ServerSetAiming(bool bIsAiming);

    // Replication
    UFUNCTION()
    void OnRep_Aiming();

private:	

    // Weapon References
    UPROPERTY(Replicated)
    class AWeapon* EquippedWeapon;

    UPROPERTY(Replicated)
    class AWeapon* SecondaryWeapon;

    // Character and Controller References
    class AMainCharacter* MainCharacter;
    class AMyPlayerController* Controller;

    UPROPERTY()
    AMyHUD* HUD;

    // Combat HUD
    FHUDPackage HUDPackage;
    FVector HitTarget;
    float CrosshairVelocityFactor;

    // Crosshair Details
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

    UPROPERTY(ReplicatedUsing = OnRep_Aiming)
    bool bAiming;

    FVector LocalHitTarget;

    UPROPERTY(ReplicatedUsing = OnRep_CombatState)
    ECombatState CombatState;

    /** Other Utility Variables */
    bool bCanFire; // Indicates if the weapon is reloading.
    bool bFireButtonPressed;

};
