// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Components/SceneComponent.h" 
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystem.h" 
#include "Kismet/GameplayStatics.h" 
#include "EnemyCharacter.h"
#include "Engine/World.h" 
#include "Engine/SkeletalMeshSocket.h" 

void AShotgun::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget); // Call the base class version of Fire

    // Cast the owner to a pawn - the character firing the weapon is the owner of the projectile
    APawn* InstigatorPawn = Cast<APawn>(GetOwner());

    // Spawn the projectile at the "MuzzleFlash" socket on the weapon
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash")); 

    UE_LOG(LogTemp, Warning, TEXT("HitTarget: %s"), *HitTarget.ToString());

    // Check if the socket is valid and the weapon is not empty
    if(MuzzleFlashSocket && !WeaponIsEmpty())
    {
        SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();
        FVector End = Start + (HitTarget - Start) * 1.25;

        
		FHitResult FireHit;
		WeaponTraceWithScatter(Start, HitTarget, FireHit);

        if(ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
            ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
        }
	}   
}

void AShotgun::WeaponTraceWithScatter(const FVector& TraceStart, const FVector& HitTarget, FHitResult& FireHit)
{
    UWorld* World = GetWorld();
     if(World)
        {
            FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

            World->LineTraceSingleByChannel(FireHit, 
            TraceStart, 
            End, 
            ECollisionChannel::ECC_Visibility);

            FVector BeamEnd = End;

            if(FireHit.bBlockingHit)
            {
                BeamEnd = FireHit.ImpactPoint;

                // First, check if the hit actor is an enemy
                AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(FireHit.GetActor());
                if (HitEnemy)
                {
                    // Apply damage to the enemy
                    HitEnemy->EnemyDamage(Damage); 
                    /*
                    applying damage, when using UGameplayStatics should be done 
                    server but fire function will be called on all machines 
                    */ 
                }
            }
            else
            {
                FireHit.ImpactPoint = End;
            }
        }
    
}

void AShotgun::SetAmmo(int32 NewAmmoOnHand, int32 NewAmmoInMag)
{
    ShotgunAmmoOnHand = NewAmmoOnHand;
    ShotgunAmmoInMag = NewAmmoInMag;

    // // Now also update the base class's ammo count
    // AWeapon::SetAmmoOnHand(NewAmmoOnHand);

}

int32 AShotgun::GetCurrentAmmoOnHand() const
{
    UE_LOG(LogTemp, Warning, TEXT("GetCurrentAmmoOnHand() called for shotgun"));
    return ShotgunAmmoOnHand;
}

int32 AShotgun::GetCurrentAmmoInMag() const
{
    return ShotgunAmmoInMag;
}

int32 AShotgun::GetMaxAmmoOnHand() const
{
    return ShotgunMaxAmmoOnHand;
}

int32 AShotgun::GetMagCapacity() const
{
    return ShotgunMagCapacity;
}

void AShotgun::ReloadAmmo(int32 AmmoAmount) 
{
// Update the Shotgun-specific ammo counts

    ShotgunAmmoOnHand += AmmoAmount;
    ShotgunAmmoInMag -= AmmoAmount;

    // Ensure the values stay within valid bounds
    ShotgunAmmoOnHand = FMath::Clamp(ShotgunAmmoOnHand, 0, ShotgunMaxAmmoOnHand);
    ShotgunAmmoInMag = FMath::Max(ShotgunAmmoInMag, 0);

}

void AShotgun::AddAmmoPickUp(int32 AmmoAdd)
{

}

float AShotgun::GetDamage() const
{
    return 45.f;
}
