#include "Weapon.h"

// #include "NPCCharacter.h"
#include "EnemyCharacter.h"
#include "TrueFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "Physics/ImmediatePhysics/ImmediatePhysicsShared/ImmediatePhysicsCore.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AWeapon::StartFire()
{
	FireShot();

	GetWorldTimerManager().SetTimer(TimerHandle_HandleRefire, this, &AWeapon::FireShot, FireRate, true);
}

void AWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleRefire);
}

void AWeapon::FireShot()
{

	FHitResult Hit;

	// For raycast from gun tip
	const FVector StartTrace = Mesh->GetBoneLocation(FName("b_gun_muzzleflash"));
	const FVector EndTrace = (Mesh->GetRightVector() * WeaponRange) + StartTrace;

	// For raycast from center of camera
	// const FVector StartTrace = CurrentOwner->Camera->GetComponentLocation();
	// const FVector EndTrace = CurrentOwner->Camera->GetForwardVector() * WeaponRange + StartTrace;
	
	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(WeaponTrace), false, CurrentOwner);

	// Play recoil animation
	FOutputDeviceNull Ar;
	CurrentOwner->GetMesh()->GetAnimInstance()->CallFunctionByNameWithArguments(TEXT("ProceduralRecoil 1.5f"), Ar, nullptr, true);

	// DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true, 1.0f);
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, QueryParams))
	{

		// For Debugging
		// if(GEngine)
		// {
		// 	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Hit.GetActor()->GetName());	
		// }
		
		FVector Direction = EndTrace - StartTrace;
		Direction.Normalize();
		
		if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Hit.GetActor()))
		{
			EnemyCharacter->StartDeathSequence(Direction, Hit);
		}
		else if (Hit.GetComponent()->IsSimulatingPhysics())
		{
			Hit.GetComponent()->AddImpulseAtLocation(Direction * 10080.0f, Hit.Location);
		}
		
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));
		}
	}

	if (MuzzleParticles)
	{
		FTransform SpawnTransform = Mesh->GetBoneTransform(Mesh->GetBoneIndex(FName("b_gun_muzzleflash")));
		FRotator NewRotation = FRotationMatrix::MakeFromX(Mesh->GetRightVector()).Rotator();
		SpawnTransform.SetRotation(NewRotation.Quaternion());
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticles, SpawnTransform);
	}
	
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!CurrentOwner)
	{
		Mesh->SetVisibility(false);
	}
}
