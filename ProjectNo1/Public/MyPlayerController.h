// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    // 플레이어가 죽었는지 여부를 나타내는 변수
    bool bIsPlayerDead = false;

public:
    // 플레이어가 죽으면 호출되는 함수
    virtual void OnPlayerDeath();

    // 게임 종료 버튼을 누를 때 호출되는 함수
    void EndGameButtonPressed();
};
