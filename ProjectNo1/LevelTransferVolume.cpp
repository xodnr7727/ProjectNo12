// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransferVolume.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Characters/BaseCharacter.h"
#include "CharacterTypes.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item.h"
#include "Weapons/Weapon.h"
#include <string.h>

// Sets default values
ALevelTransferVolume::ALevelTransferVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));
	RootComponent = TransferVolume;
	TransferVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

// Called when the game starts or when spawned
void ALevelTransferVolume::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ALevelTransferVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ALevelTransferVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AProjectNo1Character* ProjectNo1Character = Cast<AProjectNo1Character>(OtherActor);
	if (ProjectNo1Character) {
		UGameplayStatics::OpenLevel(GetWorld(), FName(TransferLevelName));
	}
}

