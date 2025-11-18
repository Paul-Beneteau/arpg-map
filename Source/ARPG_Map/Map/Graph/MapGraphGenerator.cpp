#include "MapGraphGenerator.h"

#include "MapGraph.h"
#include "ARPG_Map/Map/Layout/MapLayout.h"

FMapGraph UMapGraphGenerator::BuildMapGraph()
{
	CachedMapGraph = FMapGraph(Rows, Columns);
	
	const FMapLayoutConfig LayoutConfig = PickLayoutConfig();
	if (!LayoutConfig.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Couldn't select a valid layout from LayoutConfigTable"));
		return FMapGraph();
	}
	
	GenerateLayout(LayoutConfig);

	if (!CachedLayout.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Couldn't generate a valid layout"));
		return FMapGraph();
	}

	PlaceMainPath();	
	PlaceBranches();

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
		UE_LOG(LogTemp, Error, TEXT("LayoutConfigTable is empty"));
		return FMapLayoutConfig();
	}
    
	float TotalWeight = 0.f;
	for (const FMapLayoutConfig* LayoutConfig : LayoutConfigs)
		TotalWeight += LayoutConfig->Weight;
    
	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;
    
	for (const FMapLayoutConfig* Config : LayoutConfigs)
	{
		CurrentWeight += Config->Weight;
		if (RandomWeight <= CurrentWeight)
			return *Config;
	}
    
	return FMapLayoutConfig();	
}

void UMapGraphGenerator::GenerateLayout(const FMapLayoutConfig& LayoutConfig)
{
	for (int32 RetryCount = 0; RetryCount < MaxLayoutGenerationRetries; RetryCount++)
	{
		CachedLayout = FMapLayoutGenerator::GenerateLayout(LayoutConfig, Rows, Columns);
		if (CachedLayout.IsValid())
			break;
		
		UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Layout generation failed (attempt %d/%d)"), RetryCount + 1, MaxLayoutGenerationRetries);
		if (RetryCount == MaxLayoutGenerationRetries - 1)
			UE_LOG(LogTemp, Error, TEXT("AMapGraphGenerator: Failed to generate layout after %d attempts"), RetryCount + 1);
	} 
}

void UMapGraphGenerator::PlaceMainPath()
{			
	for (const FMapSegment& Segment : CachedLayout.MainPath.Segments)
		PlaceSegment(Segment);
	
	// Tag the first and last node of the main path
	CachedMapGraph.At(CachedLayout.MainPath.Start).Role = EMapRole::MainPathStart;
	CachedMapGraph.At(CachedLayout.MainPath.GetCells().Last()).Role = EMapRole::MainPathEnd;
}

void UMapGraphGenerator::PlaceBranches()
{
	for (const FBranchRule& BranchRule : CachedLayout.BranchRules)
		PlaceBranchesForRule(BranchRule);
}

void UMapGraphGenerator::PlaceBranchesForRule(const FBranchRule& BranchRule)
{	
	for (int32 Index = 0; Index < CachedLayout.MainPath.GetCells().Num(); Index += BranchRule.StepInterval)
	{		
		const FMapGraphCoord CurrentCell = CachedLayout.MainPath.GetCells()[Index];		
		const FMapSegment CurrentSegment = CachedLayout.MainPath.GetSegmentAt(CurrentCell);		
		const EMapDirection BranchDirection = MapUtils::GetTurnDirection(CurrentSegment.Direction, BranchRule.Turn);
		
		FMapSegment Branch {
			// Branch start one cell after the path cell
			.Start = CurrentCell.Stepped(BranchDirection, 1),
			.Direction = BranchDirection,
			.Sections = BranchRule.Sections
		};			
		
		PlaceSegment(Branch);
	}
}

void UMapGraphGenerator::PlaceSegment(const FMapSegment& Segment)
{	
	for (FMapGraphCoord CurrentCell : Segment.GetCells())
	{
		// Cell should be inside the graph and not already used
		if (!MapUtils::IsInsideBounds(Rows, Columns, CurrentCell) || CachedMapGraph.At(CurrentCell).IsUsed())
			return;		

		PlaceCell(CurrentCell, FMapGraphCell(Segment.GetSection(CurrentCell).Role, Segment.GetSection(CurrentCell).Theme));

		const FMapGraphCoord PreviousCell = CurrentCell.Stepped(Segment.Direction, -1);				
		ConnectCells(CurrentCell, PreviousCell);	
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