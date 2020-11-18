// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPlayer.h"
#include "Engine/World.h"
#include "AIController.h"
#include "GameFramework/PlayerController.h"
#include "Enemy.h"


EBTNodeResult::Type UMoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemy* enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if(enemy == nullptr)
        return EBTNodeResult::Aborted;
        
    FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
    FVector enemyLocation = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
    
    OwnerComp.GetAIOwner()->MoveToLocation(playerLocation,100.f);
    
    return EBTNodeResult::Succeeded;
}
