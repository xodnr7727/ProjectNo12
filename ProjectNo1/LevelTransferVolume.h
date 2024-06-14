// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AttributeComponent.h"
#include "LevelTransferVolume.generated.h"
class USoundBase;
class UParticleSystemComponent;
UCLASS()
class PROJECTNO1_API ALevelTransferVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTransferVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CheckMonsters();

	UFUNCTION()
	void ActivateTransition();

	void PlayLevelTransSound();

	void PlayLevelOpenSound();

	void DeactivateLevelEffect();

	void ActivateLevelEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAttributeComponent* AttributeComponent;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void ShowLoadingScreen();

	void TransitionLevel();

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FName TransferLevelName;

	UPROPERTY()
		class UBoxComponent* TransferVolume;

		int32 RemainingMonsters;

		bool bIsTransferVolumeActive;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UUserWidget> LoadingScreenWidgetClass;

		class UUserWidget* LoadingScreenWidget;

		UPROPERTY(EditAnywhere, Category = "Level")
		USoundBase* LevelTransSound;

		UPROPERTY(EditAnywhere, Category = "Level")
		USoundBase* LevelOpenSound;

		UPROPERTY(VisibleAnywhere, Category = "Level")
		UParticleSystemComponent* LevelOpenEffect;
};
