
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDCharacter.generated.h"

UENUM()
enum class CharacterState : uint8
{
	CS_Idle,
	CS_Moving,

	CS_MAX
};

UCLASS()
class GASTOPDOWNEXAMPLE_API ATDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UTDCharacterMovementComponent* MovementComponent;
	
	CharacterState CurrentState;

public:
	FORCEINLINE UTDCharacterMovementComponent* GetWMovementComponent() const {return MovementComponent;}
	void SetCharacterState(CharacterState State);
	FORCEINLINE CharacterState GetCharacterState() const {return CurrentState;}
};
