// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNo1GameMode.h"
#include "ProjectNo1Character.h"
#include "UObject/ConstructorHelpers.h"

AProjectNo1GameMode::AProjectNo1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
