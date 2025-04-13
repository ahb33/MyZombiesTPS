// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetSelectedGameMode(FString GameMode) { SelectedGameMode = GameMode; }

    UFUNCTION(BlueprintCallable)
    FString GetSelectedGameMode() const { return SelectedGameMode; }

private:
    FString SelectedGameMode;
	
};
