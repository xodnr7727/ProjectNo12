// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNo1GameMode.h"
#include "ProjectNo1Character.h"
#include "MyProGameInstance.h"
#include "UObject/ConstructorHelpers.h"
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
	if (GameInstance && !GameInstance->bIsInitialized)
	{
		GameInstance->InitializeDefaultValues();
	}
	if (AProjectNo1Character* PlayerCharacter = Cast<AProjectNo1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharacter->LoadPlayerState();
	}
}