// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMenuWidget.h"

// Fill out your copyright notice in the Description page of Project Settings.



void UBaseMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("Base Menu NativeConstruct() called"));

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
    UE_LOG(LogTemp, Warning, TEXT("CreateAndStoreWidget called"));
    
    if (WidgetClass && !menuWidgetMap.Contains(MenuName))
    {
        UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (NewWidget)
        {
            menuWidgetMap.Add(MenuName, NewWidget);
            UE_LOG(LogTemp, Log, TEXT("Created and stored widget: %s"), *MenuName.ToString());
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
        UE_LOG(LogTemp, Warning, TEXT("BaseMenu added to viewport"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseMenu is already in the viewport, skipping AddToViewport"));
    }
}

void UBaseMenuWidget::SetupInputMode()
{
    if (playerController)
    {
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        playerController->SetInputMode(InputModeData);
        playerController->bShowMouseCursor = true;
    }
}



