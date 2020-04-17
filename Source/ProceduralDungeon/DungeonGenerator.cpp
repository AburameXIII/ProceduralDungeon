// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGenerator.h"

DungeonGenerator::DungeonGenerator()
{
	/*
	RoomTries = 3;
	ExtraConnectorChance = 1;
	RoomMinSize = 5;
	RoomMaxSize = 8;
	Width = 40;
	Height = 40;

	WindingPercentage = 40;
	*/

}

DungeonGenerator::~DungeonGenerator()
{
}


//TArray<Tile> DungeonGenerator::Generate() {
	/*
	

	//fill dungeon with wall tiles

	for (int j = 0; j < Height; j++) {
		for (int i = 0; i < Width; i++) {
			DungeonArchitecture[j * Height + i] = Tile{ TileType::Wall, 0, FVector2D(i,j) };
		}
	}

	//add rooms
	AddRooms();

	//fill all empty spaces with mazes
	for (int j = 0; j < Height; j++) {
		for (int i = 0; i < Width; i++) {
			if (DungeonArchitecture[j * Height + i].Type != TileType::Wall) continue;
			//GROWMAZE
		}
	}


	//connect regions

	//removedeadends

	//decorate each room
	
	return DungeonArchitecture;

	*/
//}


void DungeonGenerator::AddRooms() {
	/*

	for (int i = 0; i < RoomTries; i++) {

		//Created random rooms given a minimum and max value.
		//This could perhaps be parameterized better and made sure that rooms agree to a subset of rules
		auto width = FMath::RandRange(RoomMinSize, RoomMaxSize);
		auto height = FMath::RandRange(RoomMinSize, RoomMaxSize);
		auto x = FMath::RandRange(0, Width - width);
		auto y = FMath::RandRange(0, Height - height);

		Room room{ x, y, width, height };

		bool overlaps = false;
		for (Room existingroom : Rooms) {
			if (room.overlaps(existingroom)) {
				overlaps = true;
				break;
			}
		}

		if(overlaps) continue;

		Rooms.Add(room);

		//CURRENT REGION COUNT
		CurrentRegion++;

		//CARVE THE DUNGEON //CHANGE
		for (int i = room.x; i <= room.x + room.width; i++) {
			for (int j = room.y; j <= room.y + room.height; j++) {
				Carve(FVector2D(i, j));
			}
		}
	}

	*/
}

void DungeonGenerator::GrowMaze(int x, int y) {
	/*


	TArray<FVector2D> Points;


	CurrentRegion++;

	//CARVE WITH FLOOR
	Carve(FVector2D(x,y));

	Points.Add(FVector2D(x , y));
	while (!Points.Empty) {
		FVector2D p = Points.Last();

		TArray<FVector2D> PossibleDirections;
		for (FVector2D d : CARDINAL) {
			if (CanCarve(p, d)) PossibleDirections.Add(d);
		}

		if (PossibleDirections.Num != 0) {
			FVector2D d;
			if (PossibleDirections.Contains(*LastDirection) && FMath::RandRange(0, 100) > WindingPercentage) {
				d = *LastDirection;
			}
			else {
				d = PossibleDirections[FMath::RandRange(0, PossibleDirections.Num - 1)];
			}

			//CARVE WITH FLOOR
			Carve(p + d);
			Carve(p + d * 2);

			Points.Add(p + d * 2);
			LastDirection = &d;

		}
		else {
			Points.Pop();
			LastDirection = nullptr;
		}
	}

	*/
}

bool DungeonGenerator::CanCarve(FVector2D p, FVector2D d) {
	/*
	if (p.X + d.X * 3 > Width ||
		p.X + d.X * 3 < 0 ||
		p.Y + d.Y * 3 > Height ||
		p.Y + d.Y * 3 < 0) return false;

	return Dungeon->DungeonArchitecture[(p.Y + d.Y * 2) * Height + (p.X + d.X * 2)].Type == TileType::Wall;
	*/
	return false;
}

void DungeonGenerator::Carve(FVector2D p) {
	/*
	Dungeon->DungeonArchitecture[p.Y * Height + p.X] = Tile{ TileType::Empty, CurrentRegion, p };
	*/
}

void DungeonGenerator::ConnectRegions() {
	/*

	TArray<Connector> ConnectorRegions;
	for (int i = 0; i < Width * Height; i++) {

		if (Dungeon->DungeonArchitecture[i].Type != TileType::Wall) continue;

		TArray<int> DifferentRegions;
		for (FVector2D d : CARDINAL) {
			FVector2D p = Dungeon->DungeonArchitecture[i].Coordinates + d;
			int region = Dungeon->DungeonArchitecture[p.Y * Height + p.X].Region;
			if (region != -1) DifferentRegions.Add(region);
		}

		if (DifferentRegions.Num < 2) continue;

		ConnectorRegions.Add(Connector{ DifferentRegions, Dungeon->DungeonArchitecture[i].Coordinates });
	}

	TArray<int> OpenRegions;
	for (int i = 0; i < CurrentRegion; i++) {
		OpenRegions.Add(i);
	}

	while (OpenRegions.Num > 1) {
		Connector c = ConnectorRegions[FMath::RandRange(0, ConnectorRegions.Num - 1)];

		Carve(c.Coordinates); //PERHAPS A CARVE WITHOUT THE SET CURRENTREGION

		for (auto r : c.Regions) {
			OpenRegions.Remove(r);
		}

		for (auto c2 : ConnectorRegions) {
			if ((c.Coordinates - c2.Coordinates).Distance < 2) {
				ConnectorRegions.Remove(c2);
				continue;
			}
			
			bool RegionYetToOpen = false;
			for (auto r : c2.Regions) {
				RegionYetToOpen |= OpenRegions.Contains(r);
			}

			if (RegionYetToOpen) continue;


			if (FMath::RandRange(0, 100) > ExtraConnectorChance) {
					Carve(c2.Coordinates); //PERHAPS A CARVE WITHOUT THE SET CURRENTREGION
			}

			ConnectorRegions.Remove(c2);

		}
	}

	*/
}