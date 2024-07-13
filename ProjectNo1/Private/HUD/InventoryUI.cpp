// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventoryUI.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"

void UInventoryUI::NativeConstruct()
{
    Super::NativeConstruct();

    // �ٽ��ϱ� ��ư�� Ŭ�� �̺�Ʈ ���ε�
    if (RETRY)
    {
        RETRY->OnClicked.AddDynamic(this, &UInventoryUI::OnRetryClicked);
    }
}

void UInventoryUI::OnRetryClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    Player->Respawn();
    this->Hide();
}

void UInventoryUI::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryUI::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}
