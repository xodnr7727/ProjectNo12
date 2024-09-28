// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyProNo1HUD.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/SlashOverlay.h"
#include "HUD/InventoryUI.h"
#include "HUD/GameRetryUI.h"

void AMyProNo1HUD::BeginPlay()
{
	Super::BeginPlay();
	PlayerStartUI();
}

void AMyProNo1HUD::PlayerStartUI()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
			SlashOverlay->AddToViewport();
			SlashOverlay->Hide();
		}
	}
}

void AMyProNo1HUD::PlayerShowUI()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SlashOverlayClass)
		{
			SlashOverlay->Show();
		}
	}
}

void AMyProNo1HUD::PlayerHideUI()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SlashOverlayClass)
		{
			SlashOverlay->Hide();
		}
	}
}


void AMyProNo1HUD::PlayerDeadUI()
{
	// 게임 종료 UI 생성
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && GameRetryUIClass)
		{
			GameRetryUI = CreateWidget<UGameRetryUI>(Controller, GameRetryUIClass);
			GameRetryUI->AddToViewport();
		}
	}
}