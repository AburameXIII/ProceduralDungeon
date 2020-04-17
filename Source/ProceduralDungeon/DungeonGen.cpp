// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGen.h"

UDungeonGen::UDungeonGen()
{
	PrimaryComponentTick.bCanEverTick = false;

	RoomTries = 10;
	ExtraConnectorChance = 2;
	RoomMinSize = 3;
	RoomMaxSize = 5;
	Width = 30;
	Height = 20;
	RoomPadding = 3;

	WindingPercentage = 100;
	
	HorizontalRooms = 3;
	VerticalRooms =  2;
	NumberOfRooms = 3;
	Deadends = false;
}




// Called when the game starts
void UDungeonGen::BeginPlay()
{
	Super::BeginPlay();
	

	RoomTries = 10;
	ExtraConnectorChance = 2;
	RoomMinSize = 3;
	RoomMaxSize = 5;
	Width = 30;
	Height = 20;
	RoomPadding = 3;

	WindingPercentage = 100;

	HorizontalRooms = 3;
	VerticalRooms = 2;
	NumberOfRooms = 3;
	Deadends = false;

	RoomType = ERoomType::Random;
	PathType = EPathType::Maze;
	
}


// Called every frame
void UDungeonGen::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

//Initializes the Dungeon Generator Parameters
void UDungeonGen::Initialize() {
	DungeonArchitecture.Empty();
	Rooms.Empty();
	CurrentRegion = 0;
	LastDirection = nullptr;
}


TArray<FTile> UDungeonGen::Generate() {
	Initialize();

	//ADD ROOMS
	switch (RoomType) {
		case ERoomType::Random:
			AddRooms();
			break;
		case ERoomType::Structured:
			AddRoomsMD();
			break;
	}


	//PATHS
	switch (PathType) {
		case EPathType::Maze:
			//fill all empty spaces with mazes
			for (int j = 1; j < Height - 1; j += 2) {
				for (int i = 1; i < Width - 1; i += 2) {

					if (DungeonArchitecture[j * Width + i].Type != ETileType::Wall) continue;
					GrowMaze(i, j);
				}
			}


			//connect regions
			ConnectRegions();
			break;
		case EPathType::RightAndDown:
			ConnectRegionsMD();
			break;
	}
	
	//REMOVE DEADENDS
	if(!Deadends) RemoveDeadEnds();

	//DECORATE
	Decorate();

	return DungeonArchitecture;
}

//Adds rooms randomly
void UDungeonGen::AddRooms() {
	
	//fill dungeon with wall tiles
	for (int j = 0; j < Height; j++) {
		for (int i = 0; i < Width; i++) {
			DungeonArchitecture.Add(FTile{ ETileType::Wall, false, -1, FVector2D(i,j) });
		}
	}


	for (int i = 0; i < RoomTries; i++) {


		//Created random rooms given a minimum and max value.
		//This could perhaps be parameterized better and made sure that rooms agree to a subset of rules
		auto width = FMath::RandRange(RoomMinSize, RoomMaxSize);
		auto height = FMath::RandRange(RoomMinSize, RoomMaxSize);
		auto x = FMath::RandRange(1, Width - width - 1);
		auto y = FMath::RandRange(1, Height - height - 1);


		FRoom room{ x, y, width, height };

		bool overlaps = false;
		for (FRoom existingroom : Rooms) {
			if (room.overlaps(existingroom, RoomPadding)) {
				overlaps = true;
				break;
			}
		}

		if(overlaps) continue;

		Rooms.Add(room);

		//CURRENT REGION COUNT
		CurrentRegion++;

		//CARVE THE DUNGEON
		CarveRoom(room);
	}
	
}

//Carves unless its a RoomWall, return false if it is
//This can be changed in the future and merged with ForceCarve
bool UDungeonGen::Carve(FVector2D p, bool isRoom) {
	if (DungeonArchitecture[p.Y * Width + p.X].Type == ETileType::RoomWall) return false;
	DungeonArchitecture[p.Y * Width + p.X] = FTile{ ETileType::Empty, isRoom, CurrentRegion, p };
	return true;
}

