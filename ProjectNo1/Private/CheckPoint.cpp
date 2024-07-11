// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "Components/BoxComponent.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "HUD/SlashOverlay.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/MapWidget.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	MapPointEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MapPointEffect"));
	MapPointEffect->SetupAttachment(RootComponent);

	TriggerBox->SetCollisionProfileName("Trigger");
}

// Called when the game starts or when spawned
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckPoint::NotifyActorBeginOverlap(AActor* OtherActor) //Æ÷Å» ÀÌµ¿
{
    AProjectNo1Character* ProjectNo1Character = Cast<AProjectNo1Character>(OtherActor);
    if (ProjectNo1Character)
    {
        if (RegionName == "Cave")
        {
            RegionOpen();
            ProjectNo1Character->CaveRegionOpen();
        }
        else if (RegionName == "IceLand")
        {
            RegionOpen();
            ProjectNo1Character->IceLandRegionOpen();
        }
        else if (RegionName == "Forest")
        {
            RegionOpen();
            ProjectNo1Character->ForestRegionOpen();
        }                                                                                   
    }
}

void ACheckPoint::RegionOpen()
{
    ActivateMapCheckParticles();
    PlayMapOpenSound();
    Destroy();
}

void ACheckPoint::PlayMapOpenSound()
{
    if (MapOpenSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            MapOpenSound,
            GetActorLocation()
        );
    }
}

void ACheckPoint::ActivateMapCheckParticles()
{
    if (MapOpenParticles && GetWorld())
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            MapOpenParticles,
            GetActorLocation()
        );
    }
}

