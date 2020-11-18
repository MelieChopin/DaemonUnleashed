// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToWaitAttack.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "AIController.h"
#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"

class AEnemy;

EBTNodeResult::Type UMoveToWaitAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    AEnemy* enemy = Cast<AEnemy>(Cast<ACharacter>(OwnerComp.GetAIOwner()->GetCharacter()));
    FVector playerLocation = player->GetActorLocation();
    FVector enemyLocation = enemy->GetActorLocation();

    if (player == nullptr || enemy == nullptr)
        return EBTNodeResult::Aborted;


    FVector distEnemyPlayer  = playerLocation - enemyLocation;
    FVector distRadiusPlayer = (distEnemyPlayer * -1).GetSafeNormal() * enemy->distanceWaitToAttackPlayer;
    FVector targetPos = (distEnemyPlayer + distRadiusPlayer) + enemyLocation;

    FHitResult result;
    FCollisionQueryParams param;
    param.AddIgnoredActor(enemy);

    GetWorld()->LineTraceSingleByChannel(result, enemyLocation, playerLocation, ECollisionChannel::ECC_Pawn, param);

    //DrawDebugLine(GetWorld(), enemyLocation, playerLocation, FColor::Red, false, 0.05f,0,10);

    enemy->isFollowingPlayer = true;

    if (result.GetActor()->ActorHasTag("Player"))
        OwnerComp.GetAIOwner()->MoveToLocation(targetPos);
    else if (result.GetActor()->ActorHasTag("Enemy"))
    {
        FVector newPos = targetPos - playerLocation;
        newPos = newPos.RotateAngleAxis(20, FVector::UpVector);
        newPos *= 1.6f;
        newPos += playerLocation;
        OwnerComp.GetAIOwner()->MoveToLocation(newPos);
    }

    FRotator rotation = UKismetMathLibrary::FindLookAtRotation(enemyLocation, playerLocation);
    rotation.Pitch = 0;
    enemy->SetActorRotation(rotation);
    
    return EBTNodeResult::Succeeded;
}
