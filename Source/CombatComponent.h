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



protected:
    virtual void BeginPlay() override;

    // Server Functions
    UFUNCTION(Server, Reliable)
    void ServerEquipWeapon(AWeapon* WeaponToEquip);

    UFUNCTION(Server, Reliable)
    void ServerSwapWeapons();

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

    // UPROPERTY(ReplicatedUsing = OnRep_CombatState)
    // ECombatState CombatState;

    // Fire Control
    bool bFireButtonPressed;
};
