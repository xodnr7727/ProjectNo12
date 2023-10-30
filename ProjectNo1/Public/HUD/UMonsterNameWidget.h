// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMonsterNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UUMonsterNameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetMonsterName(const FText& Name);

private:
	UPROPERTY()
	class UTextBlock* MonsterName;
};
