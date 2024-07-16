// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlessingPoint.generated.h"

UCLASS()
class PROJECTNO1_API ABlessingPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlessingPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void RespawnGoblinMonsters();
	void RespawnDragonMonsters();
	void RespawnGhoulMonsters();
private:
	UPROPERTY(EditAnywhere, Category = "Point")
	FString RegionName; // 지역 이름 (예: "Cave", "IceLand", "Forest")

	UPROPERTY(VisibleAnywhere, Category = "Point")
	UParticleSystemComponent* BlessingPointEffect;

};
