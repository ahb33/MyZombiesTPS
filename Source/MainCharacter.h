// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "TurnInPlace.h"
#include "CombatState.h"
#include "HealthPickUp.h"
#include "AmmoPickUp.h"
#include "MyPlayerController.h"
#include "MainCharacter.generated.h"

UCLASS()
class MYZOMBIES_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AMainCharacter();

	virtual void PostInitializeComponents() override; /*
	Allow actors to initialize themselves on the C++ side after all of their components have been initialized, 
	only called during gameplay */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetOverlappingWeapon(class AWeapon* Weapon);
	void SetOverlappingItem(APickUp* PickUp);	
	bool IsWeaponEquipped();
	bool IsAiming();

	bool IsReloading();

	float GetReloadDuration();

	void PlayFireMontage(bool bAiming);

	void PlayReloadMontage();
	
	class AWeapon* GetEquippedWeapon();

	UFUNCTION()
	void OnRep_OverlappingWeapon(class AWeapon* LastWeapon); // this function will be called whenever the variable's value is replicated

	FORCEINLINE float GetCharacterYaw() const { return AO_Yaw; }
	FORCEINLINE float GetCharacterPitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace; }

	void TurnInPlace(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetPlayerHealth() const { return PlayerHealth; }

	UFUNCTION()
	void OnRep_Health();

	FVector GetHitTarget() const;

    // call on client and execute on server
    UFUNCTION(Server, Reliable)
    void ServerEquipButtonPressed();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	void PickUpButtonPressed();
	void AimOffset(float DeltaTime);

	

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere)
	class AWeapon* myWeapon; // this pointer variable will have access to the Weapon class

	class AMyPlayerController* MyPlayerController;

	class AMyHUD* MyGameHUD;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon; // we have to override a function

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadMontage;

	ETurningInPlace TurningInPlace;

	FRotator StartingAimRotation;

	float AO_Yaw;
	float AO_Pitch;

	class APickUp* OverlappingItem; // pointer to APickUp class

	class AHealthPickUp* pickUpHealth; // pointer to AHealthPickUp class

	class AAmmoPickUp* pickUpAmmo; // pointer to AAmmoPickUp class

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float PlayerHealth = 50.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float MAXHealth = 100.f;

	float InterpAO_Yaw;


};		
