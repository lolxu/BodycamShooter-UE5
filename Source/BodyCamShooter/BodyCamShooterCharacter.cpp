// Copyright Epic Games, Inc. All Rights Reserved.

#include "BodyCamShooterCharacter.h"
#include "BodyCamShooterProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// ABodyCamShooterCharacter

ABodyCamShooterCharacter::ABodyCamShooterCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a SpringArm for Camera
	SpringArmCameraComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmCamera"));
	SpringArmCameraComponent->SetupAttachment(GetCapsuleComponent());
	SpringArmCameraComponent->SetRelativeLocation(FVector(0.0f, 0.f, 60.f));
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArmCameraComponent);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a SpringArm for Arms
	SpringArmMeshComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmMesh"));
	SpringArmMeshComponent->SetupAttachment(FirstPersonCameraComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(SpringArmMeshComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ABodyCamShooterCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Setup DOF
	GetWorldTimerManager().SetTimer(DOFTimerHandle, this, &ABodyCamShooterCharacter::AutoDOF, 0.2f, true);
}

void ABodyCamShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HeadBob();
}

//////////////////////////////////////////////////////////////////////////// Input

void ABodyCamShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABodyCamShooterCharacter::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Ongoing, this, &ABodyCamShooterCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABodyCamShooterCharacter::StopSprinting);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABodyCamShooterCharacter::Look);
	}
}


void ABodyCamShooterCharacter::AutoDOF()
{
	FVector CameraWorldPos = FirstPersonCameraComponent->GetComponentLocation();
	FVector CameraForward = FirstPersonCameraComponent->GetForwardVector();
	FVector LineTraceEndVector = CameraForward * 10000.0f;

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraWorldPos, LineTraceEndVector, ECC_Visibility))
	{
		FVector FocusVec = HitResult.Location - CameraWorldPos;
		FocusLocation = FocusVec.Length();
		// Calling a blueprint function
		SetDOF();
	}
}

void ABodyCamShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABodyCamShooterCharacter::StartSprinting()
{
	if (UCharacterMovementComponent* MyCMC = GetCharacterMovement())
	{
		if (!bIsSprinting)
		{
			MyCMC->MaxWalkSpeed *= SprintSpeedMultiplier;
			MyCMC->MaxAcceleration *= SprintAccelMultiplier;
		}
	}
	bIsSprinting = true;
}

void ABodyCamShooterCharacter::StopSprinting()
{
	bIsSprinting = false;
	if (UCharacterMovementComponent* MyCMC = GetCharacterMovement())
	{
		MyCMC->MaxWalkSpeed /= SprintSpeedMultiplier;
		MyCMC->MaxAcceleration /= SprintAccelMultiplier;
	}
}

void ABodyCamShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABodyCamShooterCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ABodyCamShooterCharacter::GetHasRifle()
{
	return bHasRifle;
}