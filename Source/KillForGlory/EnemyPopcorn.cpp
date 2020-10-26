// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPopcorn.h"

#include "KFGGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"

class AKFGPlayer;
// Sets default values
AEnemyPopcorn::AEnemyPopcorn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	isAttacking = false;

	boxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	boxCollision->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AEnemyPopcorn::BeginPlay()
{
	Super::BeginPlay();
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPopcorn::OnOverlapBegin);
    boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void AEnemyPopcorn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (isFollowingPlayer)
		timeAttack += DeltaTime;

	if (isAttacking)
	{
		if (changeIsAttacking <= 0.1)
			PlayAnimMontage(attackBasic);
		//GEngine->AddOnScreenDebugMessage(-5, 1.0f, FColor::Blue, FString::SanitizeFloat(timeAttack));
		changeIsAttacking += DeltaTime;
		if (changeIsAttacking >= timeAnimAttack)
		{
			StopAnimMontage(attackBasic);
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


void AEnemyPopcorn::EnableAttackHitBox()
{
	boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyPopcorn::DisableAttackHitBox()
{
	boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AEnemyPopcorn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag("Player") && OtherComp->ComponentHasTag("Body"))
	{
		//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red, OtherComp->GetName());
		AKFGPlayer* player = Cast<AKFGPlayer>(OtherActor);
		if(player != nullptr)
			player->PlayerDamage(damage);
	}
	
	if (OtherActor == nullptr || !isAttacking)
		return;
}






