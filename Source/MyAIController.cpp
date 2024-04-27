// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "EnemyCharacter.h"





AMyAIController::AMyAIController()
{
    // Create AI Perception component as well as Sight configuration
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("SightPerceptionComponent"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    // Configuring the Sight Sense 
    SightConfig->SightRadius = 600;
    SightConfig->LoseSightRadius = 700;

    // Check all the flags for DetectionByAffiliation so that we detect our Player
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // Assign Sight Sense to AI Perception Component
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    // Binding the OnTargetPerceptionUpdate function
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetPerceptionUpdate);


}
/*
Call parent version of onpossess
Create and initialize reference to main character and cast pawn to maincharacter 
Check if pointer is not null
Create and initialize behavior tree reference
Check if behavior tree reference is not null
Run behavior tree
*/

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(InPawn);
    
    if(EnemyCharacter != nullptr) 
    {
        UBehaviorTree* BehaviorTree = EnemyCharacter->BehaviorTree;
        if(BehaviorTree != nullptr)
        RunBehaviorTree(BehaviorTree);
    }

}

void AMyAIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
    // // Retrieve all currently perceived Actors
    // TArray<AActor*> PerceivedActors;
    // PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), PerceivedActors);

    // // Calculate number of Perceived Actors and if current target left or entered FOV
    // bool isEntered = PerceivedActors.Contains(Actor);
    // int NumberObjectSeen = PerceivedActors.Num();
    // // page 205 for rest
    

    // FString text = FString(Actor->GetName() + "has just " + (isEntered ? "Entered" : "Left" ) 
    // + " the field of view. Now "  + FString::FromInt(NumberObjectSeen) + " objects are visible. ");
    
    // if(GEngine)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, text);
    // }
    // UE_LOG(LogTemp, Warning, TEXT("%s"), *text);
    
}