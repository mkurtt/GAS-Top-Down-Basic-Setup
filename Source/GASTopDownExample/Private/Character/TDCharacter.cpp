#include "Character/TDCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Components/TDCharacterMovementComponent.h"

ATDCharacter::ATDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTDCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void ATDCharacter::SetCharacterState(CharacterState NewState)
{
	if (CurrentState == NewState) return;

	switch (NewState)
	{
	case CharacterState::CS_Idle:
		break;
	case CharacterState::CS_Moving:
		break;
	case CharacterState::CS_MAX:
		break;
	default: ;
	}
	CurrentState = NewState;

	UE_LOG(LogTemp, Log, TEXT("CurrentCharacterState : %s"), *UEnum::GetValueAsString(CurrentState));
}
