#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"


UCLASS()
class TETRIS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* _startSinglePlayerButton;

	UPROPERTY(meta = (BindWidget))
	UButton* _start2PlayerButton;

	UPROPERTY(meta = (BindWidget))
	UButton* _quitButton;

private:
	UFUNCTION()
	void StartSinglePlayer();

	UFUNCTION()
	void Start2Player();

	UFUNCTION()
	void QuitGame();
};
