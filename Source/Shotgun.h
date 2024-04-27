// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h" 
#include "Sound/SoundCue.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API AShotgun : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;	


	// modify WaponTrace function from Assault Weapon to include scatter 
	void WeaponTraceWithScatter(const FVector& TraceStart, const FVector& HitTarget, FHitResult& FireHit);


	virtual void SetAmmo(int32 NewAmmoOnHand, int32 NewAmmoInMag) override;

    virtual int32 GetCurrentAmmoOnHand() const override;

    virtual int32 GetCurrentAmmoInMag() const override;

	virtual int32 GetMaxAmmoOnHand() const override;

	virtual int32 GetMagCapacity() const override;

	virtual void ReloadAmmo(int32 Ammo) override;

	virtual float GetDamage() const override;


private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> Projectile; /*we can populate this variable with projectile or anything derived of  projectile*/

	class AMainCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles; // particles will be spawned if we get blocking hit

	UPROPERTY(EditAnywhere)
	int32 ShotgunAmmoOnHand; // 

	UPROPERTY(EditAnywhere)
	int32 ShotgunAmmoInMag; // 

	UPROPERTY(EditAnywhere)
	int32 ShotgunMaxAmmoOnHand; // 

	UPROPERTY(EditAnywhere)
	int32 ShotgunMagCapacity; // 

	FTransform SocketTransform;


};
