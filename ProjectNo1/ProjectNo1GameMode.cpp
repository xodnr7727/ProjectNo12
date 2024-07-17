// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNo1GameMode.h"
#include "ProjectNo1Character.h"
#include "MyProGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HUD/StartWidget.h"

AProjectNo1GameMode::AProjectNo1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AProjectNo1GameMode::BeginPlay()
{
	Super::BeginPlay();
	StartWidget();
}

void AProjectNo1GameMode::StartWidget()
{
	if (StartWidgetClass)
	{
		StartWidgetInstance = CreateWidget<UStartWidget>(GetWorld(), StartWidgetClass);
		if (StartWidgetInstance)
		{
			StartWidgetInstance->AddToViewport();
		}
	}
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetPause(true);
		PC->bShowMouseCursor = true;
	}
}

void AProjectNo1GameMode::OpenStartWidget()
{
	if (StartWidgetInstance)
	{
		StartWidgetInstance->Show();
	}
}

void AProjectNo1GameMode::RestartPlayerAtPlayerStart(ACharacter* Character)
{
	if (Character)
	{
		//PlayerStart 위치를 찾습니다.
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

		if (PlayerStarts.Num() > 0)
		{
			// 첫 번째 PlayerStart 위치로 플레이어를 리스타트합니다.
			AActor* PlayerStart = PlayerStarts[0];
			Character->SetActorLocation(PlayerStart->GetActorLocation());
			Character->SetActorRotation(PlayerStart->GetActorRotation());
		}
	}
}