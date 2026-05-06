// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGP_2526Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MGP_2526.h"

AMGP_2526Character::AMGP_2526Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


	// Create camera boom (spring arm)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());

	CameraBoom->SocketOffset = FVector(73.f, 81.f, 85.f);

	CameraBoom->TargetArmLength = 300.f;

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;



	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}


void AMGP_2526Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

		// Looking
		// EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

		EnhancedInputComponent->BindAction(RightMouseHeldAction,ETriggerEvent::Started,this,&AMGP_2526Character::OnAimStarted);

		EnhancedInputComponent->BindAction(RightMouseHeldAction,ETriggerEvent::Completed,this,&AMGP_2526Character::OnAimEnded);

		EnhancedInputComponent->BindAction(FireAction,ETriggerEvent::Started,this,&AMGP_2526Character::OnFire);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMGP_2526Character::TryDash);



		
	}
	else
	{
		UE_LOG(LogMGP_2526, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMGP_2526Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	LastMovementInput = MovementVector; // storing for dash

	float Scale = 1.0f;

	if (bIsAimingNow)
	{
		if (GetCharacterMovement()->IsFalling()) {} // keep air momentum and air steering while aiming
		
		else
		{
			return; // still block ground movement while aiming 
		}
	}

	// route the input
	DoMove(MovementVector.X * Scale, MovementVector.Y * Scale);
}

void AMGP_2526Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMGP_2526Character::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMGP_2526Character::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMGP_2526Character::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMGP_2526Character::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AMGP_2526Character::Tick(float DeltaTime) // i did this after a while oif looking for solutions for why my character was looking right. it was an animation problem, not a code one. too lazy to change everything back since this works
{
	Super::Tick(DeltaTime);

	if (Controller)
	{
		{
			FRotator ControlRot = Controller->GetControlRotation();
			ControlRot.Pitch = 0.f;
			ControlRot.Roll = 0.f;

			SetActorRotation(ControlRot);
		}
	}
	
	bInLeftRightDodgeZone = false;

	TArray<UPrimitiveComponent*> OverlappingComponents;
	GetOverlappingComponents(OverlappingComponents);


	for (UPrimitiveComponent* Comp : OverlappingComponents)
	{
		if (!Comp) continue;

		if (UBoxComponent* Box = Cast<UBoxComponent>(Comp))
		{
			if (Box->ComponentHasTag(FName("LeftRightDodgeZone")))
			{
				bInLeftRightDodgeZone = true;
				break;
			}
		}
	}

	//if (bInLeftRightDodgeZone==false) UE_LOG(LogTemp, Warning, TEXT("Dodge miss"));
	//if (bInLeftRightDodgeZone == true) UE_LOG(LogTemp, Warning, TEXT("Dodge"));

}


void AMGP_2526Character::OnAimStarted()
{

	bIsAimingNow = true;
	CameraBoom->TargetArmLength = 150.0f;

	ReticleWidget->SetVisibility(ESlateVisibility::Visible); // show reticle when aiming

}

void AMGP_2526Character::OnAimEnded()
{
	bIsAimingNow = false;
	CameraBoom->TargetArmLength = 300.f;
	ReticleWidget->SetVisibility(ESlateVisibility::Hidden); // remove reticle when not aiming. or well, make it invisible

}


void AMGP_2526Character::OnFire()
{

	if (!bIsAimingNow) return;

	UWorld* World = GetWorld();

	// Get camera position + direction
	FVector CameraLocation;
	FRotator CameraRotation;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);


	// Trace forward from camera
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * 10000.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // bullets kept clipping barrel end when i got too precise

	bool bHit = World->LineTraceSingleByChannel(Hit,CameraLocation,TraceEnd,ECC_Visibility,Params);
	FVector TargetPoint = bHit ? Hit.ImpactPoint : TraceEnd;

	// Spawn slightly in front of character
	// FVector SpawnLocation = GetActorLocation() + CameraRotation.Vector() * 100.f; // should replace it with a socket on gus end
	FVector SpawnLocation = GetMesh()->GetChildComponent(0)->GetSocketLocation(TEXT("Muzzle")); // bullet spawns at muzzle with this


	// Rotate projectile toward target
	FVector Direction = (TargetPoint - SpawnLocation).GetSafeNormal();
	FRotator SpawnRotation = Direction.Rotation();

	// Spawn projectile
	World->SpawnActor<AActor>(ProjectileClass,SpawnLocation,SpawnRotation);
}
	
void AMGP_2526Character::BeginPlay()
{
	Super::BeginPlay();

	if (ReticleWidgetClass)
	{
		ReticleWidget = CreateWidget<UUserWidget>(GetWorld(), ReticleWidgetClass);

		if (ReticleWidget)
		{
			ReticleWidget->AddToViewport();
			ReticleWidget->SetRenderScale(FVector2D(0.05f, 0.05f)); // cause it was huge

			ReticleWidget->SetVisibility(ESlateVisibility::Hidden); // stay hiden when not aiming

		}
	}
}

void AMGP_2526Character::TryDash()
{
	if (!bCanDash || !GetCharacterMovement() || !bIsAimingNow) return; // make it so dash can't hapopen if dash cooldown isn't up and you'r not aiming

	FVector DashDirection = FVector::ZeroVector;

	DashLeft = false;
	DashRight = false; // stop overlaps

	// A = -1, D = +1 
	if (LastMovementInput.X < 0.f)
	{
		DashDirection = -GetActorRightVector();
		DashLeft = true;
	}
	else if (LastMovementInput.X > 0.f)
	{
		DashDirection = GetActorRightVector();
		DashRight = true;
	}
	else
	{
		return; 
	}

	if (TimeMultiplier==1) // i only want the perfect dodge slowMo to happen when outside the sloMo
	{

		if (bInLeftRightDodgeZone == true && (DashRight == true || DashLeft == true)) // check if the player is in the left/right dodge zone and dodges left or right
		{
			UE_LOG(LogTemp, Warning, TEXT("Perfect Dodged!"));
			TimeMultiplier = slowTimeMultiplier;




		}






		if (TimeMultiplier != 1){
			GetWorldTimerManager().SetTimer(PerfectDodgeTimerHandle,[this](){TimeMultiplier = 1;},3.0f,false); // add a timed turn off for the 
		}

	}








	LaunchCharacter(DashDirection * DashStrength, true, true);

	bCanDash = false;

	// reset dash after 0.5s
	GetWorldTimerManager().SetTimer(DashResetHandle,[this](){DashLeft = false;DashRight = false;},0.4f,false);

	// cooldown
	GetWorldTimerManager().SetTimer(DashCooldownHandle,[this](){bCanDash = true;},0.4f,false);
}

