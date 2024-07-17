// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SystemWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API USystemWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void Show();

	UFUNCTION()
	void Hide();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnHelpClicked();

	UFUNCTION()
	void UnknownRegionMessage();

	UFUNCTION()
	void HideMessage();

	UFUNCTION()
	void OnBackClicked();

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HelpButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HelpTextBlock;

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class  UStartWidget> StartWidgetClass;

	UPROPERTY()
	UStartWidget* StartWidgetInstance;
};
