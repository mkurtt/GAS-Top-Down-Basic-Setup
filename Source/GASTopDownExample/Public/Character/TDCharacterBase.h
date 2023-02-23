
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GAS/GASCharacterBase.h"
#include "TDCharacterBase.generated.h"

UENUM()
enum class CharacterState : uint8
{
	CS_Idle,
	CS_Moving,

	CS_MAX
};

UCLASS()
class GASTOPDOWNEXAMPLE_API ATDCharacterBase : public AGASCharacterBase
{
	GENERATED_BODY()

public:
	ATDCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UTDCharacterMovementComponent* MovementComponent;
	
	CharacterState CurrentState;

// Getters and Setters
public:
	FORCEINLINE UTDCharacterMovementComponent* GetWMovementComponent() const {return MovementComponent;}
	void SetCharacterState(CharacterState State);
	FORCEINLINE CharacterState GetCharacterState() const {return CurrentState;}
};
