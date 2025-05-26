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
    void InitValues();

    virtual void Tick(float DeltaTime) override;

    // Combat Functions
    void SetOverlappingWeapon(class AWeapon* Weapon);
    bool IsWeaponEquipped() const;
    AWeapon* GetEquippedWeapon();
    bool IsAiming() const;
    bool IsReloading() const;
    float GetReloadDuration() const;
    void PlayFireMontage(bool bAiming);
    void PlayReloadMontage();

	void SetOverlappingItem(APickUp* PickUp);

    // Replication Functions
    UFUNCTION()
    void OnRep_OverlappingWeapon();

    // Input Functions
    void EquipButtonPressed();
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

    // Getters for Animations
    FORCEINLINE float GetCharacterYaw() const { return AO_Yaw; }
    FORCEINLINE float GetCharacterPitch() const { return AO_Pitch; }
    FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

    // Health Functions
    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetPlayerHealth() const { return PlayerHealth; }

	FVector GetHitTarget() const;

    AMyPlayerController* GetMyPlayerController() const { return MyPlayerController; }

    ECombatState GetCharacterCombatState() const;


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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

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