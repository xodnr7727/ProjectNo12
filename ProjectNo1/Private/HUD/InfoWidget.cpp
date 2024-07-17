// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InfoWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/AttributeComponent.h"
#include "HUD/AllMenuWidget.h"
#include "HUD/DamageIncreaseWidget.h"

void UInfoWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Back)
    {
        Back->OnClicked.AddDynamic(this, &UInfoWidget::OnBackClicked);
    }
    if (Refresh)
    {
        Refresh->OnClicked.AddDynamic(this, &UInfoWidget::OnRefreshClicked);
    }
    if (!InfoHealth)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoHealth is not bound!"));
    }
    if (!InfoStamina)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoStamina is not bound!"));
    }
    if (!InfoHealthRegenRate)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoHealthRegenRate is not bound!"));
    }
    if (!InfoStaminaRegenRate)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoStaminaRegenRate is not bound!"));
    }
    if (!InfoExperience)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoExperience is not bound!"));
    }
    if (!InfoDamage)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoDamage is not bound!"));
    }
    if (!InfoArmor)
    {
        UE_LOG(LogTemp, Warning, TEXT("InfoArmor is not bound!"));
    }
}

void UInfoWidget::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UInfoWidget::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UInfoWidget::SetHealth(float WHealth)
{
    if (WHealth)
    {
        const FString String = FString::Printf(TEXT("%'d"), WHealth);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WHealth, &FormatOptions);
        InfoHealth->SetText(Text);
    }
}

void UInfoWidget::SetStamina(float WStamina)
{
    if (WStamina)
    {
        const FString String = FString::Printf(TEXT("%'d"), WStamina);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WStamina, &FormatOptions);
        InfoStamina->SetText(Text);
    }
}

void UInfoWidget::SetHealthRegenRate(float WHealthRegenRate)
{
    if(WHealthRegenRate)
    {
        const FString String = FString::Printf(TEXT("%'d"), WHealthRegenRate);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WHealthRegenRate, &FormatOptions);
        InfoHealthRegenRate->SetText(Text);
    }
}

void UInfoWidget::SetStaminaRegenRate(float WStaminaRegenRate)
{
    if(WStaminaRegenRate)
    {
        const FString String = FString::Printf(TEXT("%'d"), WStaminaRegenRate);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WStaminaRegenRate, &FormatOptions);
        InfoStaminaRegenRate->SetText(Text);
    }
}

void UInfoWidget::SetExperience(float WExperience)
{
    if(WExperience)
    {
        const FString String = FString::Printf(TEXT("%'d"), WExperience);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WExperience, &FormatOptions);
        InfoExperience->SetText(Text);
    }
}

void UInfoWidget::SetInfoDamage(float WDamage)
{
    if (WDamage)
    {
        const FString String = FString::Printf(TEXT("%'d"), WDamage);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WDamage, &FormatOptions);
        InfoDamage->SetText(Text);
    }
}

void UInfoWidget::SetInfoArmor(float WArmor)
{
    if (WArmor)
    {
        const FString String = FString::Printf(TEXT("%'d"), WArmor);
        FNumberFormattingOptions FormatOptions;
        FormatOptions.UseGrouping = true;
        const FText Text = FText::AsNumber(WArmor, &FormatOptions);
        InfoArmor->SetText(Text);
    }
}

void UInfoWidget::OnBackClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (Player)
        {
            Player->PlayerCanMove();
            this->Hide();
        }
        UE_LOG(LogTemp, Log, TEXT("InfoWidgetHide"));
}

void UInfoWidget::OnRefreshClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    Player->SetStatusWithDmgAm();
}