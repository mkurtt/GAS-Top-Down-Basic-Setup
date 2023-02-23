
#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "TDPlayerController.generated.h"

UCLASS()
class GASTOPDOWNEXAMPLE_API ATDPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATDPlayerController();
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, Category=Camera)
	TSubclassOf<class ATDCameraController> CameraControllerClass;
	
	UPROPERTY()
	ATDCameraController* CameraController;

public:
	FORCEINLINE ATDCameraController* GetCameraController() const {return CameraController;}
	
};
