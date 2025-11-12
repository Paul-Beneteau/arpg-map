#pragma once

#include "CoreMinimal.h"
#include "MapTypes.generated.h"

UENUM(BlueprintType)
enum class EMapDirection : uint8
{
	None,
	North,
	East,
	South,
	West
};

UENUM(BlueprintType)
enum class EMapRole : uint8
{
	None,
	MainPathStart,
	MainPath,
	MainPathEnd,
	Wall
};

USTRUCT(BlueprintType)
struct FMapGraphCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Column = 0;

	FMapGraphCoord() = default;
	
	FMapGraphCoord(int32 InRow, int32 InColumn) : Row(InRow), Column(InColumn)	{}

	bool operator==(const FMapGraphCoord& Other) const { return Row == Other.Row && Column == Other.Column; }

	void Step(EMapDirection Direction, int32 Distance);
	FMapGraphCoord Stepped(EMapDirection Direction, int32 Distance);
};