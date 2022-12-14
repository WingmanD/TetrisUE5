#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TetrisHUD.generated.h"

class ATetrisPlayerController;
class UMatchFinishedWidget;
class UGameOverWidget;
class UScoreWidget;

UCLASS()
class TETRIS_API ATetrisHUD : public AHUD
{
	GENERATED_BODY()

public:

	void UpdateScore(int32 score) const;
	void ShowGameOver(float getScore);
	void ShowMatchFinished(const ATetrisPlayerController* winnerController);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	UScoreWidget* _scoreWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UScoreWidget> _scoreWidgetClass;

	UPROPERTY()
	UGameOverWidget* _gameOverWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UGameOverWidget> _gameOverWidgetClass;

	UPROPERTY()
	UMatchFinishedWidget* _matchFinishedWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMatchFinishedWidget> _matchFinishedWidgetClass;
};
