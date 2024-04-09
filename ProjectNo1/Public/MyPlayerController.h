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
    // �÷��̾ �׾����� ���θ� ��Ÿ���� ����
    bool bIsPlayerDead = false;

public:
    // �÷��̾ ������ ȣ��Ǵ� �Լ�
    virtual void OnPlayerDeath();

    // ���� ���� ��ư�� ���� �� ȣ��Ǵ� �Լ�
    void EndGameButtonPressed();
};
