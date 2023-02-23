// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TDAbilitySystemComponent.h"

void UTDAbilitySystemComponent::ReceiveDamage(UTDAbilitySystemComponent * SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}