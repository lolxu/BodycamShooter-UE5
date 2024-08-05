// Fill out your copyright notice in the Description page of Project Settings.


#include "TrueFPSAnimInstance.h"

#include "TrueFPSCharacter.h"
#include "Camera/CameraComponent.h"

UTrueFPSAnimInstance::UTrueFPSAnimInstance()
{
}

void UTrueFPSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	/*Character = Cast<ATrueFPSCharacter>(TryGetPawnOwner());
	if (Character)
	{
		Mesh = Character->GetMesh();
		Character->OnCurrentWeaponChanged.AddDynamic(this, &UTrueFPSAnimInstance::CurrentWeaponChanged);
		CurrentWeaponChanged(Character->CurrentWeapon, nullptr); // Needs to call this at beginning to setup
	}*/
}

void UTrueFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!Character)
	{
		Character = Cast<ATrueFPSCharacter>(TryGetPawnOwner());

		if (Character)
		{
			Mesh = Character->GetMesh();
			Character->OnCurrentWeaponChanged.AddDynamic(this, &UTrueFPSAnimInstance::CurrentWeaponChanged);
			CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
		}
		else
		{
			return;
		}
	}

	SetVars(DeltaTime);
	CalculateWeaponSway(DeltaTime);
}

void UTrueFPSAnimInstance::CurrentWeaponChanged(AWeapon* NewWeapon, const AWeapon* OldWeapon)
{
	CurrentWeapon = NewWeapon;
	if (CurrentWeapon)
	{
		IKProperties = CurrentWeapon->IKProperties;

		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTrueFPSAnimInstance::SetIKTransforms);
	}
}

void UTrueFPSAnimInstance::SetVars(const float DeltaTime)
{
	CameraTransform = FTransform(Character->GetBaseAimRotation(), Character->Camera->GetComponentLocation());

	// For meshes that have a included rotation
	const FTransform& RootOffset = Mesh->GetSocketTransform(FName("Root"), RTS_Component).Inverse() * Mesh->GetSocketTransform(FName("ik_hand_root"));
	RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);
}

void UTrueFPSAnimInstance::CalculateWeaponSway(const float DeltaTime)
{
	
}

void UTrueFPSAnimInstance::SetIKTransforms()
{
	RHandToSightsTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r")));
}
