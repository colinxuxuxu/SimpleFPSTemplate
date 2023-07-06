// Fill out your copyright notice in the Description page of Project Settings.

// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
#include "FPSBombActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
//when have a new class, create a blueprint in UE editor for the code to spawn
AFPSBombActor::AFPSBombActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ExplodeDelay = 2.0f;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//collision will ignore the player
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	RootComponent = MeshComp;

}

void AFPSBombActor::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionTemplate, GetActorLocation());

	//Creating the query to allow configurations
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionShape CollShape;
	CollShape.SetSphere(500.0f);

	TArray<FOverlapResult> OutOverlaps;
	GetWorld()->OverlapMultiByObjectType(OutOverlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	//loop over the TArray of overlapped items to see which should change color
	for (FOverlapResult Result : OutOverlaps)
	{
		UPrimitiveComponent* Overlap = Result.GetComponent();
		if (Overlap && Overlap->IsSimulatingPhysics())
		{
			UMaterialInstanceDynamic* MatInst = Overlap->CreateAndSetMaterialInstanceDynamic(0);
			if (MatInst)
			{
				MatInst->SetVectorParameterValue("Color", TargetColor);
			}
		}
	}

	Destroy();
}

// Called when the game starts or when spawned
void AFPSBombActor::BeginPlay()
{
	Super::BeginPlay();

	//TimerHandle allows easy access later on, the first parameter
	FTimerHandle Explode_TimerHandle;
	GetWorldTimerManager().SetTimer(Explode_TimerHandle, this, &AFPSBombActor::Explode, ExplodeDelay, false);

	MaterialInstance = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	if (MaterialInstance)
	{
		//fetch from the material to interpolate from this color to another
		CurrentColor = MaterialInstance->K2_GetVectorParameterValue("Color");
	}
	TargetColor = FLinearColor::MakeRandomColor();

}

// Called every frame
void AFPSBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MaterialInstance)
	{
		//progress = time alive
		float progress = (GetWorld()->TimeSeconds - CreationTime) / ExplodeDelay;

		FLinearColor NewColor = FLinearColor::LerpUsingHSV(CurrentColor, TargetColor, progress);
		MaterialInstance->SetVectorParameterValue("Color", NewColor);
	}
}

