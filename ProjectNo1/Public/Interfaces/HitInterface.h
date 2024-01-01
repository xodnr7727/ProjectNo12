// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"


UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTNO1_API IHitInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint, AActor* Hitter);
	UFUNCTION(BlueprintNativeEvent)
	void GetBlock(const FVector& ImpactPoint, AActor* Hitter);
	UFUNCTION(BlueprintNativeEvent)
	void GetStun(const FVector& ImpactPoint, AActor* Hitter);
	UFUNCTION(BlueprintNativeEvent)
	void BallHit(const FVector& ImpactPoint, AActor* Hitter);
	UFUNCTION(BlueprintNativeEvent)
	void ProjectileHit(FHitResult HitResult, AActor* Shooter, AController* ShooterController);
};
