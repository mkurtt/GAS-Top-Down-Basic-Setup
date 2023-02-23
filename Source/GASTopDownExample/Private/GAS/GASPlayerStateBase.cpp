// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GASPlayerStateBase.h"

#include "AbilitySystemComponent.h"
#include "GAS/TDAbilitySystemComponent.h"
#include "GAS/TDAttributeSetBase.h"

AGASPlayerStateBase::AGASPlayerStateBase()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UTDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<UTDAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));


}

void AGASPlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AGASPlayerStateBase::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &AGASPlayerStateBase::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthRegenRateAttribute()).AddUObject(this, &AGASPlayerStateBase::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddUObject(this, &AGASPlayerStateBase::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddUObject(this, &AGASPlayerStateBase::MaxManaChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaRegenRateAttribute()).AddUObject(this, &AGASPlayerStateBase::ManaRegenRateChanged);
		
		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGASPlayerStateBase::StunTagChanged);
	}
}

void AGASPlayerStateBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	// If the player died, handle death
	if (!(AttributeSet->GetHealth() > 0.f) && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		// Update floating status bar
		AGASCharacterBase* Character = Cast<AGASCharacterBase>(GetPawn());
		if (Character)
		{
			Character->Die();
		}
	}
}

void AGASPlayerStateBase::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	
}

void AGASPlayerStateBase::HealthRegenRateChanged(const FOnAttributeChangeData& Data)
{
	
}

void AGASPlayerStateBase::ManaChanged(const FOnAttributeChangeData& Data)
{
	
}

void AGASPlayerStateBase::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	
}

void AGASPlayerStateBase::ManaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	
}

void AGASPlayerStateBase::StunTagChanged(FGameplayTag GameplayTag, int NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}

UAbilitySystemComponent* AGASPlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTDAttributeSetBase* AGASPlayerStateBase::GetAttributeSet() const
{
	return AttributeSet;
}
