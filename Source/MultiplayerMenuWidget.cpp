// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerMenuWidget.h"
#include "Kismet/GameplayStatics.h"


void UMultiplayerMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    MenuSetup();
}

void UMultiplayerMenuWidget::MenuSetup()
{
    Super::MenuSetup();
    BindButtonEvents(); 

    // retreive gameinstance and store in ref
    // 
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        multiplayerSessionPtr = GameInstance->GetSubsystem<UMultiplayerSessions>();
    
    }

    if(multiplayerSessionPtr)
    {
        // For dynamic multicast delegates (AddDynamic and RemoveDynamic):
        multiplayerSessionPtr->MultiplayerOnCreateSessionComplete.RemoveDynamic(this, &ThisClass::OnCreateSession);
        multiplayerSessionPtr->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);

        // For non-dynamic multicast delegates (AddUObject and Clear):
        multiplayerSessionPtr->MultiplayerOnFindSessionComplete.Clear();  // Clear all bindings
        multiplayerSessionPtr->MultiplayerOnFindSessionComplete.AddUObject(this, &UMultiplayerMenuWidget::OnFindSessions);

        multiplayerSessionPtr->MultiplayerOnJoinSessionComplete.Clear();  // Clear all bindings
        multiplayerSessionPtr->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMultiplayerMenuWidget::OnJoinSession);

        // For dynamic multicast delegates (AddDynamic and RemoveDynamic):
        multiplayerSessionPtr->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UMultiplayerMenuWidget::OnDestroySession);
        multiplayerSessionPtr->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMultiplayerMenuWidget::OnDestroySession);

        multiplayerSessionPtr->MultiplayerOnStartSessionComplete.RemoveDynamic(this, &UMultiplayerMenuWidget::OnStartSession);
        multiplayerSessionPtr->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMultiplayerMenuWidget::OnStartSession);
    }
}


void UMultiplayerMenuWidget::BindButtonEvents()
{
    // Check if HostButton is valid and not already bound and then bind it to OnHostClicked
    if (HostButton && !HostButton->OnClicked.IsAlreadyBound(this, &UMultiplayerMenuWidget::OnHostButtonClicked))
    {
        HostButton->OnClicked.AddDynamic(this, &UMultiplayerMenuWidget::OnHostButtonClicked);
    }

    // Check if MultiplayerButton is valid and not already bound and then bind it to OnMultiplayerClicked
    if (JoinButton && !JoinButton->OnClicked.IsAlreadyBound(this, &UMultiplayerMenuWidget::OnJoinButtonClicked))
    {
        JoinButton->OnClicked.AddDynamic(this, &UMultiplayerMenuWidget::OnJoinButtonClicked);
    }

    // Check if BackButton is valid and not already bound and then bind it to OnMultiplayerClicked
    if(BackButton && !BackButton->OnClicked.IsAlreadyBound(this, &UMultiplayerMenuWidget::OnBackButtonClicked))
    {
        BackButton->OnClicked.AddDynamic(this, &UMultiplayerMenuWidget::OnBackButtonClicked);
    }
}

// shouldnt host create a session?
void UMultiplayerMenuWidget::OnHostButtonClicked()
{
    // Ensure widget creation before attempting transition
    FName GameModeSelectionMenu = "GameModeSelectionMenu";

    auto gameModeSelectionMenuWidgetRef = GetGameModeSelectionMenuWidgetClass();

    if (!menuWidgetMap.Contains(GameModeSelectionMenu))
    {
        Super::CreateAndStoreWidget(GameModeSelectionMenu, gameModeSelectionMenuWidgetRef);
    }

    TransitionToMenu(GameModeSelectionMenu);
}

void UMultiplayerMenuWidget::OnJoinButtonClicked()
{
    if (multiplayerSessionPtr)
    {
        // Clear previous bindings to avoid duplicate calls
        multiplayerSessionPtr->MultiplayerOnFindSessionComplete.Clear();
        multiplayerSessionPtr->MultiplayerOnFindSessionComplete.AddUObject(
            this,
            &UMultiplayerMenuWidget::OnFindSessions
        );

        // Initiate session search
        multiplayerSessionPtr->FindSessions(100);
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("MultiplayerSessionPtr is invalid!"));
        }
    }
}

void UMultiplayerMenuWidget::OnBackButtonClicked()
{
    // Ensure widget creation before attempting transition

    FName MainMenu = "MainMenu";

    auto mainMenuWidgetRef = GetMainMenuWidgetClass();

    if (!menuWidgetMap.Contains(MainMenu))
    {
        Super::CreateAndStoreWidget(MainMenu, mainMenuWidgetRef);
    }

    TransitionToMenu(MainMenu);
}

void UMultiplayerMenuWidget::OnCreateSession(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BWasSuccessful, OnCreateSession being called"));
        }
        UGameplayStatics::OpenLevel(this, FName("LobbyLevel"));  // Replace with actual level name
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BWasSuccessful failed"));
        }
    }
    // travel to lobby
}


void UMultiplayerMenuWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResults, bool bWasSuccessful)
{
    if (!bWasSuccessful || SessionResults.Num() == 0)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No sessions found or session search failed."));
        }
        return;
    }

    // Log the available sessions
    for (const auto& Result : SessionResults)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session found: %s"), *Result.Session.OwningUserName);
    }

    // Select the first session (you can add logic to filter sessions based on match type, etc.)
    const FOnlineSessionSearchResult& SelectedSession = SessionResults[0];

    if (multiplayerSessionPtr)
    {
        multiplayerSessionPtr->JoinSession(SelectedSession);
    }
}

void UMultiplayerMenuWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{

}

void UMultiplayerMenuWidget::OnDestroySession(bool bWasSuccessful)
{

}

void UMultiplayerMenuWidget::OnStartSession(bool bWasSuccessful)
{
    
}
