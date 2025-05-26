// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"
/**
 * 
 */

class AMyHUD;

UCLASS()
class MYZOMBIES_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

    AMyPlayerController();

    UPROPERTY(EditAnywhere, Category = "HUD")
    class AMyHUD* MyPlayerHUD;

    void SetHUDHealth(float CurrentHealth, float MaxHealth);
    
    void SetHUDAmmo(int32 Ammo);

    void SetHUDMagAmmo(int32 AmmoInMag);

protected:
    
    virtual void BeginPlay() override;

private:


	
};
