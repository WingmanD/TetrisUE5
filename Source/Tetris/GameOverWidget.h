#pragma once

#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UTextBlock;

UCLASS()
class TETRIS_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetScore(int32 score) const;	
	
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* _text;
};
