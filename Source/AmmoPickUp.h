// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "WeaponTypes.h"
#include "AmmoPickUp.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API AAmmoPickUp : public APickUp
{
	GENERATED_BODY()




protected: 

	
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// you only want overlap events to happen on the server


private:

	class AMainCharacter* MainCharacter;

	class AWeapon* myWeapon;

	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;
};


/*
Create pointer to Weapon class
Create and construct variable that is pointer to MainCharacter and cast OtherActor from Sphere begin overlap
Check if MainCharacter is valid



*/