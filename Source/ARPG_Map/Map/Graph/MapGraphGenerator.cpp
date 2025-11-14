#include "MapGraphGenerator.h"

#include "MapGraph.h"
#include "MapGraphUtils.h"

void AMapGraphGenerator::BeginPlay()
{
	Super::BeginPlay();

	CachedMapGraph.Resize(Rows, Columns);

	// TODO: Load layout config from data asset
	LayoutConfig.Layout = EMapLayout::U;
	LayoutConfig.SegmentMaxLength = 3;
	LayoutConfig.SegmentMinLength = 2;
	LayoutConfig.Themes.Add("Plain");
	
	BuildMapGraph();
	
	MapUtils::PrintGraph(CachedMapGraph, GetWorld());
}

void AMapGraphGenerator::BuildMapGraph()
{
	for (int32 RetryCount = 1; RetryCount < MaxRetry; RetryCount++)
	{
		CachedLayout = FMapLayoutGenerator::GenerateLayout(LayoutConfig, Rows, Columns);
		if (CachedLayout.IsValid())
			break;
		
		UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Failed to generate layout L - Retry: %d"), RetryCount);		
	} 

	GenerateMainPath();
	
	for (const FBranchRule& BranchRule : CachedLayout.BranchRules)
		GenerateBranchesFromMainPath(BranchRule);
}

bool AMapGraphGenerator::GenerateLayoutL()
{
	CachedLayout.Reset();	
	GenerateMainPathStart();
	
	FMapSegment FirstSegment = GenerateFirstMainPathSegment();
	if (!FirstSegment.IsValid())
		return false;
	
	CachedLayout.MainPath.Segments.Add(FirstSegment);
	FMapGraphCoord Anchor = CachedLayout.MainPath.Start.Stepped(FirstSegment.Direction, FirstSegment.GetLength() - 1);

	FMapSegment SecondSegment = GenerateMainPathSegment(Anchor, MapUtils::Perpendicular(FirstSegment.Direction));
	if (!SecondSegment.IsValid())
		return false;
	
	CachedLayout.MainPath.Segments.Add(SecondSegment);	
	Anchor = Anchor.Stepped(SecondSegment.Direction, SecondSegment.GetLength());

	FMapSegment ThirdSegment = GenerateMainPathSegment(Anchor, { MapUtils::Opposite(FirstSegment.Direction) }, FirstSegment.GetLength() - 1);
	if (!ThirdSegment.IsValid())
		return false;
	
	CachedLayout.MainPath.Segments.Add(ThirdSegment);
	
	FBranchRule BranchRule { EMapTurn::Right, 2, 1.f };
	BranchRule.Sections.Add(FMapSegmentSection(1, EMapRole::None, "Plain"));

	CachedLayout.BranchRules.Add(BranchRule);

	return true;
}

void AMapGraphGenerator::GenerateMainPathStart()
{
	switch (const int32 Edge = FMath::RandRange(0, 3))
	{
		// Top Edge
	case 0:
		CachedLayout.MainPath.Start = {0,FMath::RandRange(0, Columns - 1)};
		break;
		
		// Bottom Edge
	case 1:
		CachedLayout.MainPath.Start = {Rows - 1,FMath::RandRange(0, Columns - 1)};
		break;
		
		// Left Edge
	case 2:
		CachedLayout.MainPath.Start = {FMath::RandRange(0, Rows - 1), 0};
		break;
		
		// Right Edge
	case 3:
		CachedLayout.MainPath.Start = {FMath::RandRange(0, Rows - 1), Columns - 1};
		break;
		
	default:
		CachedLayout.MainPath.Start = { -1, -1 };
		break;
	}
}

FMapSegment AMapGraphGenerator::GenerateFirstMainPathSegment()
{
	TArray<EMapDirection> PossibleDirections;

	FMapSegment Segment;

	Segment.Start = CachedLayout.MainPath.Start;
	
	if (CachedLayout.MainPath.Start.Row == 0)
		Segment.Direction = EMapDirection::South;
	if (CachedLayout.MainPath.Start.Row == Rows - 1)
		Segment.Direction =EMapDirection::North;
	if (CachedLayout.MainPath.Start.Column == 0)
		Segment.Direction =EMapDirection::East;
	if (CachedLayout.MainPath.Start.Column == Columns - 1)
		Segment.Direction =EMapDirection::West;
	
	FMapSegmentSection SegmentSection;
	SegmentSection.Length = FMath::RandRange(LayoutConfig.SegmentMinLength, LayoutConfig.SegmentMaxLength);
	SegmentSection.Role = EMapRole::MainPath;
	SegmentSection.Theme = LayoutConfig.Themes[FMath::RandRange(0, LayoutConfig.Themes.Num() - 1)];
	
	Segment.Sections.Add(SegmentSection);

	//UE_LOG(LogTemp, Warning, TEXT("Generated first layout segment %d-%d | Direction: %d | Length: %d:"),
		//Segment.Start.Row, Segment.Start.Column, Segment.Direction, Segment.GetLength());
	
	return Segment;
}

