// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeSelectionMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"


void UGameModeSelectionMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    MenuSetup();

}

void UGameModeSelectionMenuWidget::MenuSetup()
{
    Super::MenuSetup();
    BindButtonEvents(); 


}

void UGameModeSelectionMenuWidget::BindButtonEvents()
{
    if (DeathmatchButton && !DeathmatchButton->OnClicked.IsAlreadyBound(this, &UGameModeSelectionMenuWidget::OnSelectDeathmatchButtonClicked))
    {
        DeathmatchButton->OnClicked.AddDynamic(this, &UGameModeSelectionMenuWidget::OnSelectDeathmatchButtonClicked);
    }

    if (ZombiesButton && !ZombiesButton->OnClicked.IsAlreadyBound(this, &UGameModeSelectionMenuWidget::OnSelectZombiesButtonClicked))
    {
        ZombiesButton->OnClicked.AddDynamic(this, &UGameModeSelectionMenuWidget::OnSelectZombiesButtonClicked);
    }

    if (BackButton && !BackButton->OnClicked.IsAlreadyBound(this, &UGameModeSelectionMenuWidget::OnBackToMultiplayerMenuButtonClicked))
    {
        BackButton->OnClicked.AddDynamic(this, &UGameModeSelectionMenuWidget::OnBackToMultiplayerMenuButtonClicked);
    }
}


void UGameModeSelectionMenuWidget::OnSelectDeathmatchButtonClicked()
{
    // UGameplayStatics::OpenLevel(this, FName("SpaceShipLevel_Solo")); // create level for this

    UE_LOG(LogTemp, Warning, TEXT("DeathMatch Button Clicked"));
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
    {
        GI->SetSelectedGameMode("Deathmatch");  // or "Zombies"
    }

    FName LobbyMenu = "LobbyMenu";

    auto LobbyMenuWidgetRef = GetLobbyMenuWidgetClass();

    if (!menuWidgetMap.Contains(LobbyMenu))
    {
        UE_LOG(LogTemp, Log, TEXT("Creating MultiplayerMenuWidget widget."));
        Super::CreateAndStoreWidget(LobbyMenu, LobbyMenuWidgetRef);
    }

    TransitionToMenu(LobbyMenu);   
}

void UGameModeSelectionMenuWidget::OnSelectZombiesButtonClicked()
{
    // UGameplayStatics::OpenLevel(this, FName("SpaceShipLevel_Solo")); // create level for this

    UE_LOG(LogTemp, Warning, TEXT("ZombiesButtonClicked"));
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
    {
        GI->SetSelectedGameMode("Zombies_Level");  // Use level name directly
    }
    
    FName LobbyMenu = "LobbyMenu";

    auto LobbyMenuWidgetRef = GetLobbyMenuWidgetClass();

    if (!menuWidgetMap.Contains(LobbyMenu))
    {
        UE_LOG(LogTemp, Log, TEXT("Creating MultiplayerMenuWidget widget."));
        Super::CreateAndStoreWidget(LobbyMenu, LobbyMenuWidgetRef);
    }

    TransitionToMenu(LobbyMenu);   
}

void UGameModeSelectionMenuWidget::OnBackToMultiplayerMenuButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("BackButtonClicked"));

    FName MultiplayerMenu = "MultiplayerMenu";

    auto multiplayerMenuWidgetRef = GetMultiplayerMenuWidgetClass();

    if (!menuWidgetMap.Contains(MultiplayerMenu))
    {
        UE_LOG(LogTemp, Log, TEXT("Creating MultiplayerMenuWidget widget."));
        Super::CreateAndStoreWidget(MultiplayerMenu, multiplayerMenuWidgetRef);
    }

    TransitionToMenu(MultiplayerMenu);
}

