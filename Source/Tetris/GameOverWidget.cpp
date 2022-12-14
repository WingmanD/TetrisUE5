#include "GameOverWidget.h"
#include "Components/TextBlock.h"

void UGameOverWidget::SetScore(int32 score) const
{
	if (_text != nullptr)
	{
		_text->SetText(FText::FromString("Game Over\nYour Score: " + FString::FromInt(score)));
	}
}
