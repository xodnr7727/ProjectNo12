// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/StunBarComponent.h"
#include "HUD/StunBar.h"
#include "Components/ProgressBar.h"

void UStunBarComponent::SetStunPercent(float Percent)
{
	if (StunBarWidget == nullptr)
	{
		StunBarWidget = Cast<UStunBar>(GetUserWidgetObject());
	}

	if (StunBarWidget && StunBarWidget->StunBar)
	{
		StunBarWidget->StunBar->SetPercent(Percent);
	}
}