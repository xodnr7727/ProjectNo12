// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapWidget.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTNO1_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void HideMessage();
	void Show();
	void Hide();

	void MapCaveOpen();
	void MapIceLandOpen();
	void MapForestOpen();

protected:
	void NativeConstruct();

	UFUNCTION()
	void OnIceLandClicked();

	UFUNCTION()
	void OnForestClicked();

	UFUNCTION()
	void OnCaveClicked();

	UFUNCTION()
	void PlayMapTransSound();

	UFUNCTION()
	void PlayUnknownRegionSound();

	UFUNCTION()
	void OnBackClicked(); //닫기

	UFUNCTION()
	void UnknownRegionMessage(); //확인되지 않은 지역 메세지

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* IceLand;

	UPROPERTY(meta = (BindWidget))
	class UButton* Forest;

	UPROPERTY(meta = (BindWidget))
	class UButton* Cave;

	UPROPERTY(meta = (BindWidget))
	class UButton* Back;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageText;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class  UAllMenuWidget> AllMenuWidgetClass;

	UPROPERTY()
	UAllMenuWidget* AllMenuWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "Map")
	USoundBase* MapTransSound;

	UPROPERTY(EditAnywhere, Category = "Map")
	USoundBase* UnknownRegionSound;

	/*지역 위치*/
	UPROPERTY(EditAnywhere, Category = "Map")
	FVector CaveLocation;

	UPROPERTY(EditAnywhere, Category = "Map")
	FVector IceLandLocation;

	UPROPERTY(EditAnywhere, Category = "Map")
	FVector ForestLocation;

	/*지도 확장 부울 변수*/
	UPROPERTY(EditAnywhere, Category = "Map")
	bool bCaveState;

	UPROPERTY(EditAnywhere, Category = "Map")
	bool bIceLandState;

	UPROPERTY(EditAnywhere, Category = "Map")
	bool bForestState;

};
