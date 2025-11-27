#include "MapGraphGenerator.h"

#include "IContentBrowserSingleton.h"
#include "IPropertyTable.h"
#include "MapGraph.h"
#include "ARPG_Map/Map/Layout/MapLayout.h"
#include "ARPG_Map/Map/Tile/MapTileTemplate.h"

FMapGraph UMapGraphGenerator::GenerateMapGraph()
{
	CachedMapGraph = FMapGraph(Rows, Columns);
	
	const FMapLayoutConfig LayoutConfig = PickLayoutConfig();
	if (!LayoutConfig.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Couldn't select a valid layout from LayoutConfigTable"));
		return FMapGraph();
	}
	
	TArray<FMapSegment> MainPath = CreateMainPath(LayoutConfig.MainPathConfig);
	if (MainPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Failed to generate map graph"));
		return FMapGraph();
	}
	
	AddBranchesToPath(MainPath, LayoutConfig.BranchConfigs);

	if (LayoutConfig.FillTheme.IsSet())
		FillEmptyCells(LayoutConfig);		
		
	return CachedMapGraph;
}

FMapLayoutConfig UMapGraphGenerator::PickLayoutConfig()
{
	if (!LayoutConfigTable)
	{
		UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: No LayoutConfigTable assigned"));
		return FMapLayoutConfig();
	}

	TArray<FMapLayoutConfig*> LayoutConfigs;
	LayoutConfigTable->GetAllRows<FMapLayoutConfig>(TEXT("LayoutConfigs"), LayoutConfigs);
    
	if (LayoutConfigs.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UMapGraphGenerator: LayoutConfigTable is empty"));
		return FMapLayoutConfig();
	}

	auto GetConfigWeight = [](const FMapLayoutConfig* Config) { return Config->Weight; };
	FMapLayoutConfig* LayoutConfig = MapUtils::PickWeightedRandom<FMapLayoutConfig>(LayoutConfigs, GetConfigWeight);
    
	return *LayoutConfig;	
}

TArray<FMapSegment> UMapGraphGenerator::CreateMainPath(const FMapPathConfig& PathConfig)
{
	FMapPathConstraints Constraints;
	Constraints.Start = GenerateMainPathStart();
	Constraints.StartDirection = MapUtils::GetInwardDirection(Constraints.Start, Rows, Columns);
	Constraints.Bounds = FMapGraphCoord(Rows - 1, Columns - 1);
	Constraints.IsCellUsed = [this](const FMapGraphCoord& Coord) { return CachedMapGraph.At(Coord).IsUsed(); };
	
	TArray<FMapSegment> MainPath = GenerateAndPlacePath(PathConfig, Constraints);

	if (!MainPath.IsEmpty())
	{
		CachedMapGraph.MainPathStart = MainPath[0].Start;
		CachedMapGraph.MainPathEnd = MainPath.Last().End();
	}

	return MainPath;
}

FMapGraphCoord UMapGraphGenerator::GenerateMainPathStart() const
{
	switch (const int32 Edge = FMath::RandRange(0, 3))
	{
	// Top Edge
	case 0:
		return FMapGraphCoord{0,FMath::RandRange(0, Columns - 1)};
		
	// Bottom Edge
	case 1:
		return FMapGraphCoord{Rows - 1,FMath::RandRange(0, Columns - 1)};

	// Left Edge
	case 2:
		return FMapGraphCoord{FMath::RandRange(0, Rows - 1), 0};
		
	// Right Edge
	case 3:
		return FMapGraphCoord{FMath::RandRange(0, Rows - 1), Columns - 1};
		
	default:
		return FMapGraphCoord::None;
	}
}

void UMapGraphGenerator::AddBranchesToPath(const TArray<FMapSegment>& Path, const TArray<FMapBranchConfig>& BranchConfigs)
{
	for (const FMapBranchConfig& BranchConfig : BranchConfigs)
		AddBranchesForConfig(Path, BranchConfig);
}

namespace 
{
	bool IsCellStraightPath(const FMapGraphCell& Cell)
	{	
		// If the current cell has exactly 2 opposite path connectors.
		if (Cell.Connectors.Num() == 2			
			&& Cell.Connectors[0].Type == EMapConnectorType::Path
			&& Cell.Connectors[1].Type == EMapConnectorType::Path
			&& Cell.Connectors[0].Direction == MapUtils::Opposite(Cell.Connectors[1].Direction) == false)
			return true;
	
		return false;
	}
}

