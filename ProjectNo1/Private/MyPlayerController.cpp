// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

void AMyPlayerController::OnPlayerDeath()
{
    // 플레이어가 죽었으므로 입력을 비활성화합니다.
    bIsPlayerDead = true;
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    bShowMouseCursor = true; // 마우스를 활성화하여 게임 종료 버튼을 누를 수 있도록 합니다.
}

void AMyPlayerController::EndGameButtonPressed()
{
    // 게임을 종료하는 로직을 여기에 추가합니다.
    // 예를 들어, 게임 종료 함수를 호출하거나 메인 메뉴로 돌아가는 코드를 추가할 수 있습니다.
}