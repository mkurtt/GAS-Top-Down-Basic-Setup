// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GASPlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class GASTOPDOWNEXAMPLE_API AGASPlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASPlayerStateBase();
	
protected:
	virtual void BeginPlay() override;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	
	UPROPERTY()
	class UTDAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	class UTDAttributeSetBase* AttributeSet;
	
	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle ManaRegenRateChangedDelegateHandle;
	FDelegateHandle MovementSpeedChangedDelegateHandle;
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void ManaRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void StunTagChanged(FGameplayTag GameplayTag, int I);
	
private:

// Getters and Setters
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UTDAttributeSetBase* GetAttributeSet() const;
};
