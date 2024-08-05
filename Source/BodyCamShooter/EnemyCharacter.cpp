// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemyCharacter::StartDeathSequence(FVector BulletDirection, FHitResult HitResult)
{
	bStartMove = true;
	MoveDirection = BulletDirection;
	bIsHit = true;
	EnemyHitResult = HitResult;
	
	// FTimerDelegate DeathDelegate = FTimerDelegate::CreateUObject(this, &AEnemyCharacter::RagdollAndDie, BulletDirection);
	// GetWorldTimerManager().SetTimer(DeathTimerHandle, DeathDelegate, EnemyHitTime, false);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// void AEnemyCharacter::RagdollAndDie(FVector BulletDirection)
// {
// 	bStartMove = false;
// 	bIsDead = true;
// 	
// 	if (USkeletalMeshComponent* MyMesh = GetMesh())
// 	{
// 		MyMesh->SetSimulatePhysics(true);
// 	}
// }

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (bStartMove)
	// {
	// 	GetCapsuleComponent()->SetSimulatePhysics(true);
	// 	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	// 	GetCapsuleComponent()->AddImpulse(MoveDirection * 1500.0f);
	// }

	bIsHit = false;
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

