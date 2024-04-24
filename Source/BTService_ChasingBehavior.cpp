// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_ChasingBehavior.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyAIController.h"


UBTService_ChasingBehavior::UBTService_ChasingBehavior(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Set the update interval for the behavior
    bNotifyTick = true;
    Interval = 0.5f; // Adjust the interval as needed

    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = false;

    // Filter the blackboard key selectors
    PlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_ChasingBehavior, PlayerKey), AActor::StaticClass());



}



void UBTService_ChasingBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
     Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

     GetStaticDescription();

     UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
     if (BlackboardComp == nullptr) return;

     // Get the AI controller from OwnerComp and check whether it is valid
     AAIController* AIController = OwnerComp.GetAIOwner();
     if (AIController == nullptr) return;
         
     // cast our AI controller to our custom AI controller
     AMyAIController* ZombieAI = Cast<AMyAIController>(AIController);
     if (ZombieAI == nullptr) return;

     // Retrieve the player's location from the blackboard
     FVector PlayerLocation = BlackboardComp->GetValueAsVector("PlayerLocation");

     // Retrieve CanSeePlayer value from blckboard
     bool bCanSeePlayer = BlackboardComp->GetValueAsBool(CanSeePlayerKey.SelectedKeyName);

    // Retrieve CanHearPlayer value from blckboard
     bool bCanHearPlayer = BlackboardComp->GetValueAsBool(CanHearPlayerKey.SelectedKeyName);

     if (bCanSeePlayer || bCanHearPlayer)
     {
         // Update the player's last known position only if the AI currently sees the player.
         FVector CurrentPlayerLocation = BlackboardComp->GetValueAsVector(PlayerKey.SelectedKeyName);
         BlackboardComp->SetValueAsVector(LastKnownPositionKey.SelectedKeyName, CurrentPlayerLocation);
     }

     bLastCanSeePlayer = bCanSeePlayer;
    
}


// Called when auxiliary node becomes active; this function should be considered as const (doesn't modify state of object) if node is not instanced!
void UBTService_ChasingBehavior::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);
    // Get reference to the player so that we can store it on the blackboard

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        // Retrieve the player character using the specified class from the blackboard
        AActor* PlayerActor = nullptr;
        TArray<AActor*> Actors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, Actors);
        if (Actors.Num() > 0)
        {
            // Assuming there's only one player, otherwise you'll need additional logic to select the right one
            PlayerActor = Actors[0];
        }

        // Make sure we have successfully found the player
        if (PlayerActor != nullptr)
        {
            // Update the blackboard with the player's location
            BlackboardComp->SetValueAsVector(LastKnownPositionKey.SelectedKeyName, PlayerActor->GetActorLocation());

            // Update the blackboard with the player actor reference
            BlackboardComp->SetValueAsObject(PlayerKey.SelectedKeyName, PlayerActor);

        }
    }
}


FString UBTService_ChasingBehavior::GetStaticDescription() const
{
    // Use FString::Printf to format strings and build the description
    return FString::Printf(TEXT("%s: '%s'\n%s: '%s'\n%s: '%s'\n%s: '%s'"),
        TEXT("Player Class"), PlayerClass ? *PlayerClass->GetName() : TEXT("None"),
        TEXT("PlayerKey"), PlayerKey.IsSet() ? *PlayerKey.SelectedKeyName.ToString() : TEXT("None"),
        TEXT("LastKnownPositionKey"), LastKnownPositionKey.IsSet() ? *LastKnownPositionKey.SelectedKeyName.ToString() : TEXT("None"),
        TEXT("CanSeePlayerKey"), CanSeePlayerKey.IsSet() ? *CanSeePlayerKey.SelectedKeyName.ToString() : TEXT("None")
    );

}