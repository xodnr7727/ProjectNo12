// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

void AMyPlayerController::OnPlayerDeath()
{
    // �÷��̾ �׾����Ƿ� �Է��� ��Ȱ��ȭ�մϴ�.
    bIsPlayerDead = true;
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    bShowMouseCursor = true; // ���콺�� Ȱ��ȭ�Ͽ� ���� ���� ��ư�� ���� �� �ֵ��� �մϴ�.
}

void AMyPlayerController::EndGameButtonPressed()
{
    // ������ �����ϴ� ������ ���⿡ �߰��մϴ�.
    // ���� ���, ���� ���� �Լ��� ȣ���ϰų� ���� �޴��� ���ư��� �ڵ带 �߰��� �� �ֽ��ϴ�.
}