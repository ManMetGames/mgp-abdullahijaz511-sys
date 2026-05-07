// Fill out your copyright notice in the Description page of Project Settings.


#include "TurrentScript.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

// Sets default values
ATurrentScript::ATurrentScript()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // originally used it but decided not to since it turns out i don't "need" it (not in the turrent script anyways)

}

// Called when the game starts or when spawned
void ATurrentScript::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(FireTimerHandle,this,&ATurrentScript::Fire,FireInterval,true); // start the firing

}

void ATurrentScript::Fire()
{
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 300.0f; // where i want the bullet to spawn

	FRotator SpawnRotation = GetActorRotation(); 

	FActorSpawnParameters SpawnParams; 
	// spawn the bullet from the parameters i just got from above
	AActor* Bullet = GetWorld()->SpawnActor<AActor>(BulletClass,SpawnLocation,SpawnRotation,SpawnParams);


	UProjectileMovementComponent* ProjectileComp = Bullet->FindComponentByClass<UProjectileMovementComponent>();

	ProjectileComp->Velocity = GetActorForwardVector() * BulletSpeed;

	ProjectileComp->InitialSpeed = BulletSpeed;
	ProjectileComp->MaxSpeed = BulletSpeed;



}


















// Called every frame
void ATurrentScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

