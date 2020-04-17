// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon.h"

// Sets default values
ADungeon::ADungeon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Initializes actor component
	DunGen = CreateDefaultSubobject<UDungeonGen>(TEXT("DungeonGenerator"));

}

// Called when the game starts or when spawned
void ADungeon::BeginPlay()
{
	Super::BeginPlay();
	
	ShowDungeon();
	
}

//Generates a new dungeon and shows it
void ADungeon::ShowDungeon() {
	TArray<FTile> Tiles = DunGen->Generate();

	for (FTile t : Tiles) {
		if (t.Type != ETileType::Empty) {
			FVector Location = FVector(100.f * t.Coordinates.Y, 100.f * t.Coordinates.X, 0.f);
			ABlock* RoomBlock = GetWorld()->SpawnActor<ABlock>(ABlock::StaticClass(), Location, FRotator::ZeroRotator);
			DungeonArchitecture.Add(RoomBlock);
			switch (t.Type) {
				case(ETileType::RoomWall):
					RoomBlock->SetRoomWall();
					break;
				case(ETileType::Beginning):
					RoomBlock->SetBeginning();
					break;
				case(ETileType::Exit):
					RoomBlock->SetExit();
					break;
				case(ETileType::Object):
					RoomBlock->SetItem();
					break;
				case(ETileType::Enemy):
					RoomBlock->SetEnemy();
					break;
			}
		}
	}
}

// Called every frame
void ADungeon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADungeon::Generate() {
	
	//Initializes the parameters
	for (auto d : DungeonArchitecture) {
		d->Destroy();
	}
	DungeonArchitecture.Empty();

	ShowDungeon();
}