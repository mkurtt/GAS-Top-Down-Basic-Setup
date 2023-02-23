// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TDPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Character/TDCharacter.h"
#include "Player/TDCameraController.h"

ATDPlayerController::ATDPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	CameraController = GetWorld()->SpawnActor<ATDCameraController>(CameraControllerClass, GetPawn()->GetActorLocation(), FRotator(), SpawnParams);
	SetViewTarget(CameraController);
}
