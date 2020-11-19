// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"

#include "CoreMinimal.h"

#include "Sound/SoundCue.h"


#include "KFGPlayer.generated.h"

enum class EPlayerState {NONE, ATTACK, ROLL, SPECIAL, PARADE};

UCLASS(config=Game)
class AKFGPlayer : public ACharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AKFGPlayer();

	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* hitAnim;
	
	/** Called for forwards/backward input */
	virtual void MoveForward(float Value);

	/** Called for side to side input */
	virtual void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UPROPERTY(BlueprintReadOnly)
	float timeOfUntouchable = 0.0f;

	float timeAnimationHit;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//// CONTENT

	//// UNTOUCHABLE
	UPROPERTY(EditAnywhere)
	float timeUntouchable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isUntouchable = false;
	////

	//// PARADE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Parade)
	float timeParade;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Parade)
	float strengthPushBack;
	void BeginParade();
	float timeCurrentParade = 0.0f;
	////
	
	UFUNCTION(BlueprintCallable)
	void PlayerDamage(int damage);
	UFUNCTION(BlueprintCallable)
	void PlayerStun(float _timeStun);

	//// Player Stat
	int* currentLife = nullptr; // Point on currentLife KFGGameMode
	float* deamonResources = nullptr; // Point on deamonResources in KFGGameMode
	float* deamonResourcesSpeedDecrease = nullptr; // Point on deamonResourcesSpeedDecrease in KFGGameMode
	float* deamonResourcesSpeedIncrease = nullptr; // Point on deamonResourcesSpeedIncrease in KFGGameMode
	UPROPERTY(BlueprintReadOnly)
	float timeStun = 0; // if = 0 not stun else stun
	////

	//// Movement
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Movement)
	float fallSpeed;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Movement)
	float fallSpeedRise;
	////
	
	UPROPERTY(EditAnywhere)
	USoundCue* hitsound;
	
	EPlayerState playerState = EPlayerState::NONE;

	virtual void Tick(float DeltaTime) override;

};