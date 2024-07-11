// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DamageIncreaseWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HUD/AllMenuWidget.h"

void UDamageIncreaseWidget::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UDamageIncreaseWidget::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UDamageIncreaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerCharacter = Cast<AProjectNo1Character>(PlayerController->GetPawn());
    }
    if (MessageText)
    {
        MessageText->SetText(FText::FromString(TEXT("Not Enough Gold")));
        MessageText->SetVisibility(ESlateVisibility::Hidden); // 처음엔 숨겨놓음
    }

    // 다시하기 버튼에 클릭 이벤트 바인딩
    if (DamageIncrease)
    {
        DamageIncrease->OnClicked.AddDynamic(this, &UDamageIncreaseWidget::OnDamageIncreaseClicked);
    }
    if (AmorIncrease)
    {
        AmorIncrease->OnClicked.AddDynamic(this, &UDamageIncreaseWidget::OnAmorIncreaseClicked);
    }
    if (Back)
    {
        Back->OnClicked.AddDynamic(this, &UDamageIncreaseWidget::OnBackClicked);
    }
}

void UDamageIncreaseWidget::OnDamageIncreaseClicked()
{
    PlayerCharacter->ICDamageGold();
}

void UDamageIncreaseWidget::OnAmorIncreaseClicked()
{
    PlayerCharacter->ICAmorGold();
}

void UDamageIncreaseWidget::OnBackClicked()
{
        PlayerCharacter->PlayerCanMove();
        this->Hide();
 
        UE_LOG(LogTemp, Log, TEXT("DamageWidgetHide"));
}

void UDamageIncreaseWidget::ShowNotEnoughGoldMessage()
{
    FTimerHandle MessageEndTimerHandle;
    if (MessageText)
    {
        MessageText->SetVisibility(ESlateVisibility::Visible);
        // 1초 후 HideMessage 함수를 호출하여 메시지를 숨김
        GetWorld()->GetTimerManager().SetTimer(MessageEndTimerHandle, this, &UDamageIncreaseWidget::HideMessage, 1.0f, false);
    }
}

void UDamageIncreaseWidget::HideMessage()
{
    if (MessageText)
    {
        MessageText->SetVisibility(ESlateVisibility::Hidden);
    }
}


void UDamageIncreaseWidget::SetDamage(float WDamage)
{
    if (WDamage)
    {
        const FString String = FString::Printf(TEXT("%'d"), WDamage);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WDamage, &FormatOptions);
        Damage->SetText(Text);
    }
}

void UDamageIncreaseWidget::SetAmor(float WAmor)
{
    if (WAmor)
    {
        const FString String = FString::Printf(TEXT("%'d"), WAmor);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WAmor, &FormatOptions);
        Amor->SetText(Text);
    }
}
