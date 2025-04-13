// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenuWidget.generated.h"

/**
 Lobby Menu Structure (Simple & Effective)

✅ Displays:

    Player List (shows connected players).
    Game Mode (DeathMatch - fixed for now).
    Start Game Button (only for Host).

✅ Logic:

    Click "Host" → Create session → Open LobbyLevel.
    Players join → List updates dynamically.
    Host clicks "Start Game" → Open DeathmatchLevel.

💡 Structure:

    ULobbyMenuWidget (Widget UI logic). done
    ALobbyGameMode (Handles player state updates). done
    ALobbyPlayerState (Tracks each player). done
 */
UCLASS()
class MYZOMBIES_API ULobbyMenuWidget : public UUserWidget
{
	GENERATED_BODY()



	
};
