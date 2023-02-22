// Fill out your copyright notice in the Description page of Project Settings.


#include "TDAssetManager.h"
#include "AbilitySystemGlobals.h"

UTDAssetManager& UTDAssetManager::Get() const
{
	UTDAssetManager* Singleton = Cast<UTDAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager, must be TDAssetManager!"));
		return *NewObject<UTDAssetManager>();	 // never calls this
	}
}


void UTDAssetManager::StartInitialLoading() 
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}