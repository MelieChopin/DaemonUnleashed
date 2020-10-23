// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayerHuman.h"



#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "KFGPlayerDeamon.h"
#include "Camera/PlayerCameraManager.h"
#include "Chaos/Utilities.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"


AKFGPlayerHuman::AKFGPlayerHuman()
{
    attackHitBox = CreateDefaultSubobject<UBoxComponent>("AttackHitBox");
    attackHitBox->SetupAttachment(RootComponent);
    attackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AKFGPlayerHuman::OnAttackHitBoxBeginOverlap);
    attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    wallJumpSphereCollision = CreateDefaultSubobject<USphereComponent>("WallJumpSphereCollision");
    wallJumpSphereCollision->SetupAttachment(RootComponent);
    wallJumpSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AKFGPlayerHuman::OnWallJumpBeginOverlap);

    specialHitBox = CreateDefaultSubobject<UBoxComponent>("SpecialHitBox");
    specialHitBox->SetupAttachment(RootComponent);
    specialHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AKFGPlayerHuman::BeginPlay()
{
    Super::BeginPlay();
    wallJumpSphereCollision->OnComponentEndOverlap.AddDynamic(this, &AKFGPlayerHuman::OnWallJumpEndOverlap);
    deamonForm = GetWorld()->SpawnActor<AKFGPlayerDeamon>(deamonFormClass.Get(), GetActorLocation(), FRotator::ZeroRotator);
    deamonForm->SetHumanForm(this);
    deamonForm->SetActorHiddenInGame(true);
    deamonForm->SetActorEnableCollision(false);

    isPossessed = true;
}

void AKFGPlayerHuman::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(isPossessed)
    {
        deamonForm->SetActorTransform(GetActorTransform());

        *transformRatio -= DeltaTime * *transformRatioSpeed;
    }
    
    if (isFreeze)
    {
        timeForWallJump += DeltaTime;
        GetCharacterMovement()->GravityScale = 0.0f;
        GetCharacterMovement()->Velocity = FVector(0, 0, -speedFriction);
        if (timeForWallJump >= timeBeforeFalling)
        {
            GetCharacterMovement()->GravityScale = 1.0f;
            isFreeze = false;
            timeForWallJump = 0.0f;
        }
    }

    if(currentCooldownSpecialAttack > 0 && playerState != EPlayerState::SPECIAL)
    {
        currentCooldownSpecialAttack -= DeltaTime;
    }

    if(playerState == EPlayerState::SPECIAL)
    {
        TArray<AActor*> overlappedEnemyList;
        specialHitBox->GetOverlappingActors(overlappedEnemyList,TSubclassOf<AEnemy>());
        SpecialActorOverlapped(overlappedEnemyList);
    }
}

void AKFGPlayerHuman::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Roll_Charge", IE_Pressed,this, &AKFGPlayerHuman::Roll);
    PlayerInputComponent->BindAction("Attack", IE_Pressed,this, &AKFGPlayerHuman::Attack);
    PlayerInputComponent->BindAction("Transform", IE_Pressed, this, &AKFGPlayerHuman::TransformToDeamon);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AKFGPlayerHuman::Jumping);
    PlayerInputComponent->BindAction("Special", IE_Pressed, this, &AKFGPlayerHuman::Special);
    //Call parent
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AKFGPlayerHuman::MoveForward(float Value)
{
    // Cancel attack anim if player is in recover time
    if ( (Controller != NULL) && (Value != 0.0f))
    {
        if(recoverAttack)
        {
            StopAnimMontage(GetCurrentMontage());
            AttackReset();
        }
        Super::MoveForward(Value);
    }
}

void AKFGPlayerHuman::MoveRight(float Value)
{
    // Cancel attack anim if player is in recover time
    if ( (Controller != NULL) && (Value != 0.0f))
    {
        if(recoverAttack)
        {
            StopAnimMontage(GetCurrentMontage());
            AttackReset();
        }
        Super::MoveRight(Value);
    }
}

void AKFGPlayerHuman::Roll()
{
    if(playerState == EPlayerState::ROLL)
        return;
    
    PlayAnimMontage(rollAnim);
}

void AKFGPlayerHuman::RollStart()
{
    changePlayerState(EPlayerState::ROLL);
}

void AKFGPlayerHuman::RollEnd()
{
    playerState = EPlayerState::NONE;
}

void AKFGPlayerHuman::TransformToDeamon()
{
    if(deamonForm != nullptr)
    {
        deamonForm->SetActorTransform(GetActorTransform());
        deamonForm->SetActorEnableCollision(true);
        deamonForm->SetActorHiddenInGame(false);
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        APlayerCameraManager* cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
        FRotator camRotation = cameraManager->GetCameraRotation();
        FRotator playerRotation = GetActorRotation();
        GetController()->Possess(deamonForm);
        deamonForm->GetCharacterMovement()->Velocity = GetVelocity();
        GetWorld()->GetFirstPlayerController()->RotationInput = camRotation-playerRotation;
        isPossessed = false;
        deamonForm->isPossessed = true;
    }
}

