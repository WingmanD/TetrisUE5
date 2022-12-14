#include "MatchFinishedWidget.h"
#include "ATetrisPlayerState.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UMatchFinishedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (_mainMenuButton != nullptr)
	{
		_mainMenuButton->OnClicked.AddDynamic(this, &UMatchFinishedWidget::GoToMainMenu);
	}
}

void UMatchFinishedWidget::SetData(const ATetrisPlayerController* winnerController) const
{
	if (winnerController == nullptr)
	{
		return;
	}

	if (_text != nullptr)
	{
		if (const ATetrisPlayerState* winnerState = winnerController->GetPlayerState<ATetrisPlayerState>())
		{
			const FString message = FString::Printf(
				TEXT("%s\nwon with score %d!"), *winnerController->GetPlayerState<APlayerState>()->GetPlayerName(),
				static_cast<int32>(winnerState->GetScore()));
			_text->SetText(FText::FromString(message));
		}
	}
}

void UMatchFinishedWidget::GoToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
}
