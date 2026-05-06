// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletsScript.h"
#include "MGP_2526Character.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ABulletsScript::ABulletsScript()
{

	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABulletsScript::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletsScript::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    TimeMult = Cast<AMGP_2526Character>(GetWorld()->GetFirstPlayerController()->GetPawn())->TimeMultiplier; // get's speed of bullet every tick

    // Check if multiplier changed
    if (TimeMult != PrevTimeMult)
    {
        UProjectileMovementComponent* ProjectileComp = FindComponentByClass<UProjectileMovementComponent>();

        if (ProjectileComp && PrevTimeMult != 0.0f)
        {
            // change current velocity
            float Scale = TimeMult / PrevTimeMult;

            ProjectileComp->Velocity *= Scale;
        }

        // Update previous multiplier
        PrevTimeMult = TimeMult;
    }

}


