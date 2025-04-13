// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuWidget.h"
#include "MultiplayerSessions.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerMenuWidget.generated.h"

/**
 * 
 */



UCLASS()
class MYZOMBIES_API UMultiplayerMenuWidget : public UMainMenuWidget
{
	GENERATED_BODY()

public:


    virtual void NativeConstruct() override;

    virtual void MenuSetup();
  
    void BindButtonEvents();

    UFUNCTION()
    void OnHostButtonClicked();

    UFUNCTION()
    void OnJoinButtonClicked();

    UFUNCTION() 
    void OnBackButtonClicked();

    UFUNCTION()
    void OnCreateSession(bool bWasSuccessful);

    void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

protected:

    
    UPROPERTY(meta = (BindWidget, OptionalWidget = true))
    class UButton* BackButton; 

	
private:

    // create pointer to OnlinePlugin class to access necessary funtion
    UPROPERTY(meta = (BindWidget, OptionalWidget = true))
    class UButton* HostButton;

    UPROPERTY(meta = (BindWidget, OptionalWidget = true))
    class UButton* JoinButton;

    class UMultiplayerSessions* multiplayerSessionPtr; // pointer to custom mutliplayer plugin class

};