//Force Carve, carves an empty tile on the map, not caring about regions or roomwalls, usually used when carving paths
void UDungeonGen::ForceCarve(FVector2D p) {
	DungeonArchitecture[p.Y * Width + p.X].Type = ETileType::Empty;
}

//Carves a room wall in the point p
void UDungeonGen::CarveRoomWall(FVector2D p) {
	DungeonArchitecture[p.Y * Width + p.X].Type = ETileType::RoomWall;
}

//Grows Maze starting in position (x, y)
void UDungeonGen::GrowMaze(int x, int y) {
	TArray<FVector2D> Points;
	

	CurrentRegion++;

	//CARVE WITH FLOOR
	Carve(FVector2D(x,y));

	Points.Add(FVector2D(x , y));
	while (Points.Num() > 0) {
		FVector2D p = Points.Last();

		TArray<FVector2D> PossibleDirections;
		for (FVector2D d : CARDINAL) {
			if (CanCarve(p, d)) PossibleDirections.Add(d);
		}

		if (PossibleDirections.Num() != 0) {
			FVector2D d;
			if (LastDirection != nullptr && PossibleDirections.Contains(*LastDirection) && FMath::RandRange(0, 100) < WindingPercentage) {
				d = *LastDirection;
			}
			else {
				int index = FMath::RandRange(0, PossibleDirections.Num() - 1);
				
				d = PossibleDirections[index];
			}

			
			//If I can carve 2 points ahead I carve it and add it as point of interest
			if (Carve(p + d * 2)) {
				Points.Add(p + d * 2);
			}
			else { //Else it means I can't continue this path, removing the current point
				Points.Pop();
			}
			//Makes sure to at least carve 1 point ahead
			Carve(p + d);
			LastDirection = &d;

		}
		else {
			Points.Pop();
			LastDirection = nullptr;
		}
	}

	
}


//Returns true if, given a certain point p and direction d, the position + 3 times the direction is within bounds and two directions ahead corresponds to a Wall or RoomWall
bool UDungeonGen::CanCarve(FVector2D p, FVector2D d) {
	if (p.X + d.X * 3 > Width - 1 ||
		p.X + d.X * 3 < 0 ||
		p.Y + d.Y * 3 > Height - 1 ||
		p.Y + d.Y * 3 < 0) return false;

	return DungeonArchitecture[(p.Y + d.Y * 2) * Width + (p.X + d.X * 2)].Type == ETileType::Wall
		|| DungeonArchitecture[(p.Y + d.Y * 2) * Width + (p.X + d.X * 2)].Type == ETileType::RoomWall;
}

