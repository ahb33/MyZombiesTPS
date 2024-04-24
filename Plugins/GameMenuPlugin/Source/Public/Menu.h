// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMENUPLUGIN_API UMenu : public UUserWidget
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumConnections = 6, 
	FString MatchType = FString(TEXT("DeathMatch")), 
	FString MultiplayerPath = FString(TEXT("/Game/GameAssets/Maps/MultiplayerMap")),

	FString SoloPath = FString(TEXT("/Game/GameAssets/Maps/SoloMap")));

	UFUNCTION()
	void OnSoloButtonClicked();

	UFUNCTION()
	void OnMultiplayerButtonClicked();  

	virtual bool Initialize() override;

	void GameStartMovement();

	virtual void NativeDestruct() override;


protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* SoloButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* Multiplayer;

private:


	class UMyGameInstanceSubsystem* mySubsystem;

	int NumPublicConnections{6};
	FString MatchType;

	FString PathToMultiplayerLevel{TEXT("")};

	FString PathToSoloLevel{TEXT("")};


};
