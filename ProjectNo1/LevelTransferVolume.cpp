// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransferVolume.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "ProjectNo1/LichEnemy.h"
#include "BossCharacter.h"
#include "Goblin.h"
#include "Characters/BaseCharacter.h"
#include "CharacterTypes.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item.h"
#include "Weapons/Weapon.h"
#include "Blueprint/UserWidget.h"
#include <string.h>

// Sets default values
ALevelTransferVolume::ALevelTransferVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));
	RootComponent = TransferVolume;
    bIsTransferVolumeActive = false;
    TransferVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TransferVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

    LevelOpenEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LevelOpenEffect"));
    LevelOpenEffect->SetupAttachment(RootComponent);

    AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
}

// Called when the game starts or when spawned
void ALevelTransferVolume::BeginPlay()
{
	Super::BeginPlay();
    CheckMonsters();
    if (LevelOpenEffect) {
        LevelOpenEffect->DeactivateSystem();
    }
}


// Called every frame
void ALevelTransferVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ALevelTransferVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
    FTimerHandle LevelTransitionTimerHandle;
	AProjectNo1Character* ProjectNo1Character = Cast<AProjectNo1Character>(OtherActor);
    if (bIsTransferVolumeActive)
    {
        if (ProjectNo1Character) {
            PlayLevelTransSound();
            ShowLoadingScreen();
            // Ÿ�̸� ����: 2�� ��� �� ���� ��ȯ

            GetWorld()->GetTimerManager().SetTimer(LevelTransitionTimerHandle, this, &ALevelTransferVolume::TransitionLevel, 2.0f, false);
        }
    }
}

void ALevelTransferVolume::ShowLoadingScreen()
{
    if (LoadingScreenWidgetClass)
    {
        LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidgetClass);
        if (LoadingScreenWidget)
        {
            LoadingScreenWidget->AddToViewport();
        }
    }
}

void ALevelTransferVolume::TransitionLevel()
{
    if (LoadingScreenWidget)
    {
        LoadingScreenWidget->RemoveFromViewport();
    }
    UGameplayStatics::OpenLevel(GetWorld(), FName(TransferLevelName));
}

void ALevelTransferVolume::CheckMonsters()
{
    TArray<AActor*> FoundLichEnemys;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALichEnemy::StaticClass(), FoundLichEnemys);
    RemainingMonsters = FoundLichEnemys.Num();

    // ���͵��� ��� ����� ���� �����ϴ� �̺�Ʈ�� ���
    for (AActor* LichEnemy : FoundLichEnemys)
    {
        ALichEnemy* LichEnemyInstance = Cast<ALichEnemy>(LichEnemy);
        if (LichEnemyInstance)
        {
            LichEnemyInstance->OnDestroyedDetected.AddDynamic(this, &ALevelTransferVolume::ActivateTransition);
        }
    }
}

void ALevelTransferVolume::ActivateTransition()
{
    RemainingMonsters--;
    if (RemainingMonsters <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("All monsters are defeated! Level transition is now active."));
        bIsTransferVolumeActive = true;
        TransferVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PlayLevelOpenSound();
        ActivateLevelEffect();
    }
}

void ALevelTransferVolume::PlayLevelTransSound()
{
    if (LevelTransSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            LevelTransSound,
            GetActorLocation()
        );
    }
}

void ALevelTransferVolume::PlayLevelOpenSound()
{
    if (LevelOpenSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            LevelOpenSound,
            GetActorLocation()
        );
    }
}

void ALevelTransferVolume::DeactivateLevelEffect()
{
    if (LevelOpenEffect)
    {
        LevelOpenEffect->DeactivateSystem();
    }
}

void ALevelTransferVolume::ActivateLevelEffect()
{
    if (LevelOpenEffect)
    {
        LevelOpenEffect->ActivateSystem();
    }
}