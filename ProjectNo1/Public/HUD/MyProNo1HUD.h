// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyProNo1HUD.generated.h"
class USlashOverlay;
/**
 * 
 */
UCLASS()
class PROJECTNO1_API AMyProNo1HUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = Slash)
		TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
		USlashOverlay* SlashOverlay;
public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
};