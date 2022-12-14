#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TetrisPlayerController.generated.h"

UCLASS()
class TETRIS_API ATetrisPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATetrisPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
