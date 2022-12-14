#include "MainMenuWidget.h"

#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (_startSinglePlayerButton != nullptr)
	{
		_startSinglePlayerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::StartSinglePlayer);
	}

	if (_start2PlayerButton != nullptr)
	{
		_start2PlayerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Start2Player);
	}

	if (_quitButton != nullptr)
	{
		_quitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::QuitGame);
	}
}

void UMainMenuWidget::StartSinglePlayer()
{
	UGameplayStatics::OpenLevel(GetWorld(), "TetrisLevel", true, "?players=1");
}

void UMainMenuWidget::Start2Player()
{
	UGameplayStatics::OpenLevel(GetWorld(), "TetrisLevel", true, "?players=2");
}

void UMainMenuWidget::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}
