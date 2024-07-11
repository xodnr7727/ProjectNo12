// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"
class USoundBase;
class UParticleSystemComponent;
class USlashOverlay;
UCLASS()
class PROJECTNO1_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void RegionOpen();

	UFUNCTION()
	void PlayMapOpenSound();

	UFUNCTION()
	void ActivateMapCheckParticles();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Map")
	FString RegionName; // 지역 이름 (예: "Cave", "IceLand", "Forest")

	UPROPERTY(EditAnywhere, Category = "Map")
	USoundBase* MapOpenSound;

	UPROPERTY(VisibleAnywhere, Category = "Map")
	UParticleSystemComponent* MapPointEffect;

	UPROPERTY(EditAnywhere, Category = "Map")
	UParticleSystem* MapOpenParticles;

	UPROPERTY(EditAnywhere, Category = "Map")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Map")
	TSubclassOf<class UMapWidget> MapWidgetClass;

	UPROPERTY()
	UMapWidget* MapWidgetInstance;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

};
