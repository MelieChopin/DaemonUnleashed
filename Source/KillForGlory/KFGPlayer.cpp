// Fill out your copyright notice in the Description page of Project Settings.

#include "KFGPlayer.h"

#include "KFGGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AKFGPlayer

AKFGPlayer::AKFGPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
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
}

void AKFGPlayer::BeginPlay()
{
	Super::BeginPlay();

	currentLife = &Cast<AKFGGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->currentLife;
	deamonResources = &Cast<AKFGGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->deamonResources;
	deamonResourcesSpeedDecrease = &Cast<AKFGGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->deamonResourcesSpeedDecrease;
	deamonResourcesSpeedIncrease = &Cast<AKFGGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->deamonResourcesSpeedIncrease;

	if(currentLife == nullptr)
		EndPlay(EEndPlayReason::Quit);
}

void AKFGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isUntouchable)
		timeOfUntouchable += DeltaTime;
	
	if (timeOfUntouchable >= timeUntouchable)
    {
		isUntouchable = false;
        GetMesh()->SetScalarParameterValueOnMaterials("PercentForBasicColor", 1.0f);
        GetMesh()->SetScalarParameterValueOnMaterials("PercentForRed", 0.0f);
		timeOfUntouchable = 0.0f;
    }	

	if(timeStun > 0)
		timeStun -= DeltaTime;

	if(GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Z < 0)
		GetCharacterMovement()->Velocity.Z =  FMath::Lerp(GetCharacterMovement()->Velocity.Z,fallSpeed,fallSpeedRise);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKFGPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
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
	if ((Controller != NULL) && (Value != 0.0f) && !(timeOfUntouchable <= timeAnimationHit && isUntouchable) && (timeStun <= 0))
	{
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
	if ( (Controller != NULL) && (Value != 0.0f) && !(timeOfUntouchable <= timeAnimationHit && isUntouchable) && (timeStun <= 0))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AKFGPlayer::PlayerDamage(int damage)
{
	if(*currentLife > 0 && !isUntouchable)
	{
		*currentLife -= damage;
		isUntouchable = true;
		UGameplayStatics::PlaySound2D(GetWorld(), hitsound);
		GetMesh()->SetScalarParameterValueOnMaterials("PercentForBasicColor", 0.5f);
		GetMesh()->SetScalarParameterValueOnMaterials("PercentForRed", 0.9f);
	}
}


void AKFGPlayer::PlayerStun(float _timeStun)
{
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(cameraShakeStun, 1.0f);
	timeStun = _timeStun;
}
