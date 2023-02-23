
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GASGameplayAbilityBase.h"
#include "GASCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterBaseHitReactDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGASCharacterBase*, Character);

UCLASS()
class GASTOPDOWNEXAMPLE_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASCharacterBase(const FObjectInitializer& ObjectInitializer);

	void RemoveCharacterAbilities();
	
	virtual void Die();
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual void FinishDying();

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetAbilityLevel(EGDAbilityInputID AbilityID) const;

	// Set the Hit React direction in the Animation Blueprint
	UPROPERTY(BlueprintAssignable, Category = "GASDocumentation|GDCharacter")
	FCharacterBaseHitReactDelegate ShowHitReact;
	UPROPERTY(BlueprintAssignable, Category = "GASDocumentation|GDCharacter")
	FCharacterDiedDelegate OnCharacterDied;
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void PlayHitReact(AActor* DamageCauser);
	
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	TWeakObjectPtr<class UTDAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UTDAttributeSetBase> AttributeSet;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;
	
	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|Animation")
	UAnimMontage* DeathMontage;

	
	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<UGASGameplayAbilityBase>> DefaultAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;
	
	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();
	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();
	virtual void AddStartupEffects();
	

private:


// Getters and Setters
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UTDAttributeSetBase* GetAttributeSet() const;
	virtual bool IsAlive() const;
};
