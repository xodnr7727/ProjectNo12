// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Components/BoxComponent.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

    PortalEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PortalEffect"));
    PortalEffect->SetupAttachment(RootComponent);

	TriggerBox->SetCollisionProfileName("Trigger");
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::NotifyActorBeginOverlap(AActor* OtherActor) //Æ÷Å» ÀÌµ¿
{
    AProjectNo1Character* ProjectNo1Character = Cast<AProjectNo1Character>(OtherActor);
    if (ProjectNo1Character)
    {
        if (ProjectNo1Character)
        {
            PlayTransSound();
            ProjectNo1Character->SetActorLocation(TeleportLocation);
        }
    }
}

void APortal::PlayTransSound()
{
    if (PortalTransSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            PortalTransSound,
            GetActorLocation()
        );
    }
}