#include "MapRoomGenerator.h"

#include "MapRoom.h"
#include "MapRoomTemplate.h"
#include "Kismet/KismetMathLibrary.h"

void AMapRoomGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (RoomTemplates.IsEmpty())
		return;

	// List of open connectors waiting for attachment
	TArray<const UMapRoomConnector*> OpenConnectors;

	for (int32 Row = 0; Row < RowCount; ++Row)
	{
		for (int32 Column = 0; Column < ColumnCount; ++Column)
		{				
			UMapRoomTemplate* RandomRoomTemplate = RoomTemplates[FMath::RandRange(0, RoomTemplates.Num() - 1)];			
			AMapRoom* NewRoom = GetWorld()->SpawnActor<AMapRoom>(RandomRoomTemplate->RoomClass);

			if (NewRoom->Connectors.IsEmpty())
			{
				UE_LOG(LogTemp, Error, TEXT("AMapRoomGenerator: %s has no connectors"), *NewRoom->GetName());
				continue;
			}
			
			// If this is the first room, don't move the room and add his connectors to the list of open connectors.
			if (OpenConnectors.IsEmpty())
			{
				OpenConnectors.Append(NewRoom->Connectors);
				DrawRoomDebug(NewRoom, Row, Column);					
				continue;
			}
			
			const UMapRoomConnector* TargetConnector = OpenConnectors[0];				
			UMapRoomConnector* ConnectorToAttach = NewRoom->Connectors[FMath::RandRange(0, NewRoom->Connectors.Num() - 1)];
			AttachRoomToConnector(NewRoom, TargetConnector, ConnectorToAttach);

			UE_LOG(LogTemp, Display, TEXT("Room: %d-%d: Attach %s to %s at %s"), Row, Column, *ConnectorToAttach->GetName(),
				*TargetConnector->GetName(), *TargetConnector->GetComponentLocation().ToString());
			
			DrawRoomDebug(NewRoom, Row, Column);
			
			// Remove the connector used from the list
			OpenConnectors.RemoveAt(0);

			// Add open connectors to the list
			for (const UMapRoomConnector* Connector : NewRoom->Connectors)
			{
				if (Connector != ConnectorToAttach)
					OpenConnectors.Add(Connector);
			}
		}
	}
}

void AMapRoomGenerator::AttachRoomToConnector(AMapRoom* Room, const UMapRoomConnector* TargetConnector, const UMapRoomConnector* ConnectorToAttach)
{
	// Compute the world location of the new room so its connector is snapped to the target connector
	const FVector ConnectorToAttachOffset = FVector::Dist(ConnectorToAttach->GetComponentLocation(), Room->GetActorLocation()) * TargetConnector->GetForwardVector();
	const FVector NewRoomLocation = TargetConnector->GetComponentLocation() + ConnectorToAttachOffset;

	// Rotate the room so its connector looks toward the target connector, then flip 180° so connectors face each other
	FRotator RoomRotation = UKismetMathLibrary::FindLookAtRotation(ConnectorToAttach->GetComponentLocation(),TargetConnector->GetComponentLocation());
	RoomRotation.Yaw += 180.f;
			
	Room->SetActorLocationAndRotation(NewRoomLocation, RoomRotation);
}

void AMapRoomGenerator::DrawRoomDebug(const AMapRoom* Room, const int32 Row, const int32 Column) const
{
	const FString Label = FString::Printf(TEXT("%d-%d"), Row, Column);
	DrawDebugString(GetWorld(), Room->GetActorLocation(), Label, nullptr, FColor::White, -1.f, true);

	for (UMapRoomConnector* Connector : Room->Connectors)
		Connector->DrawDebug(Column, Row);
}