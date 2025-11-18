#include "MapLayoutGenerator.h"

FMapLayout FMapLayoutGenerator::GenerateLayout(const FMapLayoutConfig& InConfig, const int32 InRows, const int32 InColumns)
{
	FMapLayoutGenerator LayoutGenerator(InConfig, InRows, InColumns);
	
	switch (InConfig.Layout)
	{
	case EMapLayout::L:
		return LayoutGenerator.GenerateLayoutL();

	case EMapLayout::U:
		return LayoutGenerator.GenerateLayoutU();
		
	default:
		return FMapLayout();
	}
}

FMapLayoutGenerator::FMapLayoutGenerator(const FMapLayoutConfig& InConfig, int32 InRows, int32 InColumns)
	: Config(InConfig), Rows(InRows), Columns(InColumns)
{
}

FMapLayout FMapLayoutGenerator::GenerateLayoutL()
{	
	return FMapLayout();
}

FMapLayout FMapLayoutGenerator::GenerateLayoutU()
{
	// U Shape is composed of 3 segments
	FMapLayout Layout;	
	GenerateStartCell(Layout);
	
	// First segment starts from an edge and goes inward with random length
	FMapGraphCoord Anchor = Layout.MainPath.Start;
	TArray<EMapDirection> PossibleDirections = { MapUtils::GetInwardDirection(Layout.MainPath.Start, Rows, Columns) };
	FMapSegment FirstSegment = GenerateSegment({ PossibleDirections }, Anchor);	
	if (!FirstSegment.IsValid())
		return FMapLayout();
	
	Layout.MainPath.Segments.Add(FirstSegment);

	// Second segment is perpendicular to the first, with random length
	Anchor = Anchor.Stepped(FirstSegment.Direction, FirstSegment.GetLength() - 1);
	PossibleDirections = { MapUtils::Perpendicular(FirstSegment.Direction) };	
	FMapSegment SecondSegment =	GenerateSegment(PossibleDirections, Anchor,1);	
	if (!SecondSegment.IsValid())
		return FMapLayout();
	
	Layout.MainPath.Segments.Add(SecondSegment);

	// Third segment has a fixed length of the first segment minus 1 to have a symmetrical shape, because the last cell
	// of the second segment adds 1 cell to the segment length visually
	Anchor = Anchor.Stepped(SecondSegment.Direction, SecondSegment.GetLength());
	PossibleDirections = { MapUtils::Opposite(FirstSegment.Direction) };
	FMapSegment ThirdSegment = GenerateSegment({ PossibleDirections }, Anchor,1,FirstSegment.GetLength() - 1);	
	if (!ThirdSegment.IsValid())
		return FMapLayout();

	Layout.MainPath.Segments.Add(ThirdSegment);
	
	Layout.BranchRules = Config.BranchRules;

	return Layout;
}

void FMapLayoutGenerator::GenerateStartCell(FMapLayout& Layout)
{
	switch (const int32 Edge = FMath::RandRange(0, 3))
	{
	// Top Edge
	case 0:
		Layout.MainPath.Start = {0,FMath::RandRange(0, Columns - 1)};
		break;
		
	// Bottom Edge
	case 1:
		Layout.MainPath.Start = {Rows - 1,FMath::RandRange(0, Columns - 1)};
		break;
		
	// Left Edge
	case 2:
		Layout.MainPath.Start = {FMath::RandRange(0, Rows - 1), 0};
		break;
		
	// Right Edge
	case 3:
		Layout.MainPath.Start = {FMath::RandRange(0, Rows - 1), Columns - 1};
		break;
		
	default:
		Layout.MainPath.Start = { -1, -1 };
		break;
	}
}

FMapSegment FMapLayoutGenerator::GenerateSegment(
	const TArray<EMapDirection>& PossibleDirections,
	const FMapGraphCoord Anchor,
	const int32 AnchorOffset /* = 0 */,
	const int32 Length /* = -1 */)
{
	// TODO: customize sections
	FMapSegmentSection Section;		
	Section.Role = EMapRole::MainPath;
	Section.Theme = Config.Themes[FMath::RandRange(0, Config.Themes.Num() - 1)];
	Section.Length = Length > 0 ? Length : FMath::RandRange(Config.SegmentMinLength, Config.SegmentMaxLength);

	TArray<EMapDirection> ValidDirections = FilterValidDirections(Anchor,Section.Length - 1 + AnchorOffset, PossibleDirections);
	
	if (ValidDirections.IsEmpty())
		return FMapSegment();

	EMapDirection Direction = ValidDirections[FMath::RandRange(0, ValidDirections.Num() - 1)];
	return FMapSegment
	{
		.Start = Anchor.Stepped(Direction, AnchorOffset),
		.Direction = Direction,
		.Sections = { Section }
	};
}

// TODO: check that segment does not cross used cells
TArray<EMapDirection> FMapLayoutGenerator::FilterValidDirections(
	const FMapGraphCoord SegmentStart,
	const int32 SegmentLength,
	const TArray<EMapDirection>& Directions) const
{
	TArray<EMapDirection> ValidDirections;

	for (EMapDirection Direction : Directions)
	{		
		if (MapUtils::IsInsideBounds(Rows, Columns, SegmentStart.Stepped(Direction, SegmentLength)))
			ValidDirections.Add(Direction);
	}

	return ValidDirections;
}