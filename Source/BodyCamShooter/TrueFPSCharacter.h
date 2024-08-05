// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Chaos/ConvexFlattenedArrayStructureData.h"
#include "GameFramework/SpringArmComponent.h"
#include "TrueFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponChanged, class AWeapon*, CurrentWeapon, const class AWeapon*, OldWeapon);

UCLASS(config=Game)
class BODYCAMSHOOTER_API ATrueFPSCharacter : public ACharacter
{
	GENERATED_BODY()

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

	/** Next Weapon Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* NextWeaponAction;

	/** Previous Weapon Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* PreviousWeaponAction;

	/** SHOOT */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	/** Crouch **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

public:
	// Sets default values for this character's properties
	ATrueFPSCharacter();

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

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

	void HandSway();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for start sprint input */
	void StartSprinting();

	/** Called for start sprint input */
	void StopSprinting();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// The sprint movement multiplier
	UPROPERTY(EditAnywhere)
	float SprintSpeedMultiplier = 5.0f;

	// The sprint movement accelerator multiplier
	UPROPERTY(EditAnywhere)
	float SprintAccelMultiplier = 8.0f;

	// The weapon classes spawned by default
	UPROPERTY(EditDefaultsOnly, Category = "Configurations")
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

	UFUNCTION()
	virtual void OnRep_CurrentWeapon(const class AWeapon* OldWeapon);

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(class AWeapon* NewWeapon);
	virtual void Server_SetCurrentWeapon_Implementation(class AWeapon* NewWeapon);

	virtual void NextWeapon();
	virtual void PreviousWeapon();

	void ToggleCrouch();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* Camera;

	// /** Spring Arm Component For Camera*/
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	// class USpringArmComponent* SpringArmCameraComponent;

	// Replicated Weapons
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
	TArray<class AWeapon*> Weapons;

	// Index of Weapons
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	int32 CurrentIndex = 0;

	// The Current Weapon you are equipped
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon, Category = "State")
	class AWeapon* CurrentWeapon;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCurrentWeaponChanged OnCurrentWeaponChanged;

	UFUNCTION(BlueprintCallable)
	virtual void EquipWeapon(const int32 Index);

private:
	// Status for Sprinting
	bool bIsSprinting;

	bool bIsCrouching = false;

	// Timer to manage DOF
	FTimerHandle DOFTimerHandle;

	void ShootWeapon();
	
};
