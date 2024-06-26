// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyProGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UMyProGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UMyProGameInstance();

    void InitializeDefaultValues();

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerHealth;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerMaxHealth;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerStamina;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerMaxStamina;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerExperience;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerMaxExperience;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerHealthRegenRate;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerStaminaRegenRate;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    int32 PlayerGold;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    int32 PlayerLevel;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player Stats")
    bool bIsInitialized;
};
