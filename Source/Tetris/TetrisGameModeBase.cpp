#include "TetrisGameModeBase.h"

#include "ATetrisPlayerState.h"
#include "TetrisHUD.h"
#include "TetrisPlayerController.h"
#include "TetrisPlayfield.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

ATetrisGameModeBase::ATetrisGameModeBase()
{
	DefaultPawnClass = ATetrisPlayfield::StaticClass();
	HUDClass = ATetrisHUD::StaticClass();
	PlayerControllerClass = ATetrisPlayerController::StaticClass();
	PlayerStateClass = ATetrisPlayerState::StaticClass();
}

void ATetrisGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator() + 1; it; ++it)
	{
		GetGameInstance()->RemoveLocalPlayer(it->Get()->GetLocalPlayer());
	}
}

void ATetrisGameModeBase::StartPlay()
{
	Super::StartPlay();

	const int32 playerCount = UGameplayStatics::GetIntOption(OptionsString, "players", 1);

	for (int32 i = 1; i < playerCount; ++i)
	{
		FString error;
		GetGameInstance()->CreateLocalPlayer(i, error, true);
		if (error.Len() > 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Error creating local player: %s"), *error);
		}
	}
}

void ATetrisGameModeBase::PlayerClearedRow(const ATetrisPlayerController* playerController, float speed) const
{
	if (playerController == nullptr)
	{
		return;
	}

	APlayerState* playerState = playerController->GetPlayerState<APlayerState>();
	if (playerState != nullptr)
	{
		const float newScore = playerState->GetScore() + speed * _scoreMultiplier;
		playerState->SetScore(newScore);
		if (const ATetrisHUD* hud = Cast<ATetrisHUD>(playerController->GetHUD()))
		{
			hud->UpdateScore(newScore);
		}
	}
}

void ATetrisGameModeBase::PlayerGameOver(const ATetrisPlayerController* playerController) const
{
	if (playerController == nullptr)
	{
		return;
	}

	ATetrisPlayerState* playerState = playerController->GetPlayerState<ATetrisPlayerState>();
	if (playerState != nullptr)
	{
		playerState->SetIsGameOver(true);
		if (ATetrisHUD* hud = Cast<ATetrisHUD>(playerController->GetHUD()))
		{
			hud->ShowGameOver(playerState->GetScore());
		}
	}

	if (const AGameStateBase* gameState = GetWorld()->GetGameState())
	{
		for (const APlayerState* state : gameState->PlayerArray)
		{
			if (state != nullptr && !Cast<ATetrisPlayerState>(state)->GetIsGameOver())
			{
				return;
			}
		}
	}

	DeclareWinner();
}

void ATetrisGameModeBase::DeclareWinner() const
{
	if (const AGameStateBase* gameState = GetWorld()->GetGameState())
	{
		float bestScore = -1.0f;
		const APlayerController* bestPlayer = nullptr;
		for (const APlayerState* state : gameState->PlayerArray)
		{
			if (state != nullptr && state->GetScore() > bestScore)
			{
				bestScore = state->GetScore();
				bestPlayer = state->GetPlayerController();
			}
		}

		if (bestPlayer != nullptr)
		{
			for (const APlayerState* state : gameState->PlayerArray)
			{
				if (const ATetrisPlayerController* playerController = Cast<ATetrisPlayerController>(
					state->GetPlayerController()))
				{
					if (ATetrisHUD* hud = Cast<ATetrisHUD>(playerController->GetHUD()))
					{
						hud->ShowMatchFinished(Cast<ATetrisPlayerController>(bestPlayer));
					}
				}
			}
		}
	}
}
