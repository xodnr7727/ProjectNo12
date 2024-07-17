// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SystemWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "HUD/StartWidget.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/InputComponent.h"
#include <ProjectNo1/ProjectNo1GameMode.h>

void USystemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (HelpTextBlock)
    {
        HelpTextBlock->SetVisibility(ESlateVisibility::Hidden); // 처음엔 숨겨놓음
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &USystemWidget::OnQuitClicked);
    }
    if (HelpButton)
    {
        HelpButton->OnClicked.AddDynamic(this, &USystemWidget::OnHelpClicked);
    }
    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &USystemWidget::OnBackClicked);
    }
}

void USystemWidget::OnQuitClicked()
{
    AProjectNo1GameMode* GameMode = Cast<AProjectNo1GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode) {
        GameMode->OpenStartWidget();
    }
    this->Hide();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PC->SetPause(true);
        PC->bShowMouseCursor = true;
    }
}

void USystemWidget::OnHelpClicked()
{
    UnknownRegionMessage();
}

void USystemWidget::UnknownRegionMessage()
{
    FTimerHandle MessageEndTimerHandle;
    if (HelpTextBlock)
    {
        HelpTextBlock->SetVisibility(ESlateVisibility::Visible);
        // 10초 후 HideMessage 함수를 호출하여 메시지를 숨김
        GetWorld()->GetTimerManager().SetTimer(MessageEndTimerHandle, this, &USystemWidget::HideMessage, 10.0f, false);
    }
}

void USystemWidget::HideMessage()
{
    if (HelpTextBlock)
    {
        HelpTextBlock->SetVisibility(ESlateVisibility::Hidden);
    }
}

void USystemWidget::OnBackClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player) {
        Player->PlayerCanMove();
        this->Hide();
    }
    UE_LOG(LogTemp, Log, TEXT("MapWidgetHide"));
}

void USystemWidget::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void USystemWidget::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}