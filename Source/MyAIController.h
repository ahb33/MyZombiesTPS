// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API AMyAIController : public AAIController
{
	GENERATED_BODY()


public:

	/*You need to override the OnPossess function in your AMyAIController class 
	because it is called when the AI controller possesses a pawn, 
	which means it takes control of that pawn's actions.*/
	void OnPossess(APawn* InPawn) override;
	
	//Constructor
	AMyAIController();


	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);



private: 


	// Need to keep a reference to AIPerception component and reference to AISenseConfig for sight sense
	UAIPerceptionComponent* PerceptionComponent;


	UAISenseConfig_Sight* SightConfig;



	
};