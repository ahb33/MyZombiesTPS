#pragma once

UENUM(BlueprintType)
enum class ECentralCommands : uint8
{
    MoveToLocation UMETA(DisplayName = "Move to Location"),
    AttackTarget UMETA(DisplayName = "Attack Target"),
    DefendArea UMETA(DisplayName = "Defend Area"),
    Retreat UMETA(DisplayName = "Retreat"),
    HoldPosition UMETA(DisplayName = "Hold Position")
};
