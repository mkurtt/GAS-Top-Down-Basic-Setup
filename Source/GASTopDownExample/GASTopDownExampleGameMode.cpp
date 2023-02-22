// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASTopDownExampleGameMode.h"
#include "GASTopDownExamplePlayerController.h"
#include "GASTopDownExampleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGASTopDownExampleGameMode::AGASTopDownExampleGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGASTopDownExamplePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}