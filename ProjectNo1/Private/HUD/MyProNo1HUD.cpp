// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyProNo1HUD.h"
#include "HUD/SlashOverlay.h"

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