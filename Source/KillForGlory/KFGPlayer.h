// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "KFGPlayer.generated.h"

enum class EPlayerState {NONE, ATTACK, ROLL };

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

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//// CONTENT
public:

	//// ANIM
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* combo1Anim;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* combo2Anim;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* combo3Anim;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* rollAnim;
	////

	//// PLAYER STAT
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
	int maxLife = 0;
	UPROPERTY(BlueprintReadOnly)
	int currentLife = 0;
	////
	
	////
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* attackHitBox;
	////

	//// EVENT
	void Roll(); //Input Roll Event

	UFUNCTION(BlueprintCallable)
	void RollStart();
	UFUNCTION(BlueprintCallable)
	void RollEnd();

	UFUNCTION(BlueprintCallable)
	void AttackLaunch();
	UFUNCTION(BlueprintCallable)
	void AttackReset();
	UFUNCTION(BlueprintCallable)
	void EnableAttackHitBox();
	UFUNCTION(BlueprintCallable)
	void DisableAttackHitBox();

	void Attack(); //Input Attack Event
	
	UFUNCTION()
    void OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    						int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	////

	bool isRolling = false;
	bool isAttacking = false;
	bool bufferAttack = false;
	UPROPERTY(BlueprintReadWrite)
	bool recoverAttack = false;
	int attackNum = 0;

	EPlayerState playerState = EPlayerState::NONE;

	void changePlayerState(EPlayerState newPlayerState);
};