// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProGameInstance.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Kismet/GameplayStatics.h"

UMyProGameInstance::UMyProGameInstance()
    : bIsInitialized(false)
{
}

void UMyProGameInstance::InitializeDefaultValues()
{
    if (!bIsInitialized)
    {
        PlayerHealth = 200;
        PlayerMaxHealth = 200;
        PlayerStamina = 200;
        PlayerMaxStamina = 200;
        PlayerExperience = 120;
        PlayerMaxExperience = 500;
        PlayerHealthRegenRate = 1;
        PlayerStaminaRegenRate = 55;
        PlayerGold = 0;
        PlayerLevel = 1;
        PlayerDamage = 100;
        PlayerArmor = 1;

        bIsInitialized = true; // 초기화 완료
    }
}