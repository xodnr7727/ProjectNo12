// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyProNo1HUD.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/SlashOverlay.h"
#include "HUD/InventoryUI.h"

void AMyProNo1HUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
			SlashOverlay->AddToViewport();
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
		if (Controller && InventoryUIClass)
		{
			InventoryUI = CreateWidget<UInventoryUI>(Controller, InventoryUIClass);
			InventoryUI->AddToViewport();
		}
	}
}