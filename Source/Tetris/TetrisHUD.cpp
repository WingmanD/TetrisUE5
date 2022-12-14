#include "TetrisHUD.h"

#include "GameOverWidget.h"
#include "MatchFinishedWidget.h"
#include "ScoreWidget.h"

void ATetrisHUD::UpdateScore(int32 score) const
{
	if (_scoreWidget != nullptr)
	{
		_scoreWidget->UpdateScore(score);
	}
}

void ATetrisHUD::ShowGameOver(float getScore)
{
	_gameOverWidget = CreateWidget<UGameOverWidget>(GetOwningPlayerController(), _gameOverWidgetClass);
	if (_gameOverWidget != nullptr)
	{
		_gameOverWidget->SetScore(getScore);
		_gameOverWidget->AddToPlayerScreen();
	}
}

void ATetrisHUD::ShowMatchFinished(const ATetrisPlayerController* winnerController)
{
	if (_gameOverWidget != nullptr)
	{
		_gameOverWidget->RemoveFromParent();
	}
	
	_matchFinishedWidget = CreateWidget<UMatchFinishedWidget>(GetOwningPlayerController(), _matchFinishedWidgetClass);
	if (_matchFinishedWidget != nullptr)
	{
		_matchFinishedWidget->SetData(winnerController);
		_matchFinishedWidget->AddToPlayerScreen();
	}

	if (APlayerController* playerController = GetOwningPlayerController())
	{
		playerController->bShowMouseCursor = true;
	}
}

void ATetrisHUD::BeginPlay()
{
	Super::BeginPlay();

	_scoreWidget = CreateWidget<UScoreWidget>(GetOwningPlayerController(), _scoreWidgetClass);
	if (_scoreWidget != nullptr)
	{
		_scoreWidget->AddToPlayerScreen();
	}
}
