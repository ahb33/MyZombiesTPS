// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyZombiesGameMode.generated.h"

UCLASS(minimalapi)
class AMyZombiesGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyZombiesGameMode();

	void EndGame(bool bPlayerWon);

	void CheckEnemiesAlive(); // this function will try how many zombies are killed and end the game when none are left

protected:

    virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "EndGameWidgets")
	TSubclassOf< class UUserWidget> YouWonWidgetClass;

private:
	void InitializeGameplay();
	
};



