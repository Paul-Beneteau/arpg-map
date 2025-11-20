#include "MapGraphGenerator.h"

#include "MapGraph.h"
#include "ARPG_Map/Map/Layout/MapLayout.h"

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
	Constraints.Bounds = FMapGraphCoord(Rows, Columns);
    
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

void UMapGraphGenerator::AddBranchesToPath(const TArray<FMapSegment>& MainPath, const TArray<FMapBranchConfig>& BranchConfigs)
{
	for (const FMapBranchConfig& BranchConfig : BranchConfigs)
		AddBranchesForConfig(MainPath, BranchConfig);
}

void UMapGraphGenerator::AddBranchesForConfig(const TArray<FMapSegment>& MainPath, const FMapBranchConfig& BranchConfig)
{
	for (const FMapSegment& Segment : MainPath)
	{
		for (int32 Index = 0; Index < Segment.Length; Index += BranchConfig.StepInterval)
		{
			FMapPathConstraints Constraints;
			Constraints.StartDirection = MapUtils::RotateClockwise(Segment.Direction, BranchConfig.RotationFromMainPath);
			Constraints.Start = Segment.GetCoordAt(Index).Stepped(Constraints.StartDirection, 1);
			Constraints.Bounds = FMapGraphCoord(Rows, Columns);
    
			TArray<FMapSegment> Branch = GenerateAndPlacePath(BranchConfig.PathConfig, Constraints);
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
			PlacePath(Path);
			return Path;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Path generation failed (attempt %d/%d)"), RetryCount + 1, MaxLayoutGenerationRetries);
		if (RetryCount == MaxLayoutGenerationRetries - 1)
			UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Failed to generate path after %d attempts"), RetryCount + 1);
	}
	
	return TArray<FMapSegment>();
}

void UMapGraphGenerator::PlacePath(const TArray<FMapSegment>& Path)
{
	for (const FMapSegment& Segment : Path)
		PlaceSegment(Segment);
}

void UMapGraphGenerator::PlaceSegment(const FMapSegment& Segment)
{
	for (int32 Index = 0; Index < Segment.Length; Index++)
	{
		const FMapGraphCoord CurrentCoord = Segment.GetCoordAt(Index);
		
		// Cell should be inside the graph and not already used
		if (!MapUtils::IsInsideBounds(Rows, Columns, CurrentCoord) || CachedMapGraph.At(CurrentCoord).IsUsed())
			return;		

		FMapGraphCell Cell;
		Cell.Type = Segment.Type;
		Cell.Theme = Segment.Theme,
		Cell.FlowDirection = Segment.Direction;		
		PlaceCell(CurrentCoord, Cell);

		const FMapGraphCoord PreviousCoord = CurrentCoord.Stepped(Segment.Direction, -1);		
		if (MapUtils::IsInsideBounds(Rows, Columns, PreviousCoord))
			ConnectCells(CurrentCoord, PreviousCoord);
	}
}

void UMapGraphGenerator::PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell)
{
	CachedMapGraph.At(Coord) = Cell;
}

void UMapGraphGenerator::ConnectCells(const FMapGraphCoord FirstCell, const FMapGraphCoord SecondCell)
{
	if (!MapUtils::IsInsideBounds(Rows, Columns, FirstCell) || !MapUtils::IsInsideBounds(Rows, Columns, SecondCell))
		return;
	
	CachedMapGraph.At(FirstCell).Connectors.Add(MapUtils::GetDirectionToward(FirstCell, SecondCell));
	CachedMapGraph.At(SecondCell).Connectors.Add(MapUtils::GetDirectionToward(SecondCell, FirstCell));
}