#include "MapGraphGenerator.h"

#include "MapGraph.h"
#include "ARPG_Map/Map/Layout/MapLayout.h"

void AMapGraphGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	// TODO: Load layout config from data asset
	LayoutConfig.Layout = EMapLayout::U;
	LayoutConfig.SegmentMaxLength = 3;
	LayoutConfig.SegmentMinLength = 2;
	LayoutConfig.Themes.Add("Plain");

	Rows = 5;
	Columns = 5;
	CachedMapGraph.Resize(Rows, Columns);
	
	BuildMapGraph();
	
	MapUtils::PrintGraph(CachedMapGraph, GetWorld());
}

void AMapGraphGenerator::BuildMapGraph()
{
	GenerateLayout();

	if (CachedLayout.IsValid())
	{
		PlaceMainPath();	
		PlaceBranches();
	}
}

void AMapGraphGenerator::GenerateLayout()
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

void AMapGraphGenerator::PlaceMainPath()
{			
	for (const FMapSegment& Segment : CachedLayout.MainPath.Segments)
		PlaceSegment(Segment);
	
	// Tag the first and last node of the main path
	CachedMapGraph.At(CachedLayout.MainPath.Start).Role = EMapRole::MainPathStart;
	CachedMapGraph.At(CachedLayout.MainPath.GetCells().Last()).Role = EMapRole::MainPathEnd;
}

void AMapGraphGenerator::PlaceBranches()
{
	for (const FBranchRule& BranchRule : CachedLayout.BranchRules)
		PlaceBranchesForRule(BranchRule);
}

void AMapGraphGenerator::PlaceBranchesForRule(const FBranchRule& BranchRule)
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

void AMapGraphGenerator::PlaceSegment(const FMapSegment& Segment)
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

void AMapGraphGenerator::PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell)
{
	CachedMapGraph.At(Coord) = Cell;
}

void AMapGraphGenerator::ConnectCells(const FMapGraphCoord FirstCell, const FMapGraphCoord SecondCell)
{
	if (!MapUtils::IsInsideBounds(Rows, Columns, FirstCell) || !MapUtils::IsInsideBounds(Rows, Columns, SecondCell))
		return;
	
	CachedMapGraph.At(FirstCell).Connectors.Add(MapUtils::GetDirectionToward(FirstCell, SecondCell));
	CachedMapGraph.At(SecondCell).Connectors.Add(MapUtils::GetDirectionToward(SecondCell, FirstCell));
}