#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FIKProperties
{
	GENERATED_BODY()

	// Base pose for Inverse Kinematics
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* AnimPose;

	// Difference between camera and aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimOffset = 15.0f;

	// Custom Weapon Offset Transform
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CustomOffsetTransform;
};

UCLASS(Abstract)
class AWeapon : public AActor
{
	GENERATED_BODY()
public:
	AWeapon();

	// Function to fire rifle
	void StartFire();
	void StopFire();
	void FireShot();

protected:
	virtual void BeginPlay() override;

	// Timeout between shots
	FTimerHandle TimerHandle_HandleRefire;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* Mesh;

	// Current owner of this weapon
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	class ATrueFPSCharacter* CurrentOwner;

	// Struct for IK Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FIKProperties IKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FTransform PlacementTransform;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
	FTransform GetSightsWorldTransform() const;
	virtual FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); };

	// Fire rate for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float FireRate;

	// Range of this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float WeaponRange = 20000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class UParticleSystem* MuzzleParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class UParticleSystem* ImpactParticles;
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
};