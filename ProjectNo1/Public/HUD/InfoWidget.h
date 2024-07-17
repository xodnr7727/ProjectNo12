// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void Show();
	void Hide();
	void SetHealth(float WHealth);
	void SetStamina(float WStamina);
	void SetHealthRegenRate(float WHealthRegenRate);
	void SetStaminaRegenRate(float WStaminaRegenRate);
	void SetExperience(float WExperience);
	void SetInfoDamage(float WDamage);
	void SetInfoArmor(float WArmor);

protected:	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void OnRefreshClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoHealth;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoStamina;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoHealthRegenRate;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoStaminaRegenRate;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoExperience;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoDamage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoArmor;

	UPROPERTY(meta = (BindWidget))
	class UButton* Back;

	UPROPERTY(meta = (BindWidget))
	class UButton* Refresh;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class  UAllMenuWidget> AllMenuWidgetClass;

	UPROPERTY()
	UAllMenuWidget* AllMenuWidgetInstance;

};
