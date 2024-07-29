// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "BTService_ChasingBehavior.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"





AMyAIController::AMyAIController()
{
    // Create AI Perception component as well as Sight configuration
    MyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("MyPerceptionComponent"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    // Configuring the Sight Sense 
    SightConfig->SightRadius = 600;
    SightConfig->LoseSightRadius = 700;
    HearingConfig->HearingRange = 400;

    // Check all the flags for DetectionByAffiliation so that we detect our Player
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // Assign configured senses to the AI Perception Component
    MyPerceptionComponent->ConfigureSense(*SightConfig);
    MyPerceptionComponent->ConfigureSense(*HearingConfig);

    
    MyPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    // Binding the OnTargetPerceptionUpdate function
    MyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetPerceptionUpdate);


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
    // Retrieve all currently perceived Actors - OnTargetPerceptionUpdate requires array of actors 
    // that it can fill
    
    // Retrieve all currently perceived Actors for sight
    TArray<AActor*> PerceivedActorsSight;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActorsSight);

    // Retrieve all currently perceived Actors for hearing
    TArray<AActor*> PerceivedActorsHearing;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), PerceivedActorsHearing);


    // Combine unique actors from both lists
    TArray<AActor*> AllPerceivedActors = PerceivedActorsSight;
    for (AActor* HeardActor : PerceivedActorsHearing)
    {
        AllPerceivedActors.AddUnique(HeardActor);
    }

    // Cast the Actor to AMainCharacter
    MainCharacter = Cast<AMainCharacter>(Actor);
    if (MainCharacter && AllPerceivedActors.Contains(MainCharacter))
    {
        LastKnownPlayerPosition = Stimulus.StimulusLocation;
        bool currentlySeeingPlayer = Stimulus.WasSuccessfullySensed();

        // Ensure we have a blackboard component
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (BlackboardComp && currentlySeeingPlayer)
        {
            UE_LOG(LogTemp, Warning, TEXT("Blackboard Comp is valid"));
            // Update the blackboard with the current perception status
            BlackboardComp->SetValueAsBool("CanSeePlayer", currentlySeeingPlayer);
            UE_LOG(LogTemp, Warning, TEXT("Set CanSeePlayer in Blackboard to %s"), 
            currentlySeeingPlayer ? TEXT("true") : TEXT("false"));
        }

    }

    // bool isEntered = AllPerceivedActors.Contains(MainCharacter);
    // int NumberObjectSeen = AllPerceivedActors.Num();

    // FString text = FString(Actor->GetName() + "has just " + (isEntered ? "Entered" : "Left" ) 
    // + " the field of view. Now "  + FString::FromInt(NumberObjectSeen) + " objects are visible. ");
    
    // if(GEngine)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, text);
    // }
    // UE_LOG(LogTemp, Warning, TEXT("%s"), *text);
    
}