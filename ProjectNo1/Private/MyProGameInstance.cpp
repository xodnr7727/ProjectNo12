// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProGameInstance.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "MyProSaveGame.h"

void UMyProGameInstance::Init()
{
    Super::Init();

    LoadGame();
}

void UMyProGameInstance::SaveGame()
{
    UMyProSaveGame* SaveGameInstance = Cast<UMyProSaveGame>(UGameplayStatics::CreateSaveGameObject(UMyProSaveGame::StaticClass()));
    SaveGameInstance->PlayerMaxHealth = PlayerMaxHealth;
    SaveGameInstance->PlayerMaxStamina = PlayerMaxStamina;
    SaveGameInstance->PlayerExperience = PlayerExperience;
    SaveGameInstance->PlayerMaxExperience = PlayerMaxExperience;
    SaveGameInstance->PlayerHealthRegenRate = PlayerHealthRegenRate;
    SaveGameInstance->PlayerStaminaRegenRate = PlayerStaminaRegenRate;
    SaveGameInstance->PlayerLevel = PlayerLevel;
    SaveGameInstance->PlayerGold = PlayerGold;
    SaveGameInstance->PlayerDamage = PlayerDamage;
    SaveGameInstance->PlayerArmor = PlayerArmor;
    SaveGameInstance->SavedBlessingPoint = SavedBlessingPoint;
    SaveGameInstance->bCaveState = bCaveState;
    SaveGameInstance->bIceLandState = bIceLandState;
    SaveGameInstance->bForestState = bForestState;
    UE_LOG(LogTemp, Log, TEXT("SaveGame"));
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0);
}

void UMyProGameInstance::LoadGame()
{
     TArray<AActor*> PlayerStarts;
     UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
    {
        UMyProSaveGame* LoadGameInstance = Cast<UMyProSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
        PlayerMaxHealth = LoadGameInstance->PlayerMaxHealth;
        PlayerMaxStamina = LoadGameInstance->PlayerMaxStamina;
        PlayerExperience = LoadGameInstance->PlayerExperience;
        PlayerMaxExperience = LoadGameInstance->PlayerMaxExperience;
        PlayerHealthRegenRate = LoadGameInstance->PlayerHealthRegenRate;
        PlayerStaminaRegenRate = LoadGameInstance->PlayerStaminaRegenRate;
        PlayerLevel = LoadGameInstance->PlayerLevel;
        PlayerGold = LoadGameInstance->PlayerGold;		
        PlayerDamage = LoadGameInstance->PlayerDamage;
        PlayerArmor = LoadGameInstance->PlayerArmor;
        SavedBlessingPoint = LoadGameInstance->SavedBlessingPoint;
        bCaveState = LoadGameInstance->bCaveState;
        bIceLandState = LoadGameInstance->bIceLandState;
        bForestState = LoadGameInstance->bForestState;
        UE_LOG(LogTemp, Log, TEXT("LoadGame"));
    }
    else
    {
        // 기본 초기 값 설정
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
        if (PlayerStarts.Num() > 0)
        {
            AActor* PlayerStart = PlayerStarts[0];
            SavedBlessingPoint = PlayerStart->GetActorLocation();
        }
        UE_LOG(LogTemp, Log, TEXT("NotExistPlayerSaveSlot"));
    }
}
