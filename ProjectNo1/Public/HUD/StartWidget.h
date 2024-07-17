// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UStartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Show();
	void Hide();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnStartClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
};