//Connects rooms to paths
//Could be repurposed
void UDungeonGen::ConnectRegions() {
	
	//get all connecting points that can be openned that connect two or more regions
	TArray<FConnector*> ConnectorRegions;
	for (int i = 0; i < Width * Height; i++) {

		if (DungeonArchitecture[i].Type != ETileType::RoomWall) continue;

		TArray<int> DifferentRegions;
		bool RoomAndCorridor = false;
		for (FVector2D d : CARDINAL) {
			FVector2D p = DungeonArchitecture[i].Coordinates + d;
			if (p.X >= 0 && p.X < Width && p.Y >= 0 && p.Y < Height) {
				int region = DungeonArchitecture[p.Y * Width + p.X].Region;
				RoomAndCorridor |= DungeonArchitecture[p.Y * Width + p.X].Room;
				if (region != -1) DifferentRegions.Add(region);
			}
		}
		if (!RoomAndCorridor) continue;
		if (DifferentRegions.Num() < 2 ) continue;

		ConnectorRegions.Add(new FConnector{ DifferentRegions, DungeonArchitecture[i].Coordinates });
	}

	//Keeps Track of Openned and Connected Regions
	TArray<int> ConnectedRegions;
	TArray<int> OpenRegions;
	for (int i = 1; i <= CurrentRegion; i++) {
		OpenRegions.Add(i);
		ConnectedRegions.Add(i);
	}

	//Connects regions until there are no connectors left or no open regions
	while (OpenRegions.Num() > 1 && ConnectorRegions.Num() > 0) {

		
		

		//Select a random connector and open it, removing itself from the list along with adjacent connectors
		FConnector* c = ConnectorRegions[FMath::RandRange(0, ConnectorRegions.Num() - 1)];
		ForceCarve(c->Coordinates);
		ConnectorRegions.Remove(c);

		ConnectorRegions.RemoveAll([&](FConnector* cother) {
			return FVector2D::Distance(cother->Coordinates, c->Coordinates) <= 1;
		});


		//Remove a region from the connector list and put it in the ConnectedRegions array
		//All regions are connected once every number is the same
		//This Array starts by mapping its own region to itself, at position x it will have the value x
		//As regions get connected, at position x it will have the value xpto which corresponds to the region it was connected
		//When all regions are connected, this array has the same number at every position
		int dest = c->Regions.Pop();
		for (int d = 1; d <= CurrentRegion; d++) {
			if (c->Regions.Contains(d)) {
				ConnectedRegions[d-1] = dest;
			}

		}

		//Removes the sources from play
		for (int d = 0; d < c->Regions.Num(); d++) {
			OpenRegions.Remove(c->Regions[d]);
		}


		//Removes points with a lambda function
		ConnectorRegions.RemoveAll([&](FConnector* cother) {

			//Counting the number of regions still in the ConnectedRegions array, if the number is more than 2, it will consider this conenctor for a future iteration
			TArray<int> RegionsToConnect;
			for (int x = 0; x < cother->Regions.Num(); x++) {

				if (RegionsToConnect.Contains(ConnectedRegions[cother->Regions[x] - 1])) continue;
				RegionsToConnect.Add(ConnectedRegions[cother->Regions[x] - 1]);

			}
			if (RegionsToConnect.Num() > 1) return false;

			//If the point doesnt connnect more than 2 regions, it still has a chance of openning, discarding adjacent points if so
			if (FMath::RandRange(0, 100) < ExtraConnectorChance) {
				ForceCarve(cother->Coordinates); 
				
				ConnectorRegions.RemoveAll([&](FConnector* cotherother) {
					return FVector2D::Distance(cother->Coordinates, cotherother->Coordinates) <= 1;
					});
				
			}


			//Removing it at the end from the array (even if it was open by chance or not)
			return true;
		});


	}

	

	
}

//Removes deadends. Deadends are points where in all other 3 directions there is a wall
//The algorithm repeats until all deadends have been removed
void UDungeonGen::RemoveDeadEnds() {
	bool done = false;

	while (!done) {
		done = true;
		for (int j = 0; j < Height - 1; j ++) {
			for (int i = 0; i < Width - 1; i ++) {

				if (DungeonArchitecture[j * Width + i].Type == ETileType::Wall||
					DungeonArchitecture[j * Width + i].Type == ETileType::RoomWall
					) continue;
				int exits = 0;

				for (auto d : CARDINAL) {
					auto x = i + d.X;
					auto y = j + d.Y;
					if (InBetween(0,Width-1,x) && InBetween(0,Height-1,y) &&
						DungeonArchitecture[y * Width + x].Type != ETileType::Wall &&
						DungeonArchitecture[y * Width + x].Type != ETileType::RoomWall) exits++;
				}

				if (exits > 1) continue;
				done = false;
				DungeonArchitecture[j * Width + i].Type = ETileType::Wall;
			}
		}
	}
}

