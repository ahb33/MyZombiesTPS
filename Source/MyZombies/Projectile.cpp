// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/SphereComponent.h"


// Sets default values
AProjectile::AProjectile()
{
    // Disable Tick as it's not needed
    PrimaryActorTick.bCanEverTick = false;

    // Create and configure the collision sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    SetRootComponent(CollisionSphere);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionObjectType(ECC_GameTraceChannel1);
    CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);


    // Bind the OnHit function
    CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    // Create and configure the projectile movement component
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f; // No gravity effect

    // Set default values
    ProjectileSpeed = 3000.0f;
    ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
    ProjectileMovementComponent->MaxSpeed = ProjectileSpeed;

    DamageAmount = 10.0f; // Example default damage
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Spawn tracer effect if specified
    if (Tracer)
    {
        TracerSystem = UGameplayStatics::SpawnEmitterAttached(Tracer, CollisionSphere);
    }
}

// Function called upon collision
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // Ensure the projectile doesn't hit itself or its owner
    if (OtherActor && OtherActor != GetOwner())
    {
        // Apply damage to the hit actor
        UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetInstigatorController(), this, UDamageType::StaticClass());

        // Spawn impact particles at the hit location
        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
        }

        // Play impact sound at the hit location
        if (ImpactSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
        }

        // Destroy the projectile after impact
        Destroy();
    }
}


