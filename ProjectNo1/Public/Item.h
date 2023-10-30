// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped,
	EIS_NeckEquipped,
	EIS_Weapon,
	EIS_Usingth,
};
enum class EWeaponAState : uint8
{
	EIS_Attaking,
	EIS_NoAttaking
};
UCLASS()
class PROJECTNO1_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
		float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
		float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
		float TransformedSin();

	UFUNCTION(BlueprintPure)
		float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();

	UFUNCTION()
	virtual void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* Capsule;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
		USoundBase* PickupSound;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float RunningTime;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* PickupEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
