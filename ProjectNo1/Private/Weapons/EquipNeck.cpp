// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/EquipNeck.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BaseCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AEquipNeck::AEquipNeck()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEquipNeck::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquipNeck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquipNeck::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AEquipNeck::EquipNeck(USceneComponent* InParent, FName InSocketName) {
	ItemState = EItemState::EIS_NeckEquipped;
	AttachMeshToSocket(InParent, InSocketName);
	PlayEquipSound();
}

void AEquipNeck::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}
