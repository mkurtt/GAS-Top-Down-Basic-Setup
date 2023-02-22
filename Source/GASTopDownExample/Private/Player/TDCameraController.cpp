// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TDCameraController.h"

#include "EnhancedInputComponent.h"
#include "Player/TDPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


ATDCameraController::ATDCameraController()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SetRootComponent(SphereComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(BaseCameraAngle, 0, 0));
	SpringArm->TargetArmLength = MaxSpringArm;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 6;
	SpringArm->CameraRotationLagSpeed = 6;
	SpringArm->bDoCollisionTest = false;
	CameraComponent->SetupAttachment(SpringArm);
}

void ATDCameraController::BeginPlay()
{
	Super::BeginPlay();

	TargetZoom = BaseSpringArm;
	
	TargetVector = GetActorLocation();
	SpringArm->SetRelativeRotation(FRotator(BaseCameraAngle, 0, 0));
	TargetPitchRotation = BaseCameraAngle;

	FTimerHandle Handler;
	// Wait to make sure the viewport is initialized
	GetWorldTimerManager().SetTimer(Handler, this, &ATDCameraController::SetBoundaries, 0.1f, false, 0.2f);

	GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &ATDCameraController::ViewportSizeChanged);

	BindActions();
}

void ATDCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MouseMovement(DeltaTime);

	MoveTo(DeltaTime);
	if (HeightAdjustment)
	{
		SetTargetHeight();
		AdjustHeight(DeltaTime);
	}

	if (GetActorRotation().Yaw != TargetYawRotation)
	{
		const FRotator CurrentTargetRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(),
		                                                                     FRotator(GetActorRotation().Pitch,
		                                                                              TargetYawRotation,
		                                                                              GetActorRotation().Roll), DeltaTime,
		                                                                     RotationSmoothness);


		SetActorRotation(CurrentTargetRotation, ETeleportType::None);
	}

	if (SpringArm->GetRelativeRotation().Pitch != TargetPitchRotation)
	{
		const FRotator CurrentTargetRotation = UKismetMathLibrary::RInterpTo(SpringArm->GetRelativeRotation(),
		                                                                     FRotator(TargetPitchRotation,
		                                                                              SpringArm->GetRelativeRotation().Yaw,
		                                                                              SpringArm->GetRelativeRotation().Roll), DeltaTime,
		                                                                     RotationSmoothness);


		SpringArm->SetRelativeRotation(CurrentTargetRotation);
	}

	if (SpringArm->TargetArmLength != TargetZoom)
	{
		float CurrentTargetZoom = UKismetMathLibrary::FInterpTo(SpringArm->TargetArmLength,
		                                                        TargetZoom, DeltaTime,
		                                                        ZoomSmoothness);
		CurrentTargetZoom = UKismetMathLibrary::Clamp(CurrentTargetZoom, MinSpringArm, MaxSpringArm);
		SpringArm->TargetArmLength = CurrentTargetZoom;
	}
}

// Called to bind functionality to input
void ATDCameraController::BindActions()
{
	const auto PlayerInputComponent = Cast<ATDPlayerController>(GetOwner())->InputComponent;
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CameraMovementAction, ETriggerEvent::Triggered, this, &ATDCameraController::CamMovement);
		EnhancedInputComponent->BindAction(CameraMovementAction, ETriggerEvent::Ongoing, this, &ATDCameraController::CamMovement);
		// EnhancedInputComponent->BindAxis(YAxis, this, &ATDCameraController::YMovement);
		// EnhancedInputComponent->BindAxis(RotationAxis, this, &ATDCameraController::RotateCamera);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ATDCameraController::CamZoom);
	}
}

void ATDCameraController::CamMovement(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0)
	{
		XMovement(Value[0]);
		YMovement(Value[1]);
	}
}

void ATDCameraController::CamZoom(const FInputActionValue& Value)
{
	if (Value[0] != 0)
	{
		Zoom(Value[0]);
	}
}

void ATDCameraController::XMovement(const float AxisValue)
{
	FVector Movement;
	Movement.X = AxisValue * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Y = 0;
	Movement.Z = 0;
	Movement = UKismetMathLibrary::TransformDirection(GetActorTransform(), Movement);
	TargetVector.X = TargetVector.X + Movement.X;
	TargetVector.Y = TargetVector.Y + Movement.Y;
}

void ATDCameraController::YMovement(const float AxisValue)
{
	FVector Movement;
	Movement.X = 0;
	Movement.Y = AxisValue * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Z = 0;
	Movement = UKismetMathLibrary::TransformDirection(GetActorTransform(), Movement);
	TargetVector.X = TargetVector.X + Movement.X;
	TargetVector.Y = TargetVector.Y + Movement.Y;
}

void ATDCameraController::MouseMovement(const float DeltaTime)
{
	if (GetInputAxisValue(XAxis) || GetInputAxisValue(YAxis))
	{
		return;
	}

	float DirectionX = 0;
	float DirectionY = 0;

	FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	if (this->IsValidMousePosition(MousePosition, Top))
	{
		DirectionX = 1;
	}
	else if (IsValidMousePosition(MousePosition, Bottom))
	{
		DirectionX = -1;
	}

	if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Right))
	{
		DirectionY = 1;
	}
	else if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Left))
	{
		DirectionY = -1;
	}

	FVector Movement;
	Movement.X = DirectionX * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Y = DirectionY * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Z = 0;
	Movement = UKismetMathLibrary::TransformDirection(GetActorTransform(), Movement);
	TargetVector.X = TargetVector.X + Movement.X;
	TargetVector.Y = TargetVector.Y + Movement.Y;
}


