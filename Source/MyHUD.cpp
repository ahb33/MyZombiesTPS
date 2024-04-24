// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"



void AMyHUD::BeginPlay()
{
	Super::BeginPlay();


	UE_LOG(LogTemp, Warning, TEXT("MyHUD begin play called"));

}

void AMyHUD::AddCharacterStats()
{
	/*Get Player Controller
	Check if player controller is valid and if CharacterStats Class is set
	Create widget and store in Character Stats*/

	APlayerController* PlayerController = GetOwningPlayerController();


	if (PlayerController && CharacterStatsClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller and CharacterStats are valid"));
		CharacterStats = CreateWidget<UCharacterStats>(PlayerController, CharacterStatsClass);
		// CharacterStats->SetHealthBar(HealthBar);
		CharacterStats->AddToViewport();


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller XX and CharacterStats are not valid"));

	}
}
// Need to get viewport size in order to draw crosshairs on screen
void AMyHUD::DrawHUD()
{
	Super::DrawHUD();

	/*Check if GEngine is valid and if GEngine->GameViewport is valid
	FVector2D ViewportSize - 
	Get Viewport size (ViewportSize) - GEngine->GameViewport has function to get the ViewportSize and it takes a 2D FVector
	Set CrosshairLocation to be x,y coordinates of ViewportSize
	Deproject coordinates to world space
	If projection successful
	*/
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread); 
		}
		if (HUDPackage.CrosshairsLeft)
		{

			FVector2D Spread(-SpreadScaled, 0.f); // only wanna move it towards left
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f); // only wanna move it towards right
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled); // moving up in UV means moving in -y
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled); // only wanna move it towards left
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread);
		}
	}
}

void AMyHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) +Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}


