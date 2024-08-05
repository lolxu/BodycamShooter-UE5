// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Chaos/ConvexFlattenedArrayStructureData.h"
#include "GameFramework/SpringArmComponent.h"
#include "BodyCamShooterCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ABodyCamShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Spring Arm Component For Camera*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmCameraComponent;

	/** Spring Arm Component For Arm Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmMeshComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	
public:
	ABodyCamShooterCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	// Custom BP Function to make head bop lmao
	UFUNCTION(BlueprintImplementableEvent)
	void HeadBob();

	// Custom BP Function to set DOF Post Processing
	UFUNCTION(BlueprintImplementableEvent)
	void SetDOF();

	// Ray trace function for DOF
	void AutoDOF();

	// Get the Focus Location from Ray Trace Result for Focal Point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FocusLocation;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for start sprint input */
	void StartSprinting();

	/** Called for start sprint input */
	void StopSprinting();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// The dash movement multiplier
	UPROPERTY(EditAnywhere)
	float SprintSpeedMultiplier = 2.0f;

	// The dash movement accelerator multiplier
	UPROPERTY(EditAnywhere)
	float SprintAccelMultiplier = 5.0f;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	
	// Status for Sprinting
	bool bIsSprinting;

	// Timer to manage DOF
	FTimerHandle DOFTimerHandle;
	
};

