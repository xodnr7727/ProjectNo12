// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GameRetryUI.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"

void UGameRetryUI::NativeConstruct()
{
    Super::NativeConstruct();

    // �ٽ��ϱ� ��ư�� Ŭ�� �̺�Ʈ ���ε�
    if (RETRY)
    {
        RETRY->OnClicked.AddDynamic(this, &UGameRetryUI::OnRetryClicked);
    }
}

void UGameRetryUI::OnRetryClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    Player->Respawn();
    this->Hide();
}

void UGameRetryUI::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UGameRetryUI::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}
