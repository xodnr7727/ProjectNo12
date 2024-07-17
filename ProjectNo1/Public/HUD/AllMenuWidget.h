// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AllMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UAllMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void Show();

	UFUNCTION()
	void Hide();

	UFUNCTION()
	void CloseMenu();

protected:

	virtual void NativeConstruct() override;

	// 다시하기 버튼 클릭 이벤트 함수
	UFUNCTION()
	void OnInfoClicked();

	UFUNCTION()
	void OnMapClicked();

	UFUNCTION()
	void OnUpgradeClicked();

	UFUNCTION()
	void OnOptionClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* InfoButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* MapButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpgradeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OptionButton;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UDamageIncreaseWidget> DamageIncreaseWidgetClass;

	UPROPERTY()
	UDamageIncreaseWidget* DamageIncreaseWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UMapWidget> MapWidgetClass;

	UPROPERTY()
	UMapWidget* MapWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInfoWidget> InfoWidgetClass;

	UPROPERTY()
	UInfoWidget* InfoWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class USystemWidget> SystemWidgetClass;

	UPROPERTY()
	USystemWidget* SystemWidgetInstance;

	class AProjectNo1Character* PlayerCharacter;
};
