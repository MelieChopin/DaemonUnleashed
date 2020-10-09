// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayer.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AKFGPlayer

AKFGPlayer::AKFGPlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	attackHitBox = CreateDefaultSubobject<UBoxComponent>("AttackHitBox");
	attackHitBox->SetupAttachment(RootComponent);
	attackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AKFGPlayer::OnAttackHitBoxBeginOverlap);
	attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKFGPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	currentLife = maxLife;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKFGPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKFGPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKFGPlayer::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AKFGPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AKFGPlayer::LookUpAtRate);

	PlayerInputComponent->BindAction("Roll", IE_Pressed,this, &AKFGPlayer::Roll);
	PlayerInputComponent->BindAction("Attack", IE_Pressed,this, &AKFGPlayer::Attack);
}

void AKFGPlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKFGPlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AKFGPlayer::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// Cancel attack anim if player is in recover time
		if(recoverAttack)
		{
			StopAnimMontage(GetCurrentMontage());
			AttackReset();
		}
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AKFGPlayer::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f))
	{
		// Cancel attack anim if player is in recover time
		if(recoverAttack)
		{
			StopAnimMontage(GetCurrentMontage());
			AttackReset();
		}
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AKFGPlayer::Roll()
{
	if(isRolling)
		return;
    GEngine->AddOnScreenDebugMessage(1,1,FColor::Blue,"Roll");
	PlayAnimMontage(rollAnim);
}


void AKFGPlayer::RollStart()
{
	changePlayerState(EPlayerState::ROLL);
	isRolling = true;
}

void AKFGPlayer::RollEnd()
{
	playerState = EPlayerState::NONE;
	isRolling = false;
}

void AKFGPlayer::Attack()
{
	GEngine->AddOnScreenDebugMessage(1,1,FColor::Blue,  "Attack");	
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

void AKFGPlayer::AttackLaunch()
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

void AKFGPlayer::AttackReset()
{
	playerState = EPlayerState::NONE;

	bufferAttack = false;
	isAttacking = false;
	recoverAttack = false;
	attackNum = 0;
}


void AKFGPlayer::EnableAttackHitBox()
{
	attackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AKFGPlayer::DisableAttackHitBox()
{
	attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKFGPlayer::changePlayerState(EPlayerState newPlayerState)
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

void AKFGPlayer::OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(1,1,FColor::Blue,"Hit");
}

void AKFGPlayer::PlayerDamage(int damage)
{
	if(currentLife > 0)
		currentLife -= damage;
}
