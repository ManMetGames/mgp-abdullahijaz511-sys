// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Logging/LogMacros.h"
#include "MGP_2526Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMGP_2526Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	virtual void Tick(float DeltaTime) override;

	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	// aim input
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RightMouseHeldAction;

	// fire input
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DashAction;

	FVector2D LastMovementInput = FVector2D::ZeroVector;


	UPROPERTY()
	APostProcessVolume* SlowMoVolume = nullptr; // kept crashing when i didn't set it to nullptr




	bool bCanDash = true; // cooldown for dash
	bool bInLeftRightDodgeZone = false;

	FTimerHandle DashResetHandle;
	FTimerHandle DashCooldownHandle; // timer ID's for later
	FTimerHandle PerfectDodgeTimerHandle;


	// Projectile class
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AActor> ProjectileClass;

	// Spawn offset
	UPROPERTY(EditAnywhere, Category = "Combat")
	FVector MuzzleOffset = FVector(100.f, 0.f, 50.f);

	// Fire function
	UFUNCTION()
	void OnFire();


	// crosshair
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> ReticleWidgetClass;

	UUserWidget* ReticleWidget;


public:

	/** Constructor */
	AMGP_2526Character();	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeMultiplier = 1.0;

	float slowTimeMultiplier = 0.2;


	float modifiedAirControl;
	float modifiedMaxWalkSpeed;
	float modifiedGetCharacterMovement;


	
	
	


protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();




	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashStrength = 3000.f;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
	bool bIsAimingNow = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
	bool DashLeft = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
	bool DashRight = false;

	UFUNCTION()
	void OnAimStarted();

	UFUNCTION()
	void OnAimEnded();

	UFUNCTION()
	void TryDash();

	

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	
};

