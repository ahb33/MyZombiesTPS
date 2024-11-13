// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAIController.h"
#include "CentralCommands.h"
#include "CentralCommanderAI.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API ACentralCommanderAI : public AAIController
{
    GENERATED_BODY()

public:
    // Constructor
    ACentralCommanderAI();

    // Implement a BroadcastCommand() function that loops through the tracked AMyAIController instances 
    // and sends a command or updates their behavior (e.g., move to a location, change state).
    // Ensure that BroadcastCommand() takes a command type or parameter, such as a string or enum, to define the instruction being sent.

    void BroadcastCommand(ECentralCommands Commands);

    void RegisterAIController(AMyAIController* AIController); // add AMyAIController instances

private:
    // List of pointers to MyAIController instances
    TArray<class AMyAIController*> ControlledAIControllers;

    UPROPERTY(EditAnywhere, Category = "Commands")    
    ECentralCommands CentralCommands;

};