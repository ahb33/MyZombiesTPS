// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultWeapon.h"
#include "Components/SceneComponent.h" 
#include "Engine/World.h" 
#include "EnemyCharacter.h"
#include "Engine/SkeletalMeshSocket.h" 

void AAssaultWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget); // Call the base class version of Fire

    APawn* InstigatorPawn = Cast<APawn>(GetOwner()); // Cast the owner to a pawn

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash")); 

    UE_LOG(LogTemp, Warning, TEXT("HitTarget: %s"), *HitTarget.ToString());

   
    if (MuzzleFlashSocket && !WeaponIsEmpty())
    {
        SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector MuzzleLocation = SocketTransform.GetLocation();

        FVector CameraLocation;
        FRotator CameraRotation;
        InstigatorPawn->GetActorEyesViewPoint(CameraLocation, CameraRotation);

        FVector CameraDirection = CameraRotation.Vector();
        FVector CameraEnd = CameraLocation + CameraDirection * Trace_Length;

        FHitResult CameraHit;
        GetWorld()->LineTraceSingleByChannel(CameraHit, CameraLocation, CameraEnd, ECollisionChannel::ECC_Visibility);

        FVector AdjustedHitTarget = CameraHit.bBlockingHit ? CameraHit.ImpactPoint : CameraEnd;
        
        FVector MuzzleToAdjustedTarget = (AdjustedHitTarget - MuzzleLocation).GetSafeNormal();
        FVector End = MuzzleLocation + MuzzleToAdjustedTarget * Trace_Length;

        FHitResult FireHit;
        WeaponTraceHit(MuzzleLocation, End, FireHit);

        DrawDebugLine(GetWorld(), MuzzleLocation, End, FColor::Red, false, 1.0f, 0, 1.0f);
        DrawDebugPoint(GetWorld(), FireHit.ImpactPoint, 10, FColor::Green, false, 1.0f);

        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
        }
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

void AAssaultWeapon::AddAmmoPickUp(int32 AmmoAdd)
{

}


float AAssaultWeapon::GetDamage() const
{
    return 40.f;
}

