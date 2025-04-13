// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerMenuWidget.h"
#include "GameModeSelectionMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYZOMBIES_API UGameModeSelectionMenuWidget : public UMultiplayerMenuWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    virtual void MenuSetup();

    void BindButtonEvents();

protected:
    UFUNCTION()
    void OnSelectDeathmatchButtonClicked();

    UFUNCTION()
    void OnSelectZombiesButtonClicked();

    UFUNCTION()
    void OnBackToMultiplayerMenuButtonClicked();

private:
    void LoadLobby(FString SelectedGameMode);

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* DeathmatchButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ZombiesButton;


	
};
