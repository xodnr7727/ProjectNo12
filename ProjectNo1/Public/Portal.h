// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"
class USoundBase;
class UParticleSystemComponent;
UCLASS()
class PROJECTNO1_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector TeleportLocation;

	UFUNCTION()
	void PlayTransSound();

private:
	UPROPERTY(EditAnywhere, Category = "Portal")
	USoundBase* PortalTransSound;

	UPROPERTY(VisibleAnywhere, Category = "Portal")
	UParticleSystemComponent* PortalEffect;
};
