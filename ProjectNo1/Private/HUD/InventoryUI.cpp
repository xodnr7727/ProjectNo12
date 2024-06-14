// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventoryUI.h"
#include "Kismet/GameplayStatics.h"

void UInventoryUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 다시하기 버튼에 클릭 이벤트 바인딩
    if (RETRY)
    {
        RETRY->OnClicked.AddDynamic(this, &UInventoryUI::OnRetryClicked);
    }
}

void UInventoryUI::OnRetryClicked()
{
    // 게임을 다시 시작
    UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
}
