// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "DamageIncreaseWidget.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTNO1_API UDamageIncreaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Show();
	void Hide();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowNotEnoughGoldMessage();

	void SetDamage(float WDamage);

	void SetAmor(float WAmor);

protected:

	void NativeConstruct();

	UFUNCTION()
	void OnDamageIncreaseClicked();

	UFUNCTION()
	void OnAmorIncreaseClicked();

	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void HideMessage();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* DamageIncrease;

	UPROPERTY(meta = (BindWidget))
	class UButton* AmorIncrease;

	UPROPERTY(meta = (BindWidget))
	class UButton* Back;

	class AProjectNo1Character* PlayerCharacter;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Damage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Amor;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class  UAllMenuWidget> AllMenuWidgetClass;

	UPROPERTY()
	UAllMenuWidget* AllMenuWidgetInstance;
};
