// Fill out your copyright notice in the Description page of Project Settings.


#include "CentralCommanderAI.h"


ACentralCommanderAI::ACentralCommanderAI()
{
    // Initialization logic if needed
}

void ACentralCommanderAI::BroadcastCommand(ECentralCommands Commands)
{
    // Implement a BroadcastCommand() function that loops through the tracked AMyAIController instances 
    // and sends a command or updates their behavior (e.g., move to a location, change state).

    // Communication Integration: Implement methods in CentralCommanderAI for broadcasting commands like “Move to X” or “Attack Player.”
    // Blackboard Key Updates: Modify existing blackboard keys in AMyAIController to respond to these commands.



}

// this function adds AMyAIController instances to ControlledAIControllers array when they are created or detected. 
void ACentralCommanderAI::RegisterAIController(AMyAIController* AIController)
{
    // if AI Controller is not valid return
    
    if (!AIController) return;

    if (ControlledAIControllers.Contains(AIController)) return;

    ControlledAIControllers.Add(AIController);
    UE_LOG(LogTemp, Log, TEXT("AIController %s registered successfully."), *AIController->GetName());

}


