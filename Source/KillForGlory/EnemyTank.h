// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyTank.generated.h"

/**
 * 
 */
UCLASS()
class KILLFORGLORY_API AEnemyTank : public AEnemy
{
	GENERATED_BODY()
public:

	AEnemyTank();
	
protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//// Colliders
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* attackHitBox;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* specialHitBox;
	////

	//// Event
	UFUNCTION()
    void OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
    void OnSpecialHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void enableBasicAttack();
	UFUNCTION(BlueprintCallable)
	void disableBasicAttack();

	UFUNCTION(BlueprintCallable)
	void startSpecialAttack();
	UFUNCTION(BlueprintCallable)
	void stopSpecialAttack();
	UFUNCTION(BlueprintCallable)
	void enableSpecialAttack();
	UFUNCTION(BlueprintCallable)
	void disableSpecialAttack();
	////
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=EnemyTank)
	float specialCooldown = 0;
	float currentSpecialCooldown = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=EnemyTank)
	float specialDamage = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=EnemyTank)
	float specialStun = 0;
};