//Add rooms structures
void UDungeonGen::AddRoomsMD() {
	//In this algorithm we don't use a set width or height, we calculated based on the values below
	Width = RoomPadding * (HorizontalRooms + 1) + HorizontalRooms * RoomMaxSize;
	Height = RoomPadding * (VerticalRooms + 1) + VerticalRooms * RoomMaxSize;

	//Fill the board with tiles after calculating width and height
	for (int j = 0; j < Height; j++) {
		for (int i = 0; i < Width; i++) {
			DungeonArchitecture.Add(FTile{ ETileType::Wall, false, -1, FVector2D(i,j) });
		}
	}

	//Creates many different rooms in their respective structured zones
	TArray<FRoom> PossibleRooms;
	for (int i = 0; i < VerticalRooms; i++) {
		for (int j = 0; j < HorizontalRooms; j++) {
			
			int x = RoomPadding * (j + 1) + RoomMaxSize * j;
			int y = RoomPadding * (i + 1) + RoomMaxSize * i;
			auto width = FMath::RandRange(RoomMinSize, RoomMaxSize);
			auto height = FMath::RandRange(RoomMinSize, RoomMaxSize);
			FRoom room{ x, y, width, height, CurrentRegion };
			PossibleRooms.Add(room);
			CurrentRegion++;
		}
	}

	//Selects only a few of those rooms to carve based on a NumberOfRooms parameter
	int RoomCount = 0;
	CurrentRegion = 0;
	while (PossibleRooms.Num() > 0 && RoomCount < NumberOfRooms) { //RoomCount < NumberOfRooms
		CurrentRegion++;
		FRoom room = PossibleRooms[FMath::RandRange(0, PossibleRooms.Num() - 1)];
		Rooms.Add(room);
		CarveRoom(room);
		PossibleRooms.Remove(room);
		RoomCount++;
	}



}


//Connects structured rooms to eachother
void UDungeonGen::ConnectRegionsMD() {
	//For all structure room zones
	for (int x = 0; x < HorizontalRooms * VerticalRooms; x++) {

		//Check if there is a room in that zone, if not continues
		FRoom* Room = Rooms.FindByPredicate([&](const FRoom& room) {
			return room.region == x;
			});
		if (Room == nullptr) continue;
		
		//If there is, checks if that room can connect to the right or down
		bool CanGoRight = CanConnectRight(*Room);
		bool CanGoDown = CanConnectDown(*Room);

		//Then checks if there is a room in the structured zone right below it or next to it
		FRoom* RoomToLinkRight = Rooms.FindByPredicate([&](const FRoom& room) {
			return room.region == Room->region + 1;
			});
		FRoom* RoomToLinkDown = Rooms.FindByPredicate([&](const FRoom& room) {
			return room.region == Room->region + HorizontalRooms;
			});


		//If everything exists, rooms are connected given a certain chance
		//This can be changed and parameterized to give dungeons more tunnability
		if (CanGoRight && RoomToLinkRight && CanGoDown && RoomToLinkDown) {
			int random = FMath::RandRange(0, 100);
			if (random < 33) {
				Connect(*Room, *RoomToLinkRight);
			}
			else if (random < 66) {
				Connect(*Room, *RoomToLinkDown);
			}
			else {
				Connect(*Room, *RoomToLinkRight);
				Connect(*Room, *RoomToLinkDown);
			}
			continue;
		}

		//If not everything exists, we examine each case
		//If it can connect down and there is a room to connect down, connects down right away
		if (CanGoDown && RoomToLinkDown){
			Connect(*Room, *RoomToLinkDown);
		}

		//If it can connect right and there is a room to connect right, connects right right away
		if (CanGoRight && RoomToLinkRight) {
			Connect(*Room, *RoomToLinkRight);
		}

		//If it can connect down but there is no room to connect down and cant go right, we have to create a miniroom
		//This miniroom is a 1by1 room with the sole purpose of aiding in the construction of the different paths
		if (CanGoDown && !RoomToLinkDown && !CanGoRight) {
			//force create and connect down
			FRoom MiniRoomToLinkDown = CreateMiniRoom(Room->region + HorizontalRooms);
			
			CarveRoom(MiniRoomToLinkDown);
			Connect(*Room, MiniRoomToLinkDown);

			Rooms.Add(MiniRoomToLinkDown);
		}

		//If it can connect right but there is no room to connect right and cant go down, we have to create a miniroom on the right and connect to it
		if (CanGoRight && !RoomToLinkRight && !CanGoDown) {
			//force create and connect right
			FRoom MiniRoomToLinkRight = CreateMiniRoom(Room->region + 1);
			
			CarveRoom(MiniRoomToLinkRight);
			Connect(*Room, MiniRoomToLinkRight);

			Rooms.Add(MiniRoomToLinkRight);
		}

		//If it can go both ways but there is no room on either side, it will have a chance of creating and connecting only down, right or both
		//Once again this could be changed and parametrized in order to make the dungeon more tunnable
		if (CanGoDown && !RoomToLinkDown && CanGoRight && !RoomToLinkRight) {
			int random = FMath::RandRange(0, 100);
			if (random < 33) {
				FRoom MiniRoomToLinkRight = CreateMiniRoom(Room->region + 1);
				
				CarveRoom(MiniRoomToLinkRight);
				Connect(*Room, MiniRoomToLinkRight);

				Rooms.Add(MiniRoomToLinkRight);
			}
			else if (random < 66) {
				FRoom MiniRoomToLinkDown = CreateMiniRoom(Room->region + HorizontalRooms);
				Rooms.Add(MiniRoomToLinkDown);
				CarveRoom(MiniRoomToLinkDown);
				Connect(*Room, MiniRoomToLinkDown);
			}
			else {
				FRoom MiniRoomToLinkRight = CreateMiniRoom(Room->region + 1);
				
				CarveRoom(MiniRoomToLinkRight);
				FRoom MiniRoomToLinkDown = CreateMiniRoom(Room->region + HorizontalRooms);
				
				CarveRoom(MiniRoomToLinkDown);
				Connect(*Room, MiniRoomToLinkRight);
				Connect(*Room, MiniRoomToLinkDown);

				Rooms.Add(MiniRoomToLinkRight);
				Rooms.Add(MiniRoomToLinkDown);
			}
		}
	}


}

