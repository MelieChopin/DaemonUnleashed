// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayerHuman.h"

#include "KFGPlayerDeamon.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PawnMovementComponent.h"


AKFGPlayerHuman::AKFGPlayerHuman()
{
    attackHitBox = CreateDefaultSubobject<UBoxComponent>("AttackHitBox");
    attackHitBox->SetupAttachment(RootComponent);
    attackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AKFGPlayerHuman::OnAttackHitBoxBeginOverlap);
    attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    wallJumpSphereCollision = CreateDefaultSubobject<USphereComponent>("WallJumpSphereCollision");
    wallJumpSphereCollision->SetupAttachment(RootComponent);
    wallJumpSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AKFGPlayerHuman::OnWallJumpBeginOverlap);
}


void AKFGPlayerHuman::BeginPlay()
{
    Super::BeginPlay();
    wallJumpSphereCollision->OnComponentEndOverlap.AddDynamic(this, &AKFGPlayerHuman::OnWallJumpEndOverlap);
    deamonForm = GetWorld()->SpawnActor<AKFGPlayerDeamon>(deamonFormClass.Get(), GetActorLocation(), FRotator::ZeroRotator);
    deamonForm->SetHumanForm(this); 
}

void AKFGPlayerHuman::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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
}

void AKFGPlayerHuman::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Roll/Charge", IE_Pressed,this, &AKFGPlayerHuman::Roll);
    PlayerInputComponent->BindAction("Attack", IE_Pressed,this, &AKFGPlayerHuman::Attack);
    PlayerInputComponent->BindAction("Transform", IE_Pressed, this, &AKFGPlayerHuman::TransformToDeamon);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AKFGPlayerHuman::Jumping);
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
    }
    
    Super::MoveForward(Value);
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
    }
    Super::MoveRight(Value);
}

void AKFGPlayerHuman::Roll()
{
    if(isRolling)
        return;
    
    PlayAnimMontage(rollAnim);
}

void AKFGPlayerHuman::RollStart()
{
    changePlayerState(EPlayerState::ROLL);
    isRolling = true;
}

void AKFGPlayerHuman::RollEnd()
{
    playerState = EPlayerState::NONE;
    isRolling = false;
}

void AKFGPlayerHuman::TransformToDeamon()
{
    GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::Red,"Transform");
    if(deamonForm != nullptr)
        GetController()->Possess(deamonForm);
}

void AKFGPlayerHuman::Attack()
{
    bufferAttack = true;

    if(!isAttacking && !isRolling)
    {
        attackNum = 0;
        isAttacking = true;
        changePlayerState(EPlayerState::ATTACK);
        AttackLaunch();
    }
    else if(isAttacking && recoverAttack)
        AttackLaunch();
}

void AKFGPlayerHuman::AttackLaunch()
{
    if(!bufferAttack)
        return;
	
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
    isAttacking = false;
    recoverAttack = false;
    attackNum = 0;
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
    if (OtherActor != this && !OtherActor->ActorHasTag("WallJump"))
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
    default: ;
    }

    playerState = newPlayerState;
}
