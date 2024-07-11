// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MapWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/InputComponent.h"
#include "HUD/AllMenuWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/InputSettings.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Slate/SlateBrushAsset.h"
#include "Framework/Application/SlateApplication.h" 
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"

void UMapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    bCaveState = false;

    bIceLandState= false;

   bForestState = false;

   if (MessageText)
   {
       MessageText->SetText(FText::FromString(TEXT("Unknown Region")));
       MessageText->SetVisibility(ESlateVisibility::Hidden); // 처음엔 숨겨놓음
   }

    if (IceLand)
    {
        IceLand->OnClicked.AddDynamic(this, &UMapWidget::OnIceLandClicked);
    }
    if (Forest)
    {
        Forest->OnClicked.AddDynamic(this, &UMapWidget::OnForestClicked);
    }
    if (Cave)
    {
        Cave->OnClicked.AddDynamic(this, &UMapWidget::OnCaveClicked);
    }
    if (Back)
    {
        Back->OnClicked.AddDynamic(this, &UMapWidget::OnBackClicked);
    }
}

void UMapWidget::OnIceLandClicked()
{
    if (bIceLandState) {
        AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        this->Hide();
        if (Player)
        {
            PlayMapTransSound();
            Player->SetActorLocation(IceLandLocation);
            Player->PlayerCanMove();
        }
    }
    else {
        UnknownRegionMessage();
    }
}

void UMapWidget::OnForestClicked()
{
    if (bForestState) {
        AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        this->Hide();
        if (Player)
        {
            PlayMapTransSound();
            Player->SetActorLocation(ForestLocation);
            Player->PlayerCanMove();
        }
    }
    else {
        UnknownRegionMessage();
    }
}

void UMapWidget::OnCaveClicked()
{
    if (bCaveState) {
        AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        this->Hide();
        if (Player)
        {
            PlayMapTransSound();
            Player->SetActorLocation(CaveLocation);
            Player->PlayerCanMove();
        }
    }
    else {
        UnknownRegionMessage();
    }
}

void UMapWidget::PlayMapTransSound()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (MapTransSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            MapTransSound,
            Player->GetActorLocation()
        );
    }
}

void UMapWidget::PlayUnknownRegionSound()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (UnknownRegionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            UnknownRegionSound,
            Player->GetActorLocation()
        );
    }
}

void UMapWidget::OnBackClicked()
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (AllMenuWidgetClass) {
        AllMenuWidgetInstance = CreateWidget<UAllMenuWidget>(GetWorld(), AllMenuWidgetClass);
        if (AllMenuWidgetInstance)
        {
            Player->PlayerCanMove();
            this->Hide();
        }
        UE_LOG(LogTemp, Log, TEXT("MapWidgetHide"));
    }
}

void UMapWidget::UnknownRegionMessage()
{
    FTimerHandle MessageEndTimerHandle;
    if (MessageText)
    {
        MessageText->SetVisibility(ESlateVisibility::Visible);
        // 1초 후 HideMessage 함수를 호출하여 메시지를 숨김
        GetWorld()->GetTimerManager().SetTimer(MessageEndTimerHandle, this, &UMapWidget::HideMessage, 1.0f, false);
    }
}

void UMapWidget::HideMessage()
{
    if (MessageText)
    {
        MessageText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UMapWidget::Show()
{
    this->SetVisibility(ESlateVisibility::Visible);
}

void UMapWidget::Hide()
{
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UMapWidget::MapCaveOpen()
{
    bCaveState = true;
}

void UMapWidget::MapIceLandOpen()
{
    bIceLandState = true;
}

void UMapWidget::MapForestOpen()
{
    bForestState = true;
}

