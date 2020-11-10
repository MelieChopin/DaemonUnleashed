// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particle.generated.h"

UCLASS()
class KILLFORGLORY_API AParticle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticle();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool isActived = false;

	UPROPERTY(BlueprintReadWrite)
	bool toDestroy = false;
};