FMapSegment AMapGraphGenerator::GenerateMainPathSegment(FMapGraphCoord Anchor, const TArray<EMapDirection>& PossibleDirections, int32 Length /* = -1 */)
{
	FMapSegment Segment;

	FMapSegmentSection SegmentSection;
		
	SegmentSection.Role = EMapRole::MainPath;
	SegmentSection.Theme = LayoutConfig.Themes[FMath::RandRange(0, LayoutConfig.Themes.Num() - 1)];

	if (Length > 0)
		SegmentSection.Length = Length;
	else
		SegmentSection.Length = FMath::RandRange(LayoutConfig.SegmentMinLength, LayoutConfig.SegmentMaxLength);
	
	Segment.Sections.Add(SegmentSection);

	TArray<EMapDirection> ValidDirections = GetValidSegmentDirections(Segment, Anchor, PossibleDirections);
	
	if (ValidDirections.IsEmpty())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed to generate layout segment %d-%d | Direction: %d | Length: %d:"),
			//Anchor.Stepped(Segment.Direction, 1).Row, Anchor.Stepped(Segment.Direction, 1).Column, Segment.Direction, Segment.GetLength());
		return FMapSegment();
	}
		
	Segment.Direction = ValidDirections[FMath::RandRange(0, ValidDirections.Num() - 1)];

	Segment.Start = Anchor.Stepped(Segment.Direction, 1);
	
	//UE_LOG(LogTemp, Warning, TEXT("Generated layout segment %d-%d | Direction: %d | Length: %d:"),
		//Segment.Start.Row, Segment.Start.Column, Segment.Direction, Segment.GetLength());
	
	return Segment;
}

void AMapGraphGenerator::GenerateMainPath()
{		
	// Anchor starts one cell before the segment
	FMapGraphCoord Anchor = CachedLayout.MainPath.Start.Stepped(CachedLayout.MainPath.Segments[0].Direction, -1);
	
	for (const FMapSegment& Segment : CachedLayout.MainPath.Segments)
	{
		if (!GenerateSegment(Segment))
			return;
		
		Anchor = Anchor.Stepped(Segment.Direction, Segment.GetLength());
	}

	// Tag the first and last node of the main path
	CachedMapGraph.At(CachedLayout.MainPath.Start).Role = EMapRole::MainPathStart;
	CachedMapGraph.At(Anchor).Role = EMapRole::MainPathEnd;
}

void AMapGraphGenerator::GenerateBranchesFromMainPath(const FBranchRule& BranchRule)
{	
	for (int32 Index = 0; Index < CachedLayout.MainPath.GetCells().Num(); Index += BranchRule.StepInterval)
	{		
		const FMapGraphCoord CurrentCell { CachedLayout.MainPath.GetCells()[Index] };		
		const FMapSegment CurrentSegment { CachedLayout.MainPath.GetSegmentAt(CurrentCell) };		
		const EMapDirection BranchDirection { MapUtils::GetTurnDirection(CurrentSegment.Direction, BranchRule.Turn) };
		
		FMapSegment Branch {
			// Branch start one cell after the path cell
			.Start = CurrentCell.Stepped(BranchDirection, 1),
			.Direction = BranchDirection,
			.Sections = BranchRule.Sections
		};			
		
		GenerateSegment(Branch);
	}
}

bool AMapGraphGenerator::GenerateSegment(const FMapSegment& Segment)
{
	FMapGraphCoord CurrentCell { Segment.Start };
	
	for (const FMapSegmentSection& Section : Segment.Sections)
	{
		for (int32 Offset = 0; Offset < Section.Length; ++Offset)
		{			
			if (!CachedMapGraph.IsValidCoord(CurrentCell))
			{
				UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Segment is out of bounds"));
				return false;
			}			

			if (CachedMapGraph.At(CurrentCell).IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Segment to generate is on an existing graph node"));
				return false;
			}			

			PlaceCell(CurrentCell, FMapGraphCell(Section.Role, Section.Theme));

			// If the placed cell is the main path start, do not link with the previous cell as it is the anchor outside the graph
			if (CurrentCell != CachedLayout.MainPath.Start)
			{
				const FMapGraphCoord PreviousCell = CurrentCell.Stepped(Segment.Direction, -1);				
				LinkCells(CurrentCell, PreviousCell);
			}

			CurrentCell = CurrentCell.Stepped(Segment.Direction, 1);			
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Generating segment %d-%d | Direction: %d | Length: %d:"),
		//Segment.Start.Row,Segment.Start.Column, Segment.Direction, Segment.GetLength());

	return true;
}

void AMapGraphGenerator::PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell)
{
	CachedMapGraph.At(Coord) = Cell;
}

void AMapGraphGenerator::LinkCells(const FMapGraphCoord FirstCell, const FMapGraphCoord SecondCell)
{
	if (!CachedMapGraph.IsValidCoord(FirstCell) || !CachedMapGraph.IsValidCoord(SecondCell))
	{
		UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: linking cells with invalid coordinates!"));
		return;
	}
	
	CachedMapGraph.At(FirstCell).Connectors.Add(MapUtils::GetDirectionToward(FirstCell, SecondCell));
	CachedMapGraph.At(SecondCell).Connectors.Add(MapUtils::GetDirectionToward(SecondCell, FirstCell));
}

TArray<EMapDirection> AMapGraphGenerator::GetValidSegmentDirections(const FMapSegment& Segment, const FMapGraphCoord Anchor,
	const TArray<EMapDirection>& PossibleDirections /* = TArray<EMapDirection>({ EMapDirection::North, EMapDirection::East, EMapDirection::South, EMapDirection::West }) */)
{
	TArray<EMapDirection> ValidDirections;

	for (EMapDirection Direction : PossibleDirections)
	{
		// If the segment is inside the graph
		if (CachedMapGraph.IsValidCoord(Anchor.Stepped(Direction, Segment.GetLength())))
			ValidDirections.Add(Direction);		
	}

	if (ValidDirections.IsEmpty())
		return TArray<EMapDirection>();

	return ValidDirections;
}