void UMapGraphGenerator::AddBranchesForConfig(const TArray<FMapSegment>& Path, const FMapBranchConfig& BranchConfig)
{
	int32 StepsSinceLastBranch = 0;
	
	for (const FMapSegment& Segment : Path)
	{		
		for (int32 Index =  0; Index < Segment.Length; ++Index, ++StepsSinceLastBranch)
		{
			FMapGraphCoord CurrentMainPathCoord = Segment.GetCoordAt(Index);

			// Check if the branch should be generated. Steps between last branch must be above step interval config, cell must be a straight path
			// (This could be removed, it's a style preference) and the random spawn probability is picked.
			if (StepsSinceLastBranch < BranchConfig.DistanceBetweenBranches				
				|| IsCellStraightPath(CachedMapGraph.At(CurrentMainPathCoord))
				|| FMath::FRand() > BranchConfig.SpawnProbability)
				continue;
			
			FMapPathConstraints Constraints;
			EMapRotation BranchRotation = MapUtils::GetRotation(BranchConfig.RotationFromMainPath);
			Constraints.StartDirection = MapUtils::RotateClockwise(Segment.Direction, BranchRotation);
			Constraints.Start = CurrentMainPathCoord.Stepped(Constraints.StartDirection, 1);
			Constraints.Bounds = FMapGraphCoord(Rows - 1, Columns - 1);
			Constraints.IsCellUsed = [this](const FMapGraphCoord& Coord) { return CachedMapGraph.At(Coord).IsUsed(); };
				
			TArray<FMapSegment> Branch = GenerateAndPlacePath(BranchConfig.PathConfig, Constraints);			
			if (!Branch.IsEmpty())
				StepsSinceLastBranch = 0;	
		}
	}
}

void UMapGraphGenerator::FillEmptyCells(const FMapLayoutConfig& LayoutConfig)
{
	for (int32 Row = 0; Row < Rows; Row++)
	{
		for (int32 Column = 0; Column < Columns; Column++)
		{
			FMapGraphCoord CurrentCoord = FMapGraphCoord(Row, Column);
			
			if (CachedMapGraph.At(CurrentCoord).IsUsed())
				continue;
				
			FMapGraphCell Cell;
			Cell.Theme = LayoutConfig.FillTheme.Get(NAME_None);

			CachedMapGraph.At(CurrentCoord) = Cell;			
		}
	}
}

TArray<FMapSegment> UMapGraphGenerator::GenerateAndPlacePath(const FMapPathConfig& PathConfig, const FMapPathConstraints& PathConstraints)
{	
	for (int32 RetryCount = 0; RetryCount < MaxLayoutGenerationRetries; RetryCount++)
	{
		TArray<FMapSegment> Path = FMapPathGenerator::GeneratePath(PathConfig, PathConstraints);
		if (!Path.IsEmpty())
		{
			PlacePath(Path, PathConfig.ConnectorsConfig);
			return Path;
		}
	}	
	
	return TArray<FMapSegment>();
}

void UMapGraphGenerator::PlacePath(const TArray<FMapSegment>& Path, const FMapConnectorsConfig& ConnectorsConfig)
{
	for (const FMapSegment& Segment : Path)
		PlaceSegment(Segment, ConnectorsConfig);
}

void UMapGraphGenerator::PlaceSegment(const FMapSegment& Segment, const FMapConnectorsConfig& ConnectorsConfig)
{
	for (int32 Index = 0; Index < Segment.Length; Index++)
	{
		const FMapGraphCoord CurrentCoord = Segment.GetCoordAt(Index);
		
		// Cell should be inside the graph and not already used. This case should never happen because FMapPathGenerator generates only valid path
		if (!MapUtils::IsInsideBounds(Rows, Columns, CurrentCoord) || CachedMapGraph.At(CurrentCoord).IsUsed())
		{
			UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Segment to place is invalid | Segment start: %d-%d"), Segment.Start.Row, Segment.Start.Column); 
			return;
		}

		FMapGraphCell Cell;
		Cell.Theme = Segment.Theme,
		PlaceCell(CurrentCoord, Cell);

		const FMapGraphCoord PreviousCoord = CurrentCoord.Stepped(Segment.Direction, -1);		
		if (MapUtils::IsInsideBounds(Rows, Columns, PreviousCoord))
			ConnectCells(CurrentCoord, PreviousCoord, ConnectorsConfig);
	}
}

void UMapGraphGenerator::PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell)
{
	CachedMapGraph.At(Coord) = Cell;
}

void UMapGraphGenerator::ConnectCells(const FMapGraphCoord FirstCellCoord, const FMapGraphCoord SecondCellCoord,  const FMapConnectorsConfig& ConnectorsConfig)
{
	if (!MapUtils::IsInsideBounds(Rows, Columns, FirstCellCoord) || !MapUtils::IsInsideBounds(Rows, Columns, SecondCellCoord))
		return;

	FMapConnector FirstCellConnector;
	FirstCellConnector.Direction = MapUtils::GetDirectionToward(FirstCellCoord, SecondCellCoord);
	FirstCellConnector.Type = ConnectorsConfig.Type;
	FirstCellConnector.Theme = ConnectorsConfig.Theme;	
	CachedMapGraph.At(FirstCellCoord).Connectors.Add(FirstCellConnector);

	FMapConnector SecondCellConnector;
	SecondCellConnector.Direction = MapUtils::GetDirectionToward(SecondCellCoord, FirstCellCoord);
	SecondCellConnector.Type = ConnectorsConfig.Type;
	SecondCellConnector.Theme = ConnectorsConfig.Theme;	
	CachedMapGraph.At(SecondCellCoord).Connectors.Add(SecondCellConnector);
}