// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyProNo1HUD.generated.h"
class USlashOverlay;
class UInventoryUI;
class UGameRetryUI;
class UAllMenuWidget;
class UInfoWidget;
class UDamageIncreaseWidget;
class UMapWidget;
/**
 * 
 */
UCLASS()
class PROJECTNO1_API AMyProNo1HUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	void PlayerStartUI();
	void PlayerShowUI();
	void PlayerHideUI();
	void PlayerDeadUI();
private:
	UPROPERTY(EditDefaultsOnly, Category = Slash)
		TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
		USlashOverlay* SlashOverlay;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<UInventoryUI> InventoryUIClass;

	UPROPERTY()
	UInventoryUI* InventoryUI;

	UPROPERTY(EditDefaultsOnly, Category = GamePlayUI)
	TSubclassOf<UGameRetryUI> GameRetryUIClass;

	UPROPERTY()
	UGameRetryUI* GameRetryUI;

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
	FORCEINLINE UInventoryUI* GetInventoryUI() const { return InventoryUI; }
	FORCEINLINE UGameRetryUI* GetGameRetryUI() const { return GameRetryUI; }
};