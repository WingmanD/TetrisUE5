#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ATetrisPlayerState.generated.h"

UCLASS()
class TETRIS_API ATetrisPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void SetIsGameOver(bool value)
	{
		_isGameOver = value;
	}

	[[nodiscard]] bool GetIsGameOver() const
	{
		return _isGameOver;
	}

private:
	bool _isGameOver = false;
};
