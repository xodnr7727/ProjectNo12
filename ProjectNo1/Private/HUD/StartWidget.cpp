// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/StartWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include <MyProGameInstance.h>

void UStartWidget::NativeConstruct()
{
    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UStartWidget::OnStartClicked);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UStartWidget::OnExitClicked);
    }
}

void UStartWidget::OnExitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void UStartWidget::OnStartClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player) {
        Player->PlayerStartUI();
        Player->InitializeSlashOverlay();
    }
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PC->SetPause(false);
        PC->bShowMouseCursor = false;
    }
    UMyProGameInstance* GameInstance = Cast<UMyProGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInstance)
    {
        GameInstance->Init();
    }
    AProjectNo1Character* PlayerCharacter = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        PlayerCharacter->LoadPlayerState();
    }
    if (Player) {
        Player->SetStatusWithDmgAm();
    }
    this->Hide();
}

void UStartWidget::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UStartWidget::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}