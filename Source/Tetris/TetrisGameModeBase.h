#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TetrisGameModeBase.generated.h"

class ATetrisPlayerController;

UCLASS()
class TETRIS_API ATetrisGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATetrisGameModeBase();

	virtual void BeginPlay() override;

	virtual void StartPlay() override;

	void PlayerClearedRow(const ATetrisPlayerController* playerController, float speed) const;

	void PlayerGameOver(const ATetrisPlayerController* playerController) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float _scoreMultiplier = 100.0f;

private:
	void DeclareWinner() const;
};
