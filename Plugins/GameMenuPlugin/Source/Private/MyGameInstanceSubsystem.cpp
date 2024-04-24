// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h" 
#include "OnlineSessionSettings.h" 
#include "OnlineSubsystem.h" 

UMyGameInstanceSubsystem::UMyGameInstanceSubsystem() : 
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{
    // Create and define pointer of type IOnlineSubsystem and access "Get" function of that class type
	// Check if pointer is valid
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(); // "Get" returns the online subsystem for a given service

    if(OnlineSubsystem)
    {
        /*Get the interface for accessing the session management services
        Returns:
        Interface pointer for the appropriate session service*/
        SessionInterface = OnlineSubsystem->GetSessionInterface(); // 
    }
}

void UMyGameInstanceSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
    if (!SessionInterface.IsValid())
    {
        // Session interface is not valid, return or handle the error
        return;
    }
    
        
    // Bind the completion delegate
    CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMyGameInstanceSubsystem::OnCreateSessionComplete);

    // Store completion delegate variable in delegate handle to allow remove of the delegate from the delegate list 
    CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);


    SessionSettings = MakeShareable(new FOnlineSessionSettings());
    SessionSettings->bIsLANMatch = false;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->NumPublicConnections = NumPublicConnections;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bUseLobbiesIfAvailable = true;



    /* Create session in IOnlineSessionPtr takaes 3 parameters
    */
    
    bool bSessionCreated = SessionInterface->CreateSession(NumPublicConnections, NAME_GameSession, *SessionSettings);
    
    // if session not created successfully clear delegate from list
    if(!bSessionCreated)
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
    }

}

void UMyGameInstanceSubsystem::FindSession(int32 MaxSearchResults)
{

}

void UMyGameInstanceSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{

}


void UMyGameInstanceSubsystem::StartSession()
{

    /*	virtual bool StartSession(FName SessionName) = 0; */

}

void UMyGameInstanceSubsystem::DestroySession()
{

}

void UMyGameInstanceSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // Session creation was successful
        UE_LOG(LogTemp, Log, TEXT("Session created successfully"));

        // If bWasSuccessful is valid travel to the map for online
        // GetWorld()->ServerTravel();
    }
    else
    {
        // Session creation failed
        UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
    }

    // Unbind the completion delegate
    SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

}

void UMyGameInstanceSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{



}

void UMyGameInstanceSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{

    if(bWasSuccessful)
    {

    }


}

void UMyGameInstanceSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{



}

void UMyGameInstanceSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{



}