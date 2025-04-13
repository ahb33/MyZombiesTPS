// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMenuWidget.h"

// Fill out your copyright notice in the Description page of Project Settings.



void UBaseMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    playerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

    if (!playerController)
    {
        UE_LOG(LogTemp, Error, TEXT("UBaseMenuWidget::NativeConstruct: Failed to get player controller!"));
    }
}



void UBaseMenuWidget::TransitionToMenu(FName MenuName)
{
    // Check if the widget is already created and stored
    if (UUserWidget** FoundWidget = menuWidgetMap.Find(MenuName))
    {
        if (*FoundWidget)
        {
            // Remove the current widget and display the new one
            RemoveFromParent();  // Ensure removal from the viewport
            (*FoundWidget)->AddToViewport();  // Add the found widget to the viewport
            SetupInputMode();  // Set input mode for the new widget
            return;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Widget for %s not found in map!"), *MenuName.ToString());
}



void UBaseMenuWidget::CreateAndStoreWidget(FName MenuName, TSubclassOf<UUserWidget> WidgetClass)
{
    
    if (WidgetClass && !menuWidgetMap.Contains(MenuName))
    {
        UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (NewWidget)
        {
            menuWidgetMap.Add(MenuName, NewWidget);
        }
    }
}

void UBaseMenuWidget::MenuSetup()
{
    // Check if the widget is already in the viewport
    if (!IsInViewport())
    {
        AddToViewport();
        SetupInputMode();  // Ensure input mode is set
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseMenu is already in the viewport, skipping AddToViewport"));
    }
}

void UBaseMenuWidget::SetupInputMode()
{
    if (!playerController)
    {
        UE_LOG(LogTemp, Error, TEXT("SetupInputMode: playerController is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Player Controller valid - SetUpInputMode called"));
    FInputModeUIOnly InputModeData;
    InputModeData.SetWidgetToFocus(nullptr);
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    playerController->SetInputMode(InputModeData);
    playerController->bShowMouseCursor = true;
    UE_LOG(LogTemp, Warning, TEXT("SetupInputMode called - Mouse cursor visible."));
}


