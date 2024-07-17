// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNo1GameMode.h"
#include "ProjectNo1Character.h"
#include "MyProGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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
	UMyProGameInstance* GameInstance = Cast<UMyProGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		GameInstance->Init();
	}
	if (AProjectNo1Character* PlayerCharacter = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharacter->LoadPlayerState();
	}
}

void AProjectNo1GameMode::RestartPlayerAtPlayerStart(ACharacter* Character)
{
	if (Character)
	{
		//PlayerStart ��ġ�� ã���ϴ�.
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

		if (PlayerStarts.Num() > 0)
		{
			// ù ��° PlayerStart ��ġ�� �÷��̾ ����ŸƮ�մϴ�.
			AActor* PlayerStart = PlayerStarts[0];
			Character->SetActorLocation(PlayerStart->GetActorLocation());
			Character->SetActorRotation(PlayerStart->GetActorRotation());
		}
	}
}