// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Engine/Blueprint.h"

#include "AEnemyMage.generated.h"

class AParticle;
/**
 * 
 */
UCLASS()
class KILLFORGLORY_API AAEnemyMage : public AEnemy
{
	GENERATED_BODY()

public :
	AAEnemyMage();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AParticle> magicBall = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AParticle> invocation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Invocation)
	int numberOfInvocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Invocation)
	float cooldownInvocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Invocation)
	int numberOfUses;

	float cooldown = 2.0f;
	float timeInvocation = 0.0f;
	TArray<AActor*> listInvocation;

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
    void CreateMagicBall();

	UFUNCTION(BlueprintCallable)
    void CreateInvocation();

	UFUNCTION(BlueprintCallable)
    void ActiveInvocation();
};
