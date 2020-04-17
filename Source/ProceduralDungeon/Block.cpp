// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Initializes the mesh
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TileAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));

	if (TileAsset.Succeeded()) {
		TileMesh->SetStaticMesh(TileAsset.Object);
	}

	//Initializes the materials
	static ConstructorHelpers::FObjectFinder<UMaterial> RoomWallMaterialR(TEXT("Material'/Game/RoomWallMaterial.RoomWallMaterial'"));
	RoomWallMaterial = RoomWallMaterialR.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> BeginningMaterialR(TEXT("Material'/Game/BeginningMaterial.BeginningMaterial'"));
	BeginningMaterial = BeginningMaterialR.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> ExitMaterialR(TEXT("Material'/Game/ExitMaterial.ExitMaterial'"));
	ExitMaterial = ExitMaterialR.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> EnemyMaterialR(TEXT("Material'/Game/EnemyMaterial.EnemyMaterial'"));
	EnemyMaterial = EnemyMaterialR.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> ItemMaterialR(TEXT("Material'/Game/ItemMaterial.ItemMaterial'"));
	ItemMaterial = ItemMaterialR.Object;

}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlock::SetRoomWall() {
	TileMesh->SetMaterial(0, RoomWallMaterial);
}

void ABlock::SetBeginning() {
	TileMesh->SetMaterial(0, BeginningMaterial);
}


void ABlock::SetExit() {
	TileMesh->SetMaterial(0, ExitMaterial);
}

void ABlock::SetItem() {
	TileMesh->SetMaterial(0, ItemMaterial);
}

void ABlock::SetEnemy() {
	TileMesh->SetMaterial(0, EnemyMaterial);
}

