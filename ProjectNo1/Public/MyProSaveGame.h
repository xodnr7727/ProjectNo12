// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MyProSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UMyProSaveGame : public USaveGame
{
	GENERATED_BODY()
 
public:

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

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerDamage;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    float PlayerArmor;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    FVector  SavedBlessingPoint;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    bool bCaveState;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    bool bIceLandState;

    UPROPERTY(EditAnywhere, Category = "PlayerState")
    bool bForestState;
};
