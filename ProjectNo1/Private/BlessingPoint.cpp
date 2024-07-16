// Fill out your copyright notice in the Description page of Project Settings.


#include "BlessingPoint.h"
#include "Goblin.h"
#include "BossCharacter.h"
#include "CaveEnemy.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include <Kismet/GameplayStatics.h>

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
            if (Player->IsBlessingPointInteract())
            {
                if (RegionName == "Cave")
                {
                    RespawnGhoulMonsters();
                    Player->BlessingPointCaveSetting();
                    Player->BlessingPointInteract();
                }
                else if (RegionName == "IceLand")
                {
                    RespawnGoblinMonsters();
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

void ABlessingPoint::RespawnGoblinMonsters()
{
    TArray<AActor*> FoundGoblinMonsters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoblin::StaticClass(), FoundGoblinMonsters);

    for (AActor* GoblinMonsterActor : FoundGoblinMonsters)
    {
        AGoblin* Goblin = Cast<AGoblin>(GoblinMonsterActor);
        if (Goblin && Goblin->IsDead())
        {
            Goblin->Respawn();
        }
    }
}

void ABlessingPoint::RespawnDragonMonsters()
{
    TArray<AActor*> FoundDragonMonsters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossCharacter::StaticClass(), FoundDragonMonsters);

    for (AActor* DragonMonsterActor : FoundDragonMonsters)
    {
        ABossCharacter* Dragon = Cast<ABossCharacter>(DragonMonsterActor);
        if (Dragon && Dragon->IsDead())
        {
            Dragon->Respawn();
        }
    }
}

void ABlessingPoint::RespawnGhoulMonsters()
{
    TArray<AActor*> FoundGhoulMonsters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACaveEnemy::StaticClass(), FoundGhoulMonsters);

    for (AActor* GhoulMonsterActor : FoundGhoulMonsters)
    {
        ACaveEnemy* Ghoul = Cast<ACaveEnemy>(GhoulMonsterActor);
        if (Ghoul && Ghoul->IsDead())
        {
            Ghoul->Respawn();
        }
    }
}



