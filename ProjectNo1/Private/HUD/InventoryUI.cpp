// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventoryUI.h"
#include "Kismet/GameplayStatics.h"

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
    // ������ �ٽ� ����
    UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
}
