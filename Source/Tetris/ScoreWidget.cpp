#include "ScoreWidget.h"
#include "Components/TextBlock.h"

void UScoreWidget::UpdateScore(int32 Score) const
{
	if (_scoreText != nullptr)
	{
		_scoreText->SetText(FText::FromString("Score: " + FString::FromInt(Score)));
	}
}
