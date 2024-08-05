// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class BODYCAMSHOOTER_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	void StartDeathSequence(FVector BulletDirection, FHitResult HitResult);

	UPROPERTY(BlueprintReadWrite)
	bool bIsDead = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsHit = false;

	UPROPERTY(BlueprintReadWrite)
	FHitResult EnemyHitResult;

	UPROPERTY(BlueprintReadWrite)
	float EnemyHitTime = 1.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RagdollAndDie(FVector BulletDirection);

	// Death timer handle
	FTimerHandle DeathTimerHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	bool bStartMove = false;
	float MoveSpeed = 300.0f;
	FVector MoveDirection;
};