void AKFGPlayerHuman::Attack()
{
    bufferAttack = true;

    if(playerState != EPlayerState::ATTACK && playerState != EPlayerState::ROLL)
    {
        attackNum = 0;
        changePlayerState(EPlayerState::ATTACK);
        AttackLaunch();
    }
    else if(playerState == EPlayerState::ATTACK && recoverAttack)
        AttackLaunch();
}

void AKFGPlayerHuman::AttackLaunch()
{
    if(!bufferAttack)
        return;

    AActor* enemyTarget = findNearestEnemyFromInput();
    
    if(enemyTarget != nullptr)
    {
        FRotator rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),enemyTarget->GetActorLocation());
        rot.Pitch = 0;
        rot.Roll = 0;
        SetActorRotation(rot);
    }
	
    UAnimMontage* animToPlay = nullptr;

    bufferAttack = false;
    recoverAttack = false;
	
    switch (attackNum)
    {
    case 0:
        animToPlay = combo1Anim;
        break;
    case 1:
        animToPlay = combo2Anim;
        break;
    case 2:
        animToPlay = combo3Anim;
        break;
    default:
        break;
    }

    if(animToPlay != nullptr)
        PlayAnimMontage(animToPlay);

    attackNum = attackNum < 2 ? attackNum+1 : 0;
}

void AKFGPlayerHuman::AttackReset()
{
    playerState = EPlayerState::NONE;

    bufferAttack = false;
    recoverAttack = false;
    attackNum = 0;
}

void AKFGPlayerHuman::Special()
{
    if(currentCooldownSpecialAttack <= 0 && (playerState == EPlayerState::NONE || recoverAttack))
    {
        PlayAnimMontage(specialAttack);
        currentCooldownSpecialAttack = cooldownSpecialAttack;
        changePlayerState(EPlayerState::SPECIAL);
    }
}

void AKFGPlayerHuman::EnableSpecial()
{
    specialHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AKFGPlayerHuman::DisableSpecial()
{
    specialHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKFGPlayerHuman::SpecialEnd()
{
    changePlayerState(EPlayerState::NONE);
}

void AKFGPlayerHuman::SpecialActorOverlapped(const TArray<AActor*>& enemyList)
{
    for(AActor* actor : enemyList)
    {
        AEnemy* enemy = Cast<AEnemy>(actor);
        if(enemy != nullptr) // Check if cast work
            enemy->EnemyDamage(specialDamage);
    }
}

void AKFGPlayerHuman::EnableAttackHitBox()
{
    attackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AKFGPlayerHuman::DisableAttackHitBox()
{
    attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKFGPlayerHuman::OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GEngine->AddOnScreenDebugMessage(1,1,FColor::Blue,"Hit");
    if(OtherActor->ActorHasTag("Enemy"))
    {
        AEnemy* enemy = Cast<AEnemy>(OtherActor);
        if(enemy != nullptr)
            enemy->EnemyDamage(attackDamage);
    }
}

void AKFGPlayerHuman::Jumping()
{
    if (isFreeze)
    {
        GetCharacterMovement()->GravityScale = 1.0f;
        GetCharacterMovement()->Velocity = FVector(normal * FVector(400, 400, 0) + FVector(0, 0, 800));
        isFreeze = false;
        return;
    }
	
    Jump();
}

void AKFGPlayerHuman::OnWallJumpBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor->ActorHasTag("WallJump"))
        return;

    FCollisionQueryParams TraceParams;
    FHitResult hit;

    GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100, ECC_PhysicsBody, TraceParams);

    if (hit.GetActor())
        normal = hit.Normal;

    if (GetMovementComponent()->IsFalling())
    {
        GetCharacterMovement()->GravityScale = 0.0f;
        GetCharacterMovement()->Velocity = FVector(0, 0, -speedFriction);
        isFreeze = true;
    }
}

void AKFGPlayerHuman::OnWallJumpEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    GetCharacterMovement()->GravityScale = 1.0f;
    isFreeze = false;
    timeForWallJump = 0.0f;
}

AActor* AKFGPlayerHuman::findNearestEnemyFromInput()
{
    TArray<AActor*> foundEnemy;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(),foundEnemy);
    
    AActor* enemyTarget = nullptr;
    for(AActor* enemy : foundEnemy)
    {
        if(FVector::Dist(enemy->GetActorLocation(),GetActorLocation()) <= 350.f)
        {
            if(enemyTarget == nullptr)
            {
                enemyTarget = enemy;
                continue;
            }
            
            FVector inputDir(GetInputAxisValue("MoveForward"),GetInputAxisValue("MoveRight"),0);
            inputDir.Normalize();
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            FVector Direction = GetActorLocation() + (YawRotation.Quaternion() * inputDir) *150;
            
            if(FVector::Dist(enemy->GetActorLocation(),Direction) < FVector::Dist(enemyTarget->GetActorLocation(),Direction))
                enemyTarget = enemy;
        }
    }

    return enemyTarget;
}

void AKFGPlayerHuman::changePlayerState(EPlayerState newPlayerState)
{
    if(playerState == newPlayerState)
        return;
	
    switch (playerState)
    {
    case EPlayerState::ATTACK:
        AttackReset(); break;
    case EPlayerState::ROLL:
        RollEnd(); break;
    case EPlayerState::SPECIAL: break;
    case EPlayerState::NONE: break;
    default: ;
    }

    playerState = newPlayerState;
}
