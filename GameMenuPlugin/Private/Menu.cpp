// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Engine/World.h" 
#include "Components/Button.h" 
#include "MyGameInstanceSubsystem.h"
#include "GameFramework/PlayerController.h"


void UMenu::MenuSetup(int32 NumConnections, FString Match, FString MultiplayerPath, FString SoloPath)
{
    // Add widget to viewport, set properties visibility to true;
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    NumPublicConnections = NumConnections;
    MatchType = Match;

    PathToMultiplayerLevel = FString::Printf(TEXT("%s?listen"), *MultiplayerPath);
    PathToSoloLevel = FString::Printf(TEXT("%s?listen"), *SoloPath);

    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if(PlayerController)
        {
            FInputModeUIOnly InputModeData; 
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData); // setting input mode to UI only
            PlayerController->SetShowMouseCursor(true);
        }
    }

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		mySubsystem = GameInstance->GetSubsystem<UMyGameInstanceSubsystem>();
	}

}

bool UMenu::Initialize() 
{

    // return false if parent version returns false
    if(!Super::Initialize())
    {
        return false;
    }
    // bind callbacks to buttons
    if (SoloButton != nullptr)
    {
        SoloButton->OnClicked.AddDynamic(this, &UMenu::OnSoloButtonClicked);
    }

    if (Multiplayer != nullptr)
    {
        Multiplayer->OnClicked.AddDynamic(this, &UMenu::OnMultiplayerButtonClicked);
    }


    return true;

}

void UMenu::OnSoloButtonClicked()
{

    // Add debug code
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Solo Button Clicked")));
    }

    if(mySubsystem)
    {
        UWorld* World = GetWorld();
		if(World)
		{
            // travel to AI map if Solo button clicked
			World->ServerTravel(PathToSoloLevel);
		}
    }
}


void UMenu::OnMultiplayerButtonClicked()
{

    // Add debug code
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Multiplayer Button Clicked")));
    }

    if(mySubsystem)
    {
        UWorld* World = GetWorld();
		if(World)
		{
            // travel to AI map if Solo button clicked
			World->ServerTravel(PathToMultiplayerLevel);
		}
    }

}


void UMenu::GameStartMovement()
{
    /* Removes the widget from its parent widget. 
       If this widget was added to the player's screen or the viewport it will also be removed from those containers.
    */
    RemoveFromParent();

    UWorld* World = GetWorld();
    if (World)
    {
       APlayerController* PlayerController = World->GetFirstPlayerController();

       if(PlayerController)
       {
            FInputModeGameOnly GameInputMode;
            PlayerController->SetInputMode(GameInputMode);
            PlayerController->SetShowMouseCursor(false);
       }
    } 
}

void UMenu::NativeDestruct()
{
    GameStartMovement();
    Super::NativeDestruct();
}





