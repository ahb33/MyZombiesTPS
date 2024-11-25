#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurnInPlace.h"
#include "CombatState.h"
#include "WeaponTypes.h"
#include "MainCharacter.generated.h"

UCLASS()
class MYZOMBIES_API AMainCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMainCharacter();

    virtual void PostInitializeComponents() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void Tick(float DeltaTime) override;

    // Combat Functions
    void SetOverlappingWeapon(class AWeapon* Weapon);
    bool IsWeaponEquipped() const;
    bool IsAiming() const;
    bool IsReloading() const;
    float GetReloadDuration() const;

    void PlayFireMontage(bool bAiming);
    void PlayReloadMontage();

	void SetOverlappingItem(APickUp* PickUp);


	class AWeapon* GetPrimaryWeapon() const {return PrimaryWeapon;}
	class AWeapon* GetSecondaryWeapon() const {return SecondaryWeapon;}

    // Replication Functions
    UFUNCTION()
    void OnRep_OverlappingWeapon();

    // Input Functions
    void EquipButtonPressed();
	void EquipWeapon(AWeapon* CurrentWeapon);
    void SwapWeapons();
    void CrouchButtonPressed();
    void AimButtonPressed();
    void AimButtonReleased();
    void FireButtonPressed();
    void FireButtonReleased();
    void ReloadButtonPressed();
    void PickUpButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	// Replication Functions
    UFUNCTION()
    void OnRep_Health();
	UFUNCTION()
	void OnRep_PrimaryWeapon();

    // Getters for Animations
    FORCEINLINE float GetCharacterYaw() const { return AO_Yaw; }
    FORCEINLINE float GetCharacterPitch() const { return AO_Pitch; }
    FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

    // Health Functions
    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetPlayerHealth() const { return PlayerHealth; }

	FVector GetHitTarget() const;


protected:
    virtual void BeginPlay() override;

    // Movement Functions
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    void AimOffset(float DeltaTime);
    void TurnInPlace(float DeltaTime);

private:
    // Components
    UPROPERTY(VisibleAnywhere, Category = Camera)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    class UCameraComponent* FollowCamera;

    UPROPERTY(Replicated)
    class AWeapon* PrimaryWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Weapons")
	class AWeapon* SecondaryWeapon; // The weapon equipped in the secondary slot.

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon; // The weapon the player is currently overlapping.

    UPROPERTY(EditAnywhere, Category = Combat)
    class UAnimMontage* FireMontage;

    UPROPERTY(EditAnywhere, Category = Combat)
    class UAnimMontage* ReloadMontage;

    // Aiming and Rotation
    FRotator StartingAimRotation;
    float AO_Yaw;
    float AO_Pitch;
    float InterpAO_Yaw;
    ETurningInPlace TurningInPlace;

    // Health
    UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
    float PlayerHealth;

    UPROPERTY(VisibleAnywhere, Category = "Player Stats")
    float MaxHealth;

	class APickUp* OverlappingItem;
    class AHealthPickUp* pickUpHealth;

    // Controllers and HUD
    class AMyPlayerController* MyPlayerController;
    class AMyHUD* MyGameHUD;

};