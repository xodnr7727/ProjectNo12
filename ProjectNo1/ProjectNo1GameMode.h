// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectNo1GameMode.generated.h"

UCLASS(minimalapi)
class AProjectNo1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectNo1GameMode();
	virtual void BeginPlay() override;
	void RestartPlayerAtPlayerStart(ACharacter* Character);

	UFUNCTION()
	void OpenStartWidget();

protected:
	UFUNCTION()
	void StartWidget();

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class  UStartWidget> StartWidgetClass;

	UPROPERTY()
	UStartWidget* StartWidgetInstance;
};



