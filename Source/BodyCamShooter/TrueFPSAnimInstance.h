// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "TrueFPSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BODYCAMSHOOTER_API UTrueFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTrueFPSAnimInstance();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION()
	virtual void CurrentWeaponChanged(class AWeapon* NewWeapon, const class AWeapon* OldWeapon);
	
	virtual void SetVars(const float DeltaTime);
	virtual void CalculateWeaponSway(const float DeltaTime);

	virtual void SetIKTransforms();

public:
	/**
	 * @brief References for Characters
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	class ATrueFPSCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	class AWeapon* CurrentWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FIKProperties IKProperties;

	/*
	 * IK Variables
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FTransform CameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FTransform RelativeCameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FTransform RHandToSightsTransform;
};
