#include "MapRoomConnector.h"

#include "MapRoom.h"

void UMapRoomConnector::DrawDebug(const int32 RoomColumn, const int32 RoomRow)
{
	FColor Color;
	switch (Direction)
	{
	case EMapConnectorDir::North:
		Color = FColor::Red;
		break;
		
	case EMapConnectorDir::East:
		Color = FColor::Green;
		break;
		
	case EMapConnectorDir::South:
		Color = FColor::Blue;
		break;
		
	case EMapConnectorDir::West:
		Color = FColor::Yellow;
		break;
		
	default:
		Color = FColor::White;
		break;
	}

	const FVector StartLocation = GetComponentLocation() + FVector::UpVector * 12.f;
	const FVector ForwardDirection = GetForwardVector().GetSafeNormal();
	const FVector RightDirection = FVector::CrossProduct(ForwardDirection, FVector::UpVector).GetSafeNormal();
	const FVector EndLocation = StartLocation + ForwardDirection * 150.f;

	// Arrow body
	DrawDebugSphere(GetWorld(), StartLocation, 6.f, 8, Color, true);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, Color, true, -1.f, 0, 3.f);

	const FVector HeadLeft  = EndLocation - ForwardDirection * 20.f - RightDirection * 6.f;
	const FVector HeadRight = EndLocation - ForwardDirection * 20.f + RightDirection * 6.f;
	// Arrow head
	DrawDebugLine(GetWorld(), EndLocation, HeadLeft,  Color, true, -1.f, 0, 3.f);
	DrawDebugLine(GetWorld(), EndLocation, HeadRight, Color, true, -1.f, 0, 3.f);
	
	// Room name. Add a small offset to separate if from arrow head
	const FVector LabelLocation = EndLocation + ForwardDirection * 20.f + FVector::UpVector * 20.f;
	FString DebugString = FString::Printf(TEXT("%s | %d-%d"), *GetName(), RoomRow,  RoomColumn);
	DrawDebugString(GetWorld(), LabelLocation, DebugString, nullptr, Color, -1.f, true);
}
