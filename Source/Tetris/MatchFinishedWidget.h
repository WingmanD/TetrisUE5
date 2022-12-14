#pragma once

#include "CoreMinimal.h"
#include "TetrisPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MatchFinishedWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TETRIS_API UMatchFinishedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetData(const ATetrisPlayerController* winnerController) const;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* _text;

	UPROPERTY(meta = (BindWidget))
	UButton* _mainMenuButton;

private:
	UFUNCTION()
	void GoToMainMenu();
};
