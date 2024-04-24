// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AI_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API UAI_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:

	virtual void NativeInitializeAnimation() override; // this function is called once at the beginning of the game

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void NativeUpdateAnimation(float DeltaTime) override;


	// Setter function to set the random dying animation variable
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsDead(bool bDead);


	// Setter function to set the random dying animation variable
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetRandomDyingAnimation(bool bShouldRandomize);


	// Setter function to set the random dying animation variable
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetPlayerVisibility(bool bVisible);



private: 

	class APawn* Pawn; // This variable is a reference to the pawn currently using this AnimInstance

	class AEnemyCharacter* EnemyCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float MovementSpeed; // we only want C++ to alter this variable

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating; // this variable will check if the character is accelerating


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bPlayerVisible; // this variable will check if the zombie can see the player

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsDead; // this variable will check if the zombie can see the player


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bRandomDyingAnimation; // this variable will select between 2 of the dying animations


};
