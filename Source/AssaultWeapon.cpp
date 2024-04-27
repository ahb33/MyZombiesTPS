// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultWeapon.h"
#include "Components/SceneComponent.h" 
#include "Engine/World.h" 
#include "EnemyCharacter.h"
#include "Engine/SkeletalMeshSocket.h" 

void AAssaultWeapon::Fire(const FVector& HitTarget)
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
		WeaponTraceHit(Start, HitTarget, FireHit);

        if(ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
            ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
        }
	}   
}

void AAssaultWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& FireHit)
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
                    HitEnemy->EnemyDamage(GetDamage());
                }
            }
            else
            {
                FireHit.ImpactPoint = End;
            }
        }
}
void AAssaultWeapon::SetAmmo(int32 NewAmmoOnHand, int32 NewAmmoInMag)
{
    AssaultWeaponAmmoOnHand = NewAmmoOnHand;
    AssaultWeaponAmmoInMag = NewAmmoInMag;

    // AWeapon::SetAmmoOnHand(NewAmmoOnHand);

}
int32 AAssaultWeapon::GetCurrentAmmoOnHand() const
{
    UE_LOG(LogTemp, Warning, TEXT("GetCurrentAmmoOnHand() called for AssaultWeapon"));
    return AssaultWeaponAmmoOnHand;
}

int32 AAssaultWeapon::GetCurrentAmmoInMag() const
{
    return AssaultWeaponAmmoInMag;
}

int32 AAssaultWeapon::GetMaxAmmoOnHand() const
{
    return AssaultWeaponMaxAmmoOnHand;
}

int32 AAssaultWeapon::GetMagCapacity() const
{
    return AssaultWeaponMagCapacity;
}

void AAssaultWeapon::ReloadAmmo(int32 AmmoAmount) 
{
    UE_LOG(LogTemp, Warning, TEXT("Assault Weapon reload function called"));
    // Update the AssaultWeapon-specific ammo counts
    AssaultWeaponAmmoOnHand += AmmoAmount;
    AssaultWeaponAmmoInMag -= AmmoAmount;

    // Ensure the values stay within valid bounds
    AssaultWeaponAmmoOnHand = FMath::Clamp(AssaultWeaponAmmoOnHand, 0, AssaultWeaponMaxAmmoOnHand);
    AssaultWeaponAmmoInMag = FMath::Max(AssaultWeaponAmmoInMag, 0);

}

float AAssaultWeapon::GetDamage() const
{
    return 40.f;
}

