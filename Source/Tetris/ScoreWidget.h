#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

class UTextBlock;

UCLASS()
class TETRIS_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateScore(int32 Score) const;
	
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* _scoreText;
};
