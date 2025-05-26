// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessions.h"
#include "OnlineSubsystem.h"

UMultiplayerSessions::UMultiplayerSessions() : CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
    // get online subsystem and store in variable
    // check if subsystem variable is valid
    // get session interface from subsystem variable and store in another variable

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("UMultiplayerSessions Constructor Called"));
    }

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        FString SubsystemName = Subsystem->GetSubsystemName().ToString();
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Using Online Subsystem: %s"), *SubsystemName));
        }
    }
    else
    {
      if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Online Subsystem is not valid: Get() failed to obtain it"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Ensure that the OnlineSubsystemSteam is enabled in your project settings."));
        }
    }

    InitSubsystem();
}

void UMultiplayerSessions::InitSubsystem()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to initialize Online Subsystem. Make sure Steam is running and the Steam subsystem is enabled."));
        }
        return;
    }

    FString SubsystemName = Subsystem->GetSubsystemName().ToString();
    if (SubsystemName != "Steam")
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Incorrect Online Subsystem. Expected: Steam, Found: %s"), *SubsystemName));
        }
        return;
    }

    sessionInterface = Subsystem->GetSessionInterface();
    if (!sessionInterface.IsValid())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to get Session Interface from Online Subsystem."));
        }
        return;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Online Subsystem successfully initialized."));
    }
}
/*
    The function first checks if a session already exists and destroys it if needed to ensure we start with a clean slate.
    sessionSettings are configured based on variables OnlinesessionSettings.h
    then we call sessionInterface's create session function
    finally, CreateSessionCompleteDelegate is bound to handle the session creation result, which will trigger the OnCreateSessionComplete callback.



*/
void UMultiplayerSessions::CreateSession(int32 NumPublicConnections, FString MatchType)
{
    UE_LOG(LogTemp, Warning, TEXT("Create Session Called"));
    
    if (!sessionInterface.IsValid()) return;

    // Check if a session already exists and destroy it if necessary
    auto ExistingSession = sessionInterface->GetNamedSession(NAME_GameSession);
    if (ExistingSession != nullptr)
    {
        sessionInterface->DestroySession(NAME_GameSession);
    }

    // Initialize session settings
    sessionSettings = MakeShareable(new FOnlineSessionSettings());
    sessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL"; // Check if using LAN
    sessionSettings->NumPublicConnections = NumPublicConnections; // Number of public connections
    sessionSettings->bShouldAdvertise = true; // Make session visible to others
    sessionSettings->bUsesPresence = true; // Enable presence for joining friends
    sessionSettings->bAllowJoinInProgress = true; // Allow players to join in progress
    sessionSettings->bAllowJoinViaPresence = true; // Allow join via presence (like Steam, etc.)
    sessionSettings->bIsDedicated = false; // Not dedicated server
    sessionSettings->bAllowJoinViaPresenceFriendsOnly = false; // Allow anyone to join
    sessionSettings->bUseLobbiesIfAvailable = true;

    // Define custom key for matching timeout
    const FName MATCHING_TIMEOUT_KEY = TEXT("MatchingTimeout");

    // Set custom match timeout setting
    sessionSettings->Set(MATCHING_TIMEOUT_KEY, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);

    // Set match type in destination projects - 
    const FName MATCH_TYPE_KEY = TEXT("MatchType");
    sessionSettings->Set(MATCH_TYPE_KEY, MatchType, EOnlineDataAdvertisementType::ViaOnlineService);
    // Create the session with the session settings
    auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer) return;

    if (!sessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionSettings))
    {
        sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        UE_LOG(LogTemp, Warning, TEXT("Failed to create session!"));
        MultiplayerOnCreateSessionComplete.Broadcast(false);
        return;
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Session Creation Successful"));
        }

    }
}

void UMultiplayerSessions::FindSessions(int32 MaxSearchResults)
{
   UE_LOG(LogTemp, Warning, TEXT("FindSessions called with MaxSearchResults = %d"), MaxSearchResults);
    MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);  // Empty result for testing
}

void UMultiplayerSessions::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
    UE_LOG(LogTemp, Warning, TEXT("JoinSession called"));

    // if session is not valid broadcast error and return

    if (!sessionInterface.IsValid()) 
    {
        MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::Type::UnknownError);  // Mock result for testing
        return;
    }

    JoinSessionCompleteDelegateHandle = sessionInterface->AddOnFindSessionsComplete
}

void UMultiplayerSessions::DestroySession()
{
    UE_LOG(LogTemp, Warning, TEXT("DestroySession called"));
    MultiplayerOnDestroySessionComplete.Broadcast(false);  // Mock result for testing
}

void UMultiplayerSessions::StartSession()
{
    UE_LOG(LogTemp, Warning, TEXT("StartSession called"));
    MultiplayerOnStartSessionComplete.Broadcast(false);  // Mock result for testing
}

void UMultiplayerSessions::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete called"));

    if(sessionInterface)
    {
        sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

    }

    MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);

}

void UMultiplayerSessions::OnFindSessionComplete(bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete called with bWasSuccessful = %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));
}

void UMultiplayerSessions::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete called with SessionName = %s, Result = %d"), *SessionName.ToString(), (int32)Result);
}

void UMultiplayerSessions::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete called with SessionName = %s, bWasSuccessful = %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));
}

void UMultiplayerSessions::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnStartSessionComplete called with SessionName = %s, bWasSuccessful = %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));
}
