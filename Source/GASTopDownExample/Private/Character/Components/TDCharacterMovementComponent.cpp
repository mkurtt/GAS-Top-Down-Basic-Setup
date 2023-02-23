// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/TDCharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/NavigationSystemBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Character/TDCharacter.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "Player/TDPlayerController.h"


UTDCharacterMovementComponent::UTDCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void UTDCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ATDCharacter>(GetOwner());
	OwnerPlayerController = Cast<ATDPlayerController>(OwnerCharacter->GetController());

	BindActions();
}

void UTDCharacterMovementComponent::BindActions()
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(OwnerPlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &UTDCharacterMovementComponent::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &UTDCharacterMovementComponent::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &UTDCharacterMovementComponent::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &UTDCharacterMovementComponent::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(StopMovementAction, ETriggerEvent::Triggered, this, &UTDCharacterMovementComponent::StopMovement);
	}
}

void UTDCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerCharacter->GetCharacterState() == CharacterState::CS_Moving)
	{
		PFollowComp ? PFollowComp : PFollowComp = OwnerPlayerController->FindComponentByClass<UPathFollowingComponent>();
		if (PFollowComp)
		{
			if (PFollowComp->HasReached(CachedDestination, EPathFollowingReachMode::OverlapAgentAndGoal))
			{
				OnDestinationReached();
			}
		}
	}
}

void UTDCharacterMovementComponent::OnInputStarted()
{
	//FNavigationSystem::StopMovement(*OwnerPlayerController);
	//OwnerCharacter->SetCharacterState(CharacterState::CS_Idle);
}

void UTDCharacterMovementComponent::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();

	FHitResult Hit;
	bool bHitSuccessful = OwnerPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	//Hold To Move
	if (OwnerCharacter != nullptr)
	{
		FVector WorldDirection = (CachedDestination - OwnerCharacter->GetActorLocation()).GetSafeNormal();
		OwnerCharacter->AddMovementInput(WorldDirection, 1.0, false);
		OwnerCharacter->SetCharacterState(CharacterState::CS_Moving);
	}
}

void UTDCharacterMovementComponent::OnSetDestinationReleased()
{
	if (FollowTime <= ShortPressThreshold)
	{
		//Click To Move
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerPlayerController, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		OwnerCharacter->SetCharacterState(CharacterState::CS_Moving);
	}
	else
	{
		//Hold To Move End
		OwnerCharacter->SetCharacterState(CharacterState::CS_Idle);
	}

	FollowTime = 0.f;
}

void UTDCharacterMovementComponent::OnDestinationReached()
{
	//Click To Move End
	OwnerCharacter->SetCharacterState(CharacterState::CS_Idle);
}

void UTDCharacterMovementComponent::StopMovement()
{
	if (OwnerCharacter->GetCharacterState() == CharacterState::CS_Moving)
	{
		FNavigationSystem::StopMovement(*OwnerPlayerController);
		OwnerCharacter->SetCharacterState(CharacterState::CS_Idle);
	}
}

