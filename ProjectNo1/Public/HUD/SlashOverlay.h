// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item.h"
#include "SlashOverlay.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTNO1_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetExperienceBarPercent(float Percent);

	void SetGold(int32 Gold);

	void SetLevel(int32 Level);
	//void SetSouls(int32 Souls);
		
private:
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* ExProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;

	//UPROPERTY(meta = (BindWidget))
	//	class UTextBlock* SoulsText;
};
