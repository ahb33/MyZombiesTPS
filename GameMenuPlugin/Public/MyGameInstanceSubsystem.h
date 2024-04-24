// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMENUPLUGIN_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UMyGameInstanceSubsystem();

	void CreateSession(int32 NumPublicConnections, FString MatchType); 
	void FindSession(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();


protected:

	/*this takes 2 params : 
	* @param SessionName the name of the session this callback is for
 	* @param bWasSuccessful true if the async action completed without error, false if there was an error */

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/* this takes one param : 
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/

	void OnFindSessionComplete(bool bWasSuccessful);


	/* this takes 2 params : 
	*
	*  @param SessionName the name of the session this callback is for
 	*  @param bWasSuccessful true if the async action completed without error, false if there was an error

		/** Possible results of a JoinSession attempt 
		/** The join worked as expected 
		Success,
		/** There are no open slots to join 
		SessionIsFull,
		/** The session couldn't be found on the service 
		SessionDoesNotExist,
		/** There was an error getting the session server's address 
		CouldNotRetrieveAddress,
		/** The user attempting to join is already a member of the session 
		AlreadyInSession,
		/** An error not covered above occurred 
		UnknownError
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	/*
		this takes 2 params :  
		* @param SessionName the name of the session the that has transitioned to started
 		* @param bWasSuccessful true if the async action completed without error, false if there was an error
	
	*/
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful );


	/*
		this takes 2 params :  
		* @param SessionName the name of the session the that has transitioned to started
 		* @param bWasSuccessful true if the async action completed without error, false if there was an error
	
	*/
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful );

private:

	// We will need access to OnlineSessionInterace
	IOnlineSessionPtr SessionInterface; /*Interface definition for the online services session services 
	Session services are defined as anything related managing a session 
	and its state within a platform service*/

	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate; // Delegate fired when a session create request has completed 
	FDelegateHandle CreateSessionCompleteDelegateHandle;


	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate; // Delegate fired when the search for an online session has completed
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;



};
