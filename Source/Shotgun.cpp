// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Components/SceneComponent.h" 
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystem.h" 
#include "Kismet/GameplayStatics.h" 
#include "EnemyCharacter.h"
#include "Engine/World.h" 
#include "Net/UnrealNetwork.h"

#include "Engine/SkeletalMeshSocket.h" 


void AShotgun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AShotgun, ShotgunAmmoOnHand, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(AShotgun, ShotgunAmmoInMag, COND_OwnerOnly);
    DOREPLIFETIME(AShotgun, ShotgunMaxAmmoOnHand);
    DOREPLIFETIME(AShotgun, ShotgunMagCapacity);
}

// void AShotgun::HandleFire(const FVector& HitTarget, const FVector& MuzzleLocation)
// {
//     Super::HandleFire(HitTarget, MuzzleLocation);

//     const int32 NumPellets = 10;

//     for (int32 i = 0; i < NumPellets; ++i)
//     {
//         FHitResult FireHit;
//         FVector EndPoint = CalculateScatterEndPoint(MuzzleLocation, HitTarget);
//         WeaponTraceWithScatter(MuzzleLocation, EndPoint, FireHit);

//         if (FireHit.bBlockingHit)
//         {
//             DealDamage(FireHit);
//         }
//     }
// }

void AShotgun::Fire(const FVector &Hit)
{
    
}

void AShotgun::WeaponTraceWithScatter(const FVector &TraceStart, const FVector &EndPoint, FHitResult &FireHit)
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->LineTraceSingleByChannel(
            FireHit,
            TraceStart,
            EndPoint,
            ECollisionChannel::ECC_Visibility
        );
    }
}

FVector AShotgun::CalculateScatterEndPoint(const FVector& MuzzleLocation, const FVector& HitTarget)
{
    float ScatterAngle = 5.0f;

    FVector ShotDirection = (HitTarget - MuzzleLocation).GetSafeNormal();
    return FVector();
    // return MuzzleLocation + FMath::VRandCone(ShotDirection, FMath::DegreesToRadians(ScatterAngle)) * Trace_Length;
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


float AShotgun::GetDamage() const
{
    return 45.f;
}

void AShotgun::OnRep_ShotgunAmmoOnHand()
{
    RefreshHUD();
}

void AShotgun::OnRep_ShotgunAmmoInMag()
{
    RefreshHUD();
}

