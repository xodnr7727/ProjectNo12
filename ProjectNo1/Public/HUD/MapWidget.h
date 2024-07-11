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
	void OnBackClicked(); //�ݱ�

	UFUNCTION()
	void UnknownRegionMessage(); //Ȯ�ε��� ���� ���� �޼���

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

	/*���� ��ġ*/
	UPROPERTY(EditAnywhere, Category = "Map")
	FVector CaveLocation;

	UPROPERTY(EditAnywhere, Category = "Map")
	FVector IceLandLocation;

	UPROPERTY(EditAnywhere, Category = "Map")
	FVector ForestLocation;

	/*���� Ȯ�� �ο� ����*/
	UPROPERTY(EditAnywhere, Category = "Map")
	bool bCaveState;

	UPROPERTY(EditAnywhere, Category = "Map")
	bool bIceLandState;

	UPROPERTY(EditAnywhere, Category = "Map")
	bool bForestState;

};
