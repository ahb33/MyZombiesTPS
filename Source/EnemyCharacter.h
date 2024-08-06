// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI_AnimInstance.h"
#include "EnemyCharacter.generated.h"

UCLASS(Abstract)
class MYZOMBIES_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	// Properties and functions that all enemies should have
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTree;

    virtual void Attack() PURE_VIRTUAL(AEnemyCharacter::Attack,);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void EnemyDamage(float Damage);

	void Die();

	UFUNCTION()
	void OnRep_Health();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:	

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_Health)
    float Health;

	UAI_AnimInstance* AnimInstanceRef;

	FTimerHandle DestructionTimer; // Timer handle for delayed destruction

	class AMyZombiesGameMode* myGameMode;

};
