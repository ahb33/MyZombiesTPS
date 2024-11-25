
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerController.h"
#include "MultiplayerMenuWidget.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // Only create the widget instance if it's not already in the array

    UE_LOG(LogTemp, Warning, TEXT("MainMenu native construct called"));

    MenuSetup();
}


void UMainMenuWidget::MenuSetup()
{
    Super::MenuSetup();
    
    BindButtonEvents();
}


// Ensure that buttons are valid and not already bound to prevent multiple bindings.
void UMainMenuWidget::BindButtonEvents()
{
    if (SoloButton && !SoloButton->OnClicked.IsAlreadyBound(this, &UMainMenuWidget::OnSoloClicked))
    {
        SoloButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSoloClicked);
    }

    if (MultiplayerButton && !MultiplayerButton->OnClicked.IsAlreadyBound(this, &UMainMenuWidget::OnMultiplayerClicked))
    {
        MultiplayerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMultiplayerClicked);
    }
}

void UMainMenuWidget::OnSoloClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("SoloButton Clicked"));
    // Ensure widget creation before attempting transition
    FName SoloMenu = "SoloMenu";

    auto soloMenuWidgetRef = GetSoloMenuWidgetClass();

    if (!menuWidgetMap.Contains(SoloMenu))
    {
        UE_LOG(LogTemp, Log, TEXT("Creating SoloMenu widget."));
        Super::CreateAndStoreWidget(SoloMenu, soloMenuWidgetRef);
    }

    TransitionToMenu(SoloMenu);
}

void UMainMenuWidget::OnMultiplayerClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("MultiplayerClicked"));
    // Ensure widget creation before attempting transition
    FName MultiplayerMenu = "MultiplayerMenu";

    auto multiplayerMenuWidgetRef = GetMultiplayerMenuWidgetClass();

    if (!menuWidgetMap.Contains(MultiplayerMenu))
    {
        UE_LOG(LogTemp, Log, TEXT("Creating MultiplayerMenu widget."));
        Super::CreateAndStoreWidget(MultiplayerMenu, multiplayerMenuWidgetRef);
    }

    TransitionToMenu(MultiplayerMenu);
}



