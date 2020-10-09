// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "EnemyPopcorn.generated.h"

UCLASS()
class KILLFORGLORY_API AEnemyPopcorn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyPopcorn();

	UPROPERTY(EditAnywhere)
	float speedAttack;

	UPROPERTY(EditAnywhere)
	float damage;

	UPROPERTY(EditAnywhere)
	float speedMove;

	UPROPERTY(EditAnywhere)
	float distanceToFocusPlayer;

	UPROPERTY(BlueprintReadWrite)
	bool isAttacking;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimSequence* attack;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* boxCollision;
	
	UPROPERTY()
	bool isFollowingPlayer;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float timeAttack;
	float changeIsAttacking;
	float timeAnimAttack;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
