// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultWeapon.h"
#include "Components/SceneComponent.h" 
#include "Engine/World.h" 
#include "EnemyCharacter.h"
#include "Engine/SkeletalMeshSocket.h" 

void AAssaultWeapon::Fire(const FVector& HitTarget)
{

    Super::Fire(HitTarget) ; // now when we call this function on projectile the base class version of fire will be called

    //spawn projectile at tip of barrel - first get "owner" of class or weapon
    APawn* InstigatorPawn = Cast<APawn>(GetOwner()); // castin owner to pawn - character firing weapon is "owner" of projectile

    /*Spawn projectile at "MuzzleFlash" socket on weapon 
    create variable that is reference of type USkeletalMeshSocket
    Use GetWeaponMesh getter from weapon.cpp*/
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash")); 


    UE_LOG(LogTemp, Warning, TEXT("HitTarget: %s"), *HitTarget.ToString());
    if (MuzzleFlashSocket && !WeaponIsEmpty())
    {
        SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        
        if (MuzzleFlash)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
        }

        FVector TraceStart = SocketTransform.GetLocation();
        FVector TraceEnd = TraceStart + (HitTarget - TraceStart) * 1.25; // Define TraceRange as a float representing the max trace distance

        FHitResult FireHit;
        FCollisionQueryParams TraceParams(FName(TEXT("FireTrace")), true, InstigatorPawn);
        TraceParams.bTraceComplex = true;
        TraceParams.bReturnPhysicalMaterial = false;

        UWorld* World = GetWorld();
        if (World->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
        {
            AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(FireHit.GetActor());
            if (HitEnemy)
            {
                HitEnemy->EnemyDamage(GetDamage());
            }
        }

        if (Projectile)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = GetOwner();
            SpawnParams.Instigator = InstigatorPawn;
            
            World->SpawnActor<AProjectile>(Projectile, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SpawnParams);
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

