// Fill out your copyright notice in the Description page of Project Settings.


#include "SoloMenuWidget.h"
#include "Kismet/GameplayStatics.h"


void USoloMenuWidget::NativeConstruct()
{
    MenuSetup();
}

void USoloMenuWidget::MenuSetup()
{
    Super::MenuSetup();
    BindButtonEvents(); 
}

void USoloMenuWidget::BindButtonEvents()
{
    // Check if EasyButton is valid and not already bound and then bind it to OnEasyButtonClicked
    if (EasyButton && !EasyButton->OnClicked.IsAlreadyBound(this, &USoloMenuWidget::OnEasyButtonClicked))
    {
        EasyButton->OnClicked.AddDynamic(this, &USoloMenuWidget::OnEasyButtonClicked);
    }

    // Check if MediumButton is valid and not already bound and then bind it to OnMediumButtonClicked
    if (MediumButton && !MediumButton->OnClicked.IsAlreadyBound(this, &USoloMenuWidget::OnMediumButtonClicked))
    {
        MediumButton->OnClicked.AddDynamic(this, &USoloMenuWidget::OnMediumButtonClicked);
    }

    // Check if HardButton is valid and not already bound and then bind it to OnHardButtonClicked
    if(HardButton && !HardButton->OnClicked.IsAlreadyBound(this, &USoloMenuWidget::OnHardButtonClicked))
    {
        HardButton->OnClicked.AddDynamic(this, &USoloMenuWidget::OnHardButtonClicked);
    }

    // Check if HardButton is valid and not already bound and then bind it to OnBackButtonClicked
    if(BackButton && !BackButton->OnClicked.IsAlreadyBound(this, &USoloMenuWidget::OnBackButtonClicked))
    {
        BackButton->OnClicked.AddDynamic(this, &USoloMenuWidget::OnBackButtonClicked);
    }
}

void USoloMenuWidget::OnEasyButtonClicked()
{

    // Transition to the Solo Level with specific GameMode
    UGameplayStatics::OpenLevel(this, FName("SoloLevel"));  

}

void USoloMenuWidget::OnMediumButtonClicked()
{

}

void USoloMenuWidget::OnHardButtonClicked()
{

}

void USoloMenuWidget::OnBackButtonClicked()
{

    UE_LOG(LogTemp, Warning, TEXT("MainMenuClicked"));
    // Ensure widget creation before attempting transition

    FName MainMenu = "MainMenu";

    auto mainMenuWidgetRef = GetMainMenuWidgetClass();

    if (!menuWidgetMap.Contains(MainMenu))
    {
        UE_LOG(LogTemp, Log, TEXT("Creating MultiplayerMenu widget."));
        Super::CreateAndStoreWidget(MainMenu, mainMenuWidgetRef);
    }

    TransitionToMenu(MainMenu);

}
