// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPopcorn.h"
#include "KFGPlayer.h"
#include "Animation/AnimSequence.h"
#include "Engine/Engine.h"
#include "Components/BoxComponent.h"

class ASkeletalMeshActor;
// Sets default values
AEnemyPopcorn::AEnemyPopcorn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	isAttacking = false;
	
	boxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	boxCollision->SetRelativeTransform(FTransform(FQuat(),
		FVector(136.99176, 0.000168, -20.238766), FVector(1, 1, 1)));
	boxCollision->InitBoxExtent(FVector(57, 40, 35));
    boxCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyPopcorn::BeginPlay()
{
	Super::BeginPlay();

	timeAnimAttack = attack->SequenceLength / attack->RateScale;
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPopcorn::OnOverlapBegin);
	boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void AEnemyPopcorn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (isFollowingPlayer)
		timeAttack += DeltaTime;

	if (timeAttack >= speedAttack)
	{
		timeAttack = 0.0f;
		isAttacking = true;
	}
	
	if (isAttacking)
	{
		boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		changeIsAttacking += DeltaTime;
		if (changeIsAttacking >= timeAnimAttack)
		{
			changeIsAttacking = 0.0f;
			isAttacking = false;
		}
	}
}

// Called to bind functionality to input
void AEnemyPopcorn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyPopcorn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor == nullptr)
		return;

	AKFGPlayer* player = Cast<AKFGPlayer>(OtherActor);

	if (player != nullptr)
	{
		player->currentLife -= 1;
		GEngine->AddOnScreenDebugMessage(-3, 1.5f, FColor::Red, TEXT("COLLISION"));
	}
}

