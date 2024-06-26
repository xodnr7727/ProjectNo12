// Fill out your copyright notice in the Description page of Project Settings.


#include "Soul.h"
#include "Interfaces/PickupInterface.h"

void ASoul::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddEx(this);
	
	SpawnPickupSystem();
	SpawnPickupSound();

	Destroy();
	}
}

void ASoul::Update(float DeltaTime)
{
	Position += Direction * Speed * DeltaTime;
}