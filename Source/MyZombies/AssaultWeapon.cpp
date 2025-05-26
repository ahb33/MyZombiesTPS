// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultWeapon.h"
#include "Components/SceneComponent.h" 
#include "Engine/World.h" 
#include "Engine/SkeletalMeshSocket.h" 
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "EnemyCharacter.h"



// void AAssaultWeapon::Fire(const FVector& HitTarget)
// {
//     // // Call the base class Fire method for shared behavior
//     // Super::Fire(HitTarget);

//     // // weaponmesh is not valid or unable to get owner return
//     // if (WeaponIsEmpty() || !GetWeaponMesh() || !GetOwner()) return;

//     // // obtain muzzleflash socket created on weapon and store it in pointer to SKM variable called MuzzleSocket
//     // const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

//     // // if MuzzleSocket not valid return
//     // if (!MuzzleSocket) return;

//     // // 
//     // FVector MuzzleLocation = MuzzleSocket->GetSocketTransform(GetWeaponMesh()).GetLocation();

//     // // Determine initial fire direction
//     // FVector FireDirection = (HitTarget - MuzzleLocation).GetSafeNormal();

//     // // Spawn projectile
//     // UWorld* World = GetWorld();
//     // if (World && Projectile)
//     // {
//     //     FActorSpawnParameters SpawnParams;
//     //     SpawnParams.Owner = GetOwner();
//     //     SpawnParams.Instigator = Cast<APawn>(GetOwner());

//     //     AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
//     //         Projectile, 
//     //         MuzzleLocation, 
//     //         FireDirection.Rotation(), 
//     //         SpawnParams
//     //     );

//     //     if (SpawnedProjectile)
//     //     {
//     //         // // Set the projectile's initial speed or other properties
//     //         // SpawnedProjectile->GetProjectileMovementComponent()->Velocity = FireDirection * SpawnedProjectile->GetProjectileSpeed();
//     //     }
//     // }

//     // // Spawn muzzle flash
//     // if (MuzzleFlash)
//     // {
//     //     UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleLocation);
//     // }

// }

void AAssaultWeapon::HandleFire(const FVector& HitTarget, const FVector& MuzzleLocation)
{
    Super::HandleFire(HitTarget, MuzzleLocation);
    
    FVector FireDirection = (HitTarget - MuzzleLocation).GetSafeNormal();

    if (Projectile)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = Cast<APawn>(GetOwner());

        GetWorld()->SpawnActor<AProjectile>(
            Projectile,
            MuzzleLocation,
            FireDirection.Rotation(),
            SpawnParams
        );
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


