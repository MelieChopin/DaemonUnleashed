// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "KFGPlayer.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	boxCollision->SetupAttachment(RootComponent);
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
    boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::EnemyDamage(int _damage)
{
	if(currentLife > 0)
		currentLife -= _damage;

	if(currentLife <= 0)
		Destroy();
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,FString::FromInt(currentLife));
}

void AEnemy::EnableAttackHitBox()
{
	boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DisableAttackHitBox()
{
	boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag("Player") && OtherComp->ComponentHasTag("Body"))
	{
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red, "EnemyHit");
		AKFGPlayer* player = Cast<AKFGPlayer>(OtherActor);
		if(player != nullptr)
			player->PlayerDamage(damage);
	}
	
	if (OtherActor == nullptr || !isAttacking)
		return;
}