//Checks if the region number is not one of the regions suppose to be on the right edge of the dungeon
bool UDungeonGen::CanConnectRight(FRoom room) {
	return room.region % HorizontalRooms != HorizontalRooms - 1;
}

//Checks if the region number is not one of the regions suppose to be on the bottom edge of the dungeon
bool UDungeonGen::CanConnectDown(FRoom room) {
	return room.region < HorizontalRooms * (VerticalRooms-1);
}

//Connects two rooms by selecting a random point from each and connecting them
void UDungeonGen::Connect(FRoom r1, FRoom r2) {
	FVector2D Point1 = GetRandomRoomPoint(r1);
	FVector2D Point2 = GetRandomRoomPoint(r2);
	ConnectTwoPoints(Point1, Point2);
}

//Created a 1 by 1 room with region number r
FRoom UDungeonGen::CreateMiniRoom(int r) {
	int j = r % HorizontalRooms;
	int i = FMath::RoundToInt(r / HorizontalRooms);
	
	int x = RoomPadding * (j+1) + RoomMaxSize * j + FMath::RandRange(0,RoomMaxSize);
	int y = RoomPadding * (i + 1) + RoomMaxSize * i + FMath::RandRange(0, RoomMaxSize);
	FRoom room{ x,y,1,1,r };
	return room;
}

//Carves a room, carving the borders with room walls and the insides with empty tiles
void UDungeonGen::CarveRoom(FRoom room) {
	for (int k = room.x - 1; k <= room.x + room.width; k++) {
		for (int j = room.y - 1; j <= room.y + room.height; j++) {

			if (k < room.x || k == room.x + room.width || j < room.y || j >= room.y + room.height) {
				CarveRoomWall(FVector2D(k, j));
				continue;
			}
			Carve(FVector2D(k, j), true);
		}
	}
}

//Given a certain Room r, it returns the coordinates of a point that exists inside that room
FVector2D UDungeonGen::GetRandomRoomPoint(FRoom r) {
	int x = FMath::RandRange(r.x, r.x + r.width - 1);
	int y = FMath::RandRange(r.y, r.y + r.height - 1);
	
	return FVector2D(x, y);
}

