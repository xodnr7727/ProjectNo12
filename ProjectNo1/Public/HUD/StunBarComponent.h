// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "StunBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UStunBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetStunPercent(float Percent);
private:
	UPROPERTY()
	class UStunBar* StunBarWidget;
};
