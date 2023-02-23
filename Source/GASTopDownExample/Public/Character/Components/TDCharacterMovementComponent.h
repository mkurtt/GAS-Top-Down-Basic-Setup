// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TDCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASTOPDOWNEXAMPLE_API UTDCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UTDCharacterMovementComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void StopMovement();
	
protected:
	virtual void BeginPlay() override;
	void BindActions();
	
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnDestinationReached();
	
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;
	
private:
	UPROPERTY(EditAnywhere, Category=Input)
	float ShortPressThreshold;
	UPROPERTY(EditAnywhere, Category=Input)
	class UNiagaraSystem* FXCursor;
	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* SetDestinationClickAction;
	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* StopMovementAction;
	
	FVector CachedDestination;
	float FollowTime; // For how long it has been pressed

	UPROPERTY()
	class ATDCharacter* OwnerCharacter;
	UPROPERTY()
	class ATDPlayerController* OwnerPlayerController;
	UPROPERTY()
	class UPathFollowingComponent* PFollowComp;
	UPROPERTY()
	class UNavigationSystemV1* NavigationSystem;

public:
	
};
