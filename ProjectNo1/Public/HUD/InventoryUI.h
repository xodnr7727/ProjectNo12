// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InventoryUI.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTNO1_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
protected:

	void NativeConstruct();

	// 다시하기 버튼 클릭 이벤트 함수
    UFUNCTION()
    void OnRetryClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* RETRY;
};
