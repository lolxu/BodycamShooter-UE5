// Fill out your copyright notice in the Description page of Project Settings.


#include "TrueFPSCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "ParticleHelper.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "UObject/UnrealTypePrivate.h"

// Sets default values
ATrueFPSCharacter::ATrueFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetTickGroup(TG_PostUpdateWork);
	
	// Create a SpringArm for Camera
	// SpringArmCameraComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmCamera"));
	// SpringArmCameraComponent->SetupAttachment(GetMesh(), FName("head"));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "head");
	Camera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void ATrueFPSCharacter::BeginPlay()
{
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
	GetWorldTimerManager().SetTimer(DOFTimerHandle, this, &ATrueFPSCharacter::AutoDOF, 0.2f, true);

	// Setting up replication
	if (HasAuthority())
	{
		for (const TSubclassOf<AWeapon>& WeaponClass : DefaultWeapons)
		{
			if (!WeaponClass) continue;
			FActorSpawnParameters Params;
			Params.Owner = this;
			AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Params);
			const int32 Index = Weapons.Add(SpawnedWeapon);

			// If found the weapon we should start with
			if (Index == CurrentIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				OnRep_CurrentWeapon(nullptr); // No weapon at start
			}
		}
	}
}

void ATrueFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, CurrentWeapon, COND_None);
}

void ATrueFPSCharacter::OnRep_CurrentWeapon(const AWeapon* OldWeapon)
{
	if (CurrentWeapon)
	{
		if (!CurrentWeapon->CurrentOwner)
		{
			const FTransform& PlacementTranceform = CurrentWeapon->PlacementTransform * GetMesh()->GetSocketTransform(FName("weaponsocket_r"));
			
			// Equip Weapon (Attach to character)
			CurrentWeapon->SetActorTransform(GetMesh()->GetSocketTransform(FName("weaponsocket_r")), false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("weaponsocket_r"));
			
			CurrentWeapon->CurrentOwner = this;
		}

		CurrentWeapon->Mesh->SetVisibility(true);
	}

	if (OldWeapon)
	{
		OldWeapon->Mesh->SetVisibility(false);
	}

	OnCurrentWeaponChanged.Broadcast(CurrentWeapon, OldWeapon);
}

void ATrueFPSCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);
}

void ATrueFPSCharacter::PreviousWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
}

// Called every frame
void ATrueFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HeadBob();
}

// Called to bind functionality to input
void ATrueFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrueFPSCharacter::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Ongoing, this, &ATrueFPSCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATrueFPSCharacter::StopSprinting);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrueFPSCharacter::ToggleCrouch);
		
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrueFPSCharacter::Look);

		//Weapons
		EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Triggered, this, &ATrueFPSCharacter::NextWeapon);
		EnhancedInputComponent->BindAction(PreviousWeaponAction, ETriggerEvent::Triggered, this, &ATrueFPSCharacter::PreviousWeapon);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATrueFPSCharacter::ShootWeapon);
	}

}

void ATrueFPSCharacter::EquipWeapon(const int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index])
	{
		return;
	}

	if (IsLocallyControlled() || HasAuthority())
	{
		CurrentIndex = Index;
		
		const AWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);
	}

	if (!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
	}
}

void ATrueFPSCharacter::ShootWeapon()
{
	if (GetMesh()->GetAnimInstance())
	{
		UFunction* Func = GetMesh()->GetAnimInstance()->FindFunction(FName("ProceduralRecoil"));
		if (Func == nullptr)
		{
			return;
		}

		// Calling a blueprint function
		FStructOnScope FuncParam(Func);
		UProperty* ReturnProp = nullptr;
		for (TFieldIterator<UProperty> It(Func); It; ++It)
		{
			UProperty* Prop = *It;
			if (Prop->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				ReturnProp = Prop;
			}
			else
			{
			}
		}

		GetMesh()->GetAnimInstance()->ProcessEvent(Func, FuncParam.GetStructMemory());
		CurrentWeapon->StartFire();
	}
}

void ATrueFPSCharacter::ToggleCrouch()
{
	if (bIsCrouching)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
	bIsCrouching = !bIsCrouching;
}

void ATrueFPSCharacter::Server_SetCurrentWeapon_Implementation(AWeapon* NewWeapon)
{
	const AWeapon* OldWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}

void ATrueFPSCharacter::AutoDOF()
{
	FVector CameraWorldPos = Camera->GetComponentLocation();
	FVector CameraForward = Camera->GetForwardVector();
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

void ATrueFPSCharacter::Move(const FInputActionValue& Value)
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

void ATrueFPSCharacter::StartSprinting()
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

void ATrueFPSCharacter::StopSprinting()
{
	bIsSprinting = false;
	if (UCharacterMovementComponent* MyCMC = GetCharacterMovement())
	{
		MyCMC->MaxWalkSpeed /= SprintSpeedMultiplier;
		MyCMC->MaxAcceleration /= SprintAccelMultiplier;
	}
}

void ATrueFPSCharacter::Look(const FInputActionValue& Value)
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

