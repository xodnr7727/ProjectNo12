// Fill out your copyright notice in the Description page of Project Settings.


#include "BlessingPoint.h"
#include "Goblin.h"
#include "BossCharacter.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectNo1/ProjectNo1Character.h"

// Sets default values
ABlessingPoint::ABlessingPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
    CollisionBox->SetCollisionProfileName("Trigger");

    BlessingPointEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BlessingPointEffect"));
    BlessingPointEffect->SetupAttachment(RootComponent);

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABlessingPoint::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABlessingPoint::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ABlessingPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlessingPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlessingPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        AProjectNo1Character* Player = Cast<AProjectNo1Character>(OtherActor);
        if (Player)
        {
            Player->ShowInteractMessage();
            Player->bInRangePointTrue();
        }
    }
}

void ABlessingPoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        AProjectNo1Character* Player = Cast<AProjectNo1Character>(OtherActor);
        if (Player)
        {
            Player->HideInteractMessage();
            Player->bInRangePointFalse();
        }
    }
}

void ABlessingPoint::NotifyActorBeginOverlap(AActor* OtherActor) 
{
    AProjectNo1Character* Player = Cast<AProjectNo1Character>(OtherActor);
    if (Player && Player->IsBlessingPointInteract())
    {
        if (RegionName == "Cave")
        {
            RespawnGoblinMonsters();
            Player->BlessingPointCaveSetting();
            Player->BlessingPointInteract();
        }
        else if (RegionName == "IceLand")
        {
            RespawnGrugMonsters();
            Player->BlessingPointIceLandSetting();
            Player->BlessingPointInteract();
        }
        else if (RegionName == "Forest")
        {
            RespawnDragonMonsters();
            Player->BlessingPointForestSetting();
            Player->BlessingPointInteract();
        }
    }
}

void ABlessingPoint::RespawnGoblinMonsters()
{
}

void ABlessingPoint::RespawnDragonMonsters()
{
}

void ABlessingPoint::RespawnGrugMonsters()
{
}



