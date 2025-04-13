// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"

ALobbyPlayerState::ALobbyPlayerState()
{
    bReady = false;
    TeamID = 0;  // Default team ID
}

void ALobbyPlayerState::SetReadyStatus(bool bIsReady)
{
    if (HasAuthority())  // Ensure only the server updates this
    {
        bReady = bIsReady;
        OnRep_ReadyStatus();
    }
}

void ALobbyPlayerState::OnRep_ReadyStatus()
{
    UE_LOG(LogTemp, Warning, TEXT("%s is %s"), *GetPlayerName(), bReady ? TEXT("Ready") : TEXT("Not Ready"));
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALobbyPlayerState, bReady);
    DOREPLIFETIME(ALobbyPlayerState, TeamID);
}