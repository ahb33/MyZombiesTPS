// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyHUD.h"

AMyPlayerController::AMyPlayerController()
{


}


void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();


}

void AMyPlayerController::SetHUDHealth(float CurrentHealth, float MaxHealth)
{

    MyPlayerHUD = Cast<AMyHUD>(GetHUD());


    if (MyPlayerHUD == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("My Player HUD is null"));
        return;


    }
    else
    {   
        if(MyPlayerHUD->CharacterStats)
        {
            // UE_LOG(LogTemp, Warning, TEXT("CharacterStats is valid within MyPlayer Controller"));
        }

        if (MyPlayerHUD->CharacterStats && MyPlayerHUD->CharacterStats->HealthBar)
        {
            const float HealthPercent = CurrentHealth / MaxHealth;
            MyPlayerHUD->CharacterStats->HealthBar->SetPercent(HealthPercent);
            // UE_LOG(LogTemp, Warning, TEXT("HealthBar is valid within MyPlayerController FINALLY"));

        }
    }
}

void AMyPlayerController::SetHUDAmmo(int32 Ammo)
{

    if (MyPlayerHUD == nullptr)
    {
        MyPlayerHUD = Cast<AMyHUD>(GetHUD());
        UE_LOG(LogTemp, Warning, TEXT("MyPlayerHUD initialized: %s"), MyPlayerHUD ? TEXT("Valid") : TEXT("Null"));
    }

    if (MyPlayerHUD)
    {
        if (MyPlayerHUD->CharacterStats)
        {
            if (MyPlayerHUD->CharacterStats->AmmoOnDisplay)
            {
                FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
                MyPlayerHUD->CharacterStats->AmmoOnDisplay->SetText(FText::FromString(AmmoText));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AmmoOnDisplay is NULL."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("CharacterStats is NULL."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MyPlayerHUD is NULL."));
    }
}

void AMyPlayerController::SetHUDMagAmmo(int32 AmmoInMag)
{
    // Check if HUD is valid
    if(MyPlayerHUD == nullptr)
    {
        MyPlayerHUD = Cast<AMyHUD>(GetHUD());
    }

    // Check if CharacterStats and AmmoOnDisplay are valid
    if(MyPlayerHUD && MyPlayerHUD->CharacterStats && MyPlayerHUD->CharacterStats->AmmoInMag)
    {
        FString AmmoMagText = FString::Printf(TEXT("%d"), AmmoInMag);
        MyPlayerHUD->CharacterStats->AmmoInMag->SetText(FText::FromString(AmmoMagText));
    }

}