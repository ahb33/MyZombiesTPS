// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h" 
#include "Sound/SoundCue.h"
#include "Projectile.generated.h"

UCLASS()
class MYZOMBIES_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();


    FORCEINLINE float GetProjectileSpeed() const { return ProjectileSpeed; } // Getter for projectile speed

    UPROPERTY(VisibleAnywhere, Category = Projectile)
    class USphereComponent* CollisionSphere;

    UPROPERTY(EditAnywhere, Category = Projectile)
    class UParticleSystem* Tracer;

    UPROPERTY(EditAnywhere, Category = Projectile)
    UParticleSystemComponent* TracerSystem;

    UPROPERTY(EditAnywhere)
    UParticleSystem* ImpactParticles;

    UPROPERTY(EditAnywhere)
    USoundCue* ImpactSound;

	/*We need to have some sort of hit event that will occur when projectile hits something
	we'd like to have function called in response to a hit event*/
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:	

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile|Movement")
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = "Projectile|Movement")
	int DamageAmount;



};
