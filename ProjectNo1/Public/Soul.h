// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Soul.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTNO1_API ASoul : public AItem
{
	GENERATED_BODY()

protected:
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


private:

public:

	void Update(float DeltaTime);

	FVector Position;
	FVector Direction;
	float Speed;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
		int32 Souls;


	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }

};
