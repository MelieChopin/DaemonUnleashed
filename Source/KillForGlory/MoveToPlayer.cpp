// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPlayer.h"
#include "Engine/World.h"
#include "AIController.h"
#include "GameFramework/PlayerController.h"
#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UMoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemy* enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if(enemy != nullptr)
    {
        float distance = enemy->distanceToFocusPlayer;
        FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
        FVector enemyLocation = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
        if ((playerLocation - enemyLocation).Size() < distance)
        {
            enemy->isFollowingPlayer = true;
            FVector newVector = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation() - playerLocation;
            newVector.Normalize();
            newVector = newVector * enemy->distanceWaitToAttackPlayer;
            FVector EP = playerLocation - OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
            OwnerComp.GetAIOwner()->MoveToLocation((EP + newVector) + OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation());
            
            FRotator rot = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), playerLocation);
            rot.Pitch = 0;
            OwnerComp.GetAIOwner()->GetPawn()->SetActorRotation(rot);
            return EBTNodeResult::Succeeded;
        }
    }
    
    return EBTNodeResult::Failed;
}