//Connects two points. It initializes the starting point as p1, and continues until it reaches p2.
void UDungeonGen::ConnectTwoPoints(FVector2D p1, FVector2D p2) {
	FVector2D CurrentPoint = p1;
	FVector2D DirectionToChoose = FVector2D::ZeroVector;

	while (CurrentPoint != p2) {
		
		//If the direction is zero, or if the winding percentage allows for a new calculation, or if the point in the current direction is no longer inside the rectangle defined by p1 and p2
		//then we calculate a new distance and select a new direction that get us closer to p2
		if (DirectionToChoose.IsZero() || FMath::RandRange(0, 100) > WindingPercentage || 
			!InBetween(p1.X,p2.X,(CurrentPoint+DirectionToChoose).X) || !InBetween(p1.Y, p2.Y, (CurrentPoint + DirectionToChoose).Y) ||
			CurrentPoint.X == p2.X || CurrentPoint.Y == p2.Y) {
			float distance = Width * Height;

			for (auto d : CARDINAL) {
				float calculatedDistance = FVector2D::Distance(p2, CurrentPoint + d);
				if (calculatedDistance < distance) {
					DirectionToChoose = d;
					distance = calculatedDistance;
				}
				/*
				//Removed because it would allow the creation of many diagonal paths. Could be parametrized and added in the future
				else if (calculatedDistance == distance) {
					if (FMath::FRandRange(0, 1)) {
						DirectionToChoose = d;
					}
				}*/
			}
		}

		//A problem with pointers was happening so I installed this break point in the loop so stuff wouldn't break.  Can be removed
		if (!InBetween(0, Width - 1, (CurrentPoint + DirectionToChoose).X) || !InBetween(0, Height - 1, (CurrentPoint + DirectionToChoose).Y)) {
			break;
		}
		CurrentPoint = CurrentPoint + DirectionToChoose;
		
		ForceCarve(CurrentPoint);
	}
}

//Decorates each room
void UDungeonGen::Decorate() {
	//Removes MiniRooms as possible rooms for decorating
	Rooms.RemoveAll([](const FRoom& room) {
		return room.width == 1 && room.height == 1;
	});

	//Chooses a random beginning point
	FVector2D Beginning = GetRandomRoomPoint(Rooms[FMath::RandRange(0, Rooms.Num() - 1)]);
	DungeonArchitecture[Beginning.Y * Width + Beginning.X].Type = ETileType::Beginning;

	//Chooses a random end point
	FVector2D End = GetRandomRoomPoint(Rooms[FMath::RandRange(0, Rooms.Num() - 1)]);
	while (End == Beginning) {
		End = GetRandomRoomPoint(Rooms[FMath::RandRange(0, Rooms.Num() - 1)]);
	}
	DungeonArchitecture[End.Y * Width + End.X].Type = ETileType::Exit;

	//Places Items
	for (int i = NumItems; i > 0; ) {
		FVector2D ItemPoint = GetRandomRoomPoint(Rooms[FMath::RandRange(0, Rooms.Num() - 1)]);
		if (DungeonArchitecture[ItemPoint.Y * Width + ItemPoint.X].Type == ETileType::Empty) {
			DungeonArchitecture[ItemPoint.Y * Width + ItemPoint.X].Type = ETileType::Object;
			i--;
		}
	}

	//Places Enemies
	for (int i = NumEnemies; i > 0; ) {
		FVector2D ItemPoint = GetRandomRoomPoint(Rooms[FMath::RandRange(0, Rooms.Num() - 1)]);
		if (DungeonArchitecture[ItemPoint.Y * Width + ItemPoint.X].Type == ETileType::Empty) {
			DungeonArchitecture[ItemPoint.Y * Width + ItemPoint.X].Type = ETileType::Enemy;
			i--;
		}
	}

}

//Function that let us know if between is in between x and y, inclusiv'e
bool  UDungeonGen::InBetween(int x, int y, int between) {
	if (x > y) return x >= between && between >= y;
	else return  x <= between && between <= y;
}