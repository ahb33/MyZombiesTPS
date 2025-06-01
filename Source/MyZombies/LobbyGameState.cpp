// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"
#include "LobbyPlayerState.h"
#include "MyGameInstance.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameState::CheckAllPlayersReady()
{
    for (APlayerState* PS : PlayerArray)
    {
        ALobbyPlayerState* LPS = Cast<ALobbyPlayerState>(PS);
        if (!LPS || !LPS->IsReady())
        {
            return; // At least one player is not ready
        }
    }

    // All players are ready - transition to selected game mode map
    UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>();
    if (!GI) return;

    const FName SelectedMode = GI->GetSelectedGameMode();
    if (SelectedMode.IsNone()) return;

    FString MapPath;
    if (SelectedMode == "Zombies")
    {
        MapPath = "/Game/GameAssets/Levels/ZombiesLevel?listen";
    }
    else if (SelectedMode == "Solo")
    {
        MapPath = "/Game/GameAssets/Levels/SoloLevel?listen";
    }
    else
    {
        MapPath = "/Game/GameAssets/Levels/ActualGameLevel?listen"; // Fallback
    }

    GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
    GetWorld()->ServerTravel(MapPath);
}