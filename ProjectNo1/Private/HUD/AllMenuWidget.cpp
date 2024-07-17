// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AllMenuWidget.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "HUD/InfoWidget.h"
#include "HUD/MapWidget.h"
#include "HUD/DamageIncreaseWidget.h"
#include "HUD/SystemWidget.h"


void UAllMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerCharacter = Cast<AProjectNo1Character>(PlayerController->GetPawn());
    }

    if (InfoButton)
    {
        InfoButton->OnClicked.AddDynamic(this, &UAllMenuWidget::OnInfoClicked);
    }

    if (MapButton)
    {
        MapButton->OnClicked.AddDynamic(this, &UAllMenuWidget::OnMapClicked);
    }

    if (UpgradeButton)
    {
        UpgradeButton->OnClicked.AddDynamic(this, &UAllMenuWidget::OnUpgradeClicked);
    }

    if (OptionButton)
    {
        OptionButton->OnClicked.AddDynamic(this, &UAllMenuWidget::OnOptionClicked);
    }

}

void UAllMenuWidget::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UAllMenuWidget::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UAllMenuWidget::OnInfoClicked()
{
    PlayerCharacter->OpenInfoWidget();    
    this->Hide();
}

void UAllMenuWidget::OnMapClicked()
{
    PlayerCharacter->OpenMapWidget();
    this->Hide();
}

void UAllMenuWidget::OnUpgradeClicked()
{
    PlayerCharacter->OpenDamageIncreaseWidget();
    this->Hide();
}

void UAllMenuWidget::OnOptionClicked()
{
    PlayerCharacter->OpenSystemWidget();
    this->Hide();
}

void UAllMenuWidget::CloseMenu()
{
    // 전체 메뉴 닫기
    this->Hide();
}
