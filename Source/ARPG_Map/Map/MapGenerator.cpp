#include "MapGenerator.h"

#include "Graph/MapGraphGenerator.h"
#include "Room/MapTile.h"
#include "Room/MapTileSelector.h"

AMapGenerator::AMapGenerator()
{
	GraphGenerator = CreateDefaultSubobject<UMapGraphGenerator>("GraphGenerator");
	TileSelector = CreateDefaultSubobject<UMapTileSelector>("TileSelector");
}

void AMapGenerator::GenerateMap()
{
	CachedMapGraph = GraphGenerator->BuildMapGraph();
	
	TArray<FMapTileSpawnData> TilesToSpawn = TileSelector->SelectMapGraphTiles(CachedMapGraph);
	
	for (const FMapTileSpawnData& TileSpawnData : TilesToSpawn)
	{
		AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(
			TileSpawnData.TileClass,
			TileSpawnData.GetWorldLocation(TileSize),
			FRotator(0, 0, 0));

		if (Tile)
		{
			SpawnedTiles.Add(Tile);
			UE_LOG(LogTemp, Warning, TEXT("Spawned Tile at: %f|%f"), TileSpawnData.GetWorldLocation(TileSize).X, TileSpawnData.GetWorldLocation(TileSize).Y);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Tile at: %f|%f"), TileSpawnData.GetWorldLocation(TileSize).X, TileSpawnData.GetWorldLocation(TileSize).Y);
		}
	}
	
	MovePlayerToStart(CachedMapGraph);
}

void AMapGenerator::ClearMap()
{
	for (AMapTile* Tile : SpawnedTiles)
	{
		if (Tile)
			Tile->Destroy();
	}
    
	SpawnedTiles.Empty();
	CachedMapGraph.Reset();
}

FColor GetCellColor(const FMapGraphCell& Cell)
{
	if (!Cell.IsUsed())
		return FColor::Black;
            
	switch (Cell.Role)
	{
	case EMapRole::MainPathStart:
		return FColor::Green;
	case EMapRole::MainPathEnd:
		return FColor::Red;
	case EMapRole::MainPath:
		return FColor::Blue;
	default:
		return FColor::Turquoise;
	}
}

FVector GetConnectorOffset(EMapDirection Direction, float CellSize)
{
	const float Offset = CellSize * 0.9f / 2.0f;
        
	switch (Direction)
	{
	case EMapDirection::North:
		return FVector(Offset, 0, 0);
	case EMapDirection::South:
		return FVector(-Offset, 0, 0);
	case EMapDirection::West:
		return FVector(0, -Offset, 0);
	case EMapDirection::East:
		return FVector(0, Offset, 0);
	default:
		return FVector::ZeroVector;
	}
}
    
void DrawCell(const UWorld* World, const FVector& Location, const FMapGraphCell& Cell, float Size)
{
	const FColor Color = GetCellColor(Cell);
	const FVector HalfSize(Size * 0.99f / 2.0f, Size * 0.99f / 2.0f, 1.0f);
	DrawDebugBox(World, Location, HalfSize, Color, true, 0.f, 0, 10.f);
}
    
void DrawConnectors(const UWorld* World, const FVector& Location, const FMapGraphCell& Cell, float Size)
{
	for (EMapDirection Direction : Cell.Connectors)
	{
		const FVector Offset = GetConnectorOffset(Direction, Size);
		DrawDebugLine(World, Location + Offset, Location + (Offset * 1.2f), FColor::Purple, true, 0.f,
			0, 10.f);
	}
}

void AMapGenerator::ShowGraph() const
{
	for (int32 Row = 0; Row < CachedMapGraph.GetRows(); ++Row)
	{
		for (int32 Column = 0; Column < CachedMapGraph.GetColumns(); ++Column)
		{
			const FMapGraphCell Cell = CachedMapGraph.At(FMapGraphCoord(Row, Column));
				
			FVector Location((-Row) * TileSize, Column * TileSize, 10.0f);	
				
			DrawCell(GetWorld(), Location, Cell, TileSize);
            
			if (Cell.IsUsed())
				DrawConnectors(GetWorld(), Location, Cell, TileSize);
		}
	}	
}

void AMapGenerator::HideGraph() const
{
	FlushPersistentDebugLines(GetWorld());
}

void AMapGenerator::MovePlayerToStart(const FMapGraph& Graph)
{
	FMapGraphCoord StartCoord = Graph.GetStart();	
	FVector StartLocation(-StartCoord.Row * TileSize, StartCoord.Column * TileSize, 150);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* Pawn = PlayerController->GetPawn())
			Pawn->SetActorLocation(StartLocation);
	}
}