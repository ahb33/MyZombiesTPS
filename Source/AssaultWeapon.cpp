// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultWeapon.h"
#include "Components/SceneComponent.h" 
#include "Engine/World.h" 
#include "Engine/SkeletalMeshSocket.h" 
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "EnemyCharacter.h"

void AAssaultWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget); // Call the base class version of Fire

    APawn* InstigatorPawn = Cast<APawn>(GetOwner()); // Cast the owner to a pawn

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

    if (MuzzleFlashSocket && !WeaponIsEmpty())
    {
        // UE_LOG(LogTemp, Warning, TEXT("MuzzleFlashSocket is valid and weapon is not empty"));
        SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector MuzzleLocation = SocketTransform.GetLocation();

        FHitResult CameraHit;
        FVector AdjustedHitTarget = CrossHairTrace(CameraHit);

        FVector Direction = ((AdjustedHitTarget - MuzzleLocation) * 0.5f + (HitTarget - MuzzleLocation) * 0.5f).GetSafeNormal();        
        FVector End = MuzzleLocation + Direction * Trace_Length;

        FHitResult FireHit;
        WeaponTraceHit(MuzzleLocation, End, FireHit);

        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
        }

        UAISense_Hearing::ReportNoiseEvent(this, MuzzleLocation, 1.0f, InstigatorPawn, 2000.0f, TEXT("WeaponFire"));
        // UE_LOG(LogTemp, Warning, TEXT("Noise event reported at location %s"), *MuzzleLocation.ToString());
        
        DrawDebugLine(GetWorld(), MuzzleLocation, End, FColor::Red, false, 1.0f, 0, 1.0f);
        DrawDebugPoint(GetWorld(), FireHit.ImpactPoint, 10, FColor::Green, false, 1.0f);

        // UE_LOG(LogTemp, Warning, TEXT("Firing weapon at %s"), *AdjustedHitTarget.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to fire weapon: MuzzleFlashSocket invalid or weapon is empty"));
    }



}

void AAssaultWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& FireHit)
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

        if (FireHit.bBlockingHit)
        {
            AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(FireHit.GetActor());
            if (HitEnemy)
            {
                HitEnemy->EnemyDamage(GetDamage());
            }
        }

        DrawDebugPoint(GetWorld(), FireHit.ImpactPoint, 10, FColor::Green, false, 1.0f);
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