void ATDCameraController::MoveTo(const float DeltaTime)
{
	const FVector CurrentTargetVector = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetVector, DeltaTime,
	                                                                  MovementSmoothness);
	SetActorLocation(FVector(CurrentTargetVector.X, CurrentTargetVector.Y, GetActorLocation().Z));
}

void ATDCameraController::AdjustHeight(float DeltaTime)
{
	const FVector CurrentLocation = GetActorLocation();

	if (CurrentLocation.Z != TargetHeight)
	{
		const float HeightToSet = UKismetMathLibrary::FInterpTo(CurrentLocation.Z, TargetHeight,
		                                                        DeltaTime,
		                                                        HeightAdjustmentSmoothness);
		SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, HeightToSet));
	}
}

void ATDCameraController::RotateCamera(float AxisValue)
{
	int ZoomStep = (MaxSpringArm - MinSpringArm) / ZoomSteps;

	TargetYawRotation = TargetYawRotation + (AxisValue * RotationMultiplier * GetAdjustedDeltaTime());
}

void ATDCameraController::Zoom(float AxisValue)
{
	if (AxisValue != 0)
	{
		TargetZoom = TargetZoom + (AxisValue * ZoomMultiplier);

		if (UseZoomSteps)
		{
			const int ZoomStepSize = (MaxSpringArm - MinSpringArm) / ZoomSteps;
			const float ZoomStep = SpringArm->TargetArmLength / ZoomStepSize;
			int Step = UKismetMathLibrary::Round(ZoomStep);
			if (AxisValue > 0)
			{
				Step++;
			}
			else if (AxisValue < 0)
			{
				Step--;
			}
			TargetZoom = Step * ZoomStepSize;
		}

		const float PitchPercentage = UKismetMathLibrary::FClamp((TargetZoom - MinSpringArm) / (AngleChangeStartDistance - MinSpringArm), 0, 1);
		TargetPitchRotation = (BaseCameraAngle - AngleAtMinDistance) * PitchPercentage + AngleAtMinDistance;

		TargetZoom = UKismetMathLibrary::Clamp(TargetZoom, MinSpringArm, MaxSpringArm);
	}
}

bool ATDCameraController::IsValidMousePosition(const FVector2D Positions, const FVector4 Rules)
{
	const bool InputY = Positions.Y >= Rules.X && Positions.Y <= Rules.Y;
	const bool InputX = Positions.X >= Rules.Z && Positions.X <= Rules.W;
	return InputX && InputY;
}

void ATDCameraController::SetBoundaries()
{
	const FVector2D Result = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

	Top = FVector4(0, 0, 0, 0);
	Bottom = FVector4(0, 0, 0, 0);
	Left = FVector4(0, 0, 0, 0);
	Right = FVector4(0, 0, 0, 0);

	Top.Y = (MovementZoneInPercent / 100.f) * Result.Y;
	Top.W = Result.X;

	Bottom.X = (1 - (MovementZoneInPercent / 100.f)) * Result.Y;
	Bottom.Y = Result.Y;
	Bottom.W = Result.X;

	Left.Y = (MovementZoneInPercent / 100.f) * Result.X;
	Left.W = Result.Y;

	Right.X = (1 - (MovementZoneInPercent / 100.f)) * Result.X;
	Right.Y = Result.X;
	Right.W = Result.Y;

	StartHeight = GetActorLocation().Z;
	PreviousHeight = GetActorLocation().Z;

	if (IgnoreBoundaries)
	{
		Top.X = -Deactivate;
		Top.Z = -Deactivate;
		Top.W = Deactivate;

		Bottom.Y = Deactivate;
		Bottom.Z = -Deactivate;
		Bottom.W = Deactivate;

		Left.X = -Deactivate;
		Left.Z = -Deactivate;
		Left.W = Deactivate;

		Right.Y = Deactivate;
		Right.Z = -Deactivate;
		Right.W = Deactivate;
	}
}

void ATDCameraController::SetTargetHeight()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation();

	if (TranceFromCamera)
	{
		Start = CameraComponent->GetComponentLocation();
		End = CameraComponent->GetComponentLocation();
	}

	Start.Z = 999999;
	End.Z = -999999;

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(TraceChannel);
	GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, QueryParams);

	if (UKismetMathLibrary::Abs(PreviousHeight - HitResult.Location.Z) >= Threshold)
	{
		TargetHeight = HitResult.Location.Z;
		PreviousHeight = HitResult.Location.Z;
	}
	else if (UKismetMathLibrary::Abs(HitResult.Location.Z - StartHeight) < Threshold)
	{
		TargetHeight = StartHeight;
	}
}

float ATDCameraController::GetAdjustedDeltaTime() const
{
	return GetWorld()->GetDeltaSeconds() * 100;
}


void ATDCameraController::ViewportSizeChanged(FViewport* ViewPort, uint32 val)
{
	SetBoundaries();
}
