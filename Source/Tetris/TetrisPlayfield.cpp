#include "TetrisPlayfield.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TetrisGameModeBase.h"
#include "TetrisPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ATetrisPlayfield::ATetrisPlayfield()
{
	PrimaryActorTick.bCanEverTick = true;

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = _root;

	_playfieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayfieldMesh"));
	_playfieldMesh->SetupAttachment(_root);

	_cubesInstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CubesInstancedMesh"));
	_cubesInstancedMesh->NumCustomDataFloats = 3;
	_cubesInstancedMesh->SetupAttachment(_root);

	_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	_camera->SetupAttachment(_root);
}

void ATetrisPlayfield::BeginPlay()
{
	Super::BeginPlay();

	SpawnTetromino();
}

void ATetrisPlayfield::PostInitProperties()
{
	Super::PostInitProperties();

	if (_playfieldMesh != nullptr)
	{
		const FVector oldSize = _playfieldMesh->GetRelativeScale3D();
		_playfieldMesh->SetRelativeScale3D(
			FVector((_playfieldWidth + 2) * _cubeSize / 100.0f, oldSize.Y, _playfieldHeight * _cubeSize / 100.0f));

		_playfieldMesh->SetRelativeLocation(FVector(0.0f, 0.0f, _playfieldHeight * _cubeSize / 2.0f));

		if (_cubesInstancedMesh != nullptr)
		{
			FVector playfieldMeshMin, playfieldMeshMax;
			_playfieldMesh->GetLocalBounds(playfieldMeshMin, playfieldMeshMax);
			_cubesInstancedMesh->SetRelativeLocation(FVector(-_playfieldWidth / 2.0f * _cubeSize + _cubeSize / 2.0f,
			                                                 playfieldMeshMax.Y / 2.0f + _cubeSize /
			                                                 2.0f,
			                                                 _cubeSize / 2.0f));
		}
	}

	_playfield = FPlayfield(_playfieldWidth, _playfieldHeight);

	_speed = _baseSpeed;
	SetActorTickInterval(1.0f / _speed);
}

void ATetrisPlayfield::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (_currentTetromino == nullptr)
	{
		return;
	}

	MoveTetromino(_currentTetromino, FIntVector2(0, -1));
}

void ATetrisPlayfield::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(playerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &ATetrisPlayfield::Move);
		EnhancedInputComponent->BindAction(_rotateAction, ETriggerEvent::Triggered, this, &ATetrisPlayfield::Rotate);

		EnhancedInputComponent->BindAction(_moveActionP2, ETriggerEvent::Triggered, this, &ATetrisPlayfield::MoveP2);
		EnhancedInputComponent->BindAction(_rotateActionP2, ETriggerEvent::Triggered, this,
		                                   &ATetrisPlayfield::RotateP2);
	}
}

void ATetrisPlayfield::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (const APlayerController* playerController = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->ClearAllMappings();

			subsystem->AddMappingContext(_inputMappingContext, 0);
		}
	}
}

void ATetrisPlayfield::Move(const FInputActionValue& inputActionValue)
{
	const FVector2D movementVector = inputActionValue.Get<FVector2D>();

	if (_currentTetromino == nullptr)
	{
		return;
	}

	MoveTetromino(_currentTetromino, FIntVector2(movementVector.X, movementVector.Y));
}

void ATetrisPlayfield::Rotate(const FInputActionValue& inputActionValue)
{
	if (_currentTetromino == nullptr)
	{
		return;
	}

	RotateTetromino(_currentTetromino);
}

void ATetrisPlayfield::MoveP2(const FInputActionValue& inputActionValue)
{
	// Horrible hack to get the second player to move because Unreal assigns one input device to one player only
	if (const APlayerController* player2 = UGameplayStatics::GetPlayerController(this, 1))
	{
		if (ATetrisPlayfield* otherPlayfield = Cast<ATetrisPlayfield>(player2->GetPawn()))
		{
			otherPlayfield->Move(inputActionValue);
		}
	}
}

void ATetrisPlayfield::RotateP2(const FInputActionValue& inputActionValue)
{
	if (const APlayerController* player2 = UGameplayStatics::GetPlayerController(this, 1))
	{
		if (ATetrisPlayfield* otherPlayfield = Cast<ATetrisPlayfield>(player2->GetPawn()))
		{
			otherPlayfield->Rotate(inputActionValue);
		}
	}
}

bool ATetrisPlayfield::CanSpawnTetromino(FIntVector2 position, const FTetrominoConfiguration& configuration) const
{
	if (IsOccupied(position))
	{
		return false;
	}

	for (const FIntVector2& block : configuration.blocks)
	{
		if (IsOccupied(position + block))
		{
			return false;
		}
	}

	return true;
}

void ATetrisPlayfield::SpawnTetromino()
{
	check(_currentTetromino == nullptr);

	if (_isGameOver)
	{
		return;
	}

	if (_tetrominoConfigurations.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No tetromino configurations found!"));
		return;
	}

	const FTetrominoConfiguration& configuration = _tetrominoConfigurations[FMath::RandRange(
		0, _tetrominoConfigurations.Num() - 1)];

	const FIntVector2 spawnPosition = FIntVector2(_playfieldWidth / 2, _playfieldHeight - 2);

	if (!CanSpawnTetromino(spawnPosition, configuration))
	{
		if (const ATetrisGameModeBase* gameMode = Cast<ATetrisGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			gameMode->PlayerGameOver(Cast<ATetrisPlayerController>(GetController()));
		}

		_isGameOver = true;

		SetActorTickEnabled(false);

		return;
	}

	_currentTetromino = NewObject<UTetromino>(this);
	_currentTetromino->position = spawnPosition;
	_currentTetromino->configuration = &configuration;

	for (const FIntVector2& block : configuration.blocks)
	{
		const FIntVector2 blockPosition(_currentTetromino->position.X + block.X,
		                                _currentTetromino->position.Y + block.Y);

		const int32 newCubeIndex = _cubesInstancedMesh->AddInstance(FTransform(
			FRotator::ZeroRotator, GetCubeLocation(blockPosition),
			FVector(_cubeSize / 100.0f)));

		_cubesInstancedMesh->SetCustomData(newCubeIndex, {
			                                   configuration.color.R, configuration.color.G, configuration.color.B
		                                   });

		_currentTetromino->ownedCubeIndices.Add(newCubeIndex);
	}
}

void ATetrisPlayfield::DestroyTetromino()
{
	for (const int32 ownedCubeIndex : _currentTetromino->ownedCubeIndices)
	{
		const FIntVector2 blockPositionLocalSpace = _currentTetromino->configuration->blocks[
			_currentTetromino->ownedCubeIndices.Find(ownedCubeIndex)];
		const FIntVector2 blockPositionBoardSpace = _currentTetromino->position + RotatePosition(
			blockPositionLocalSpace,
			_currentTetromino->rotation);
		_playfield[blockPositionBoardSpace.X][blockPositionBoardSpace.Y] = true;
	}

	for (const int32 ownedCubeIndex : _currentTetromino->ownedCubeIndices)
	{
		const FIntVector2 blockPositionLocalSpace = _currentTetromino->configuration->blocks[
			_currentTetromino->ownedCubeIndices.Find(ownedCubeIndex)];
		const FIntVector2 blockPositionBoardSpace = _currentTetromino->position + RotatePosition(
			blockPositionLocalSpace,
			_currentTetromino->rotation);

		if (IsRowFull(blockPositionBoardSpace.Y))
		{
			ClearRow(blockPositionBoardSpace.Y);
			_speed = FMath::Max(_baseSpeed, _speed / 2.0f);
		}
	}

	_currentTetromino->ConditionalBeginDestroy();
	_currentTetromino = nullptr;

	SpawnTetromino();
	_speed *= _speedMultiplier;
	SetActorTickInterval(1.0f / _speed);
}

void ATetrisPlayfield::MoveTetromino(UTetromino* tetromino, FIntVector2 delta)
{
	if (tetromino == nullptr)
	{
		return;
	}


	if (!CanMoveTetromino(tetromino, delta))
	{
		if (delta.X == 0 && delta.Y < 0)
		{
			DestroyTetromino();
		}

		return;
	}

	tetromino->position.X += delta.X;
	tetromino->position.Y += delta.Y;

	for (const int32 ownedCubeIndex : tetromino->ownedCubeIndices)
	{
		const FIntVector2 blockPositionLocalSpace = tetromino->configuration->blocks[
			tetromino->ownedCubeIndices.Find(ownedCubeIndex)];
		const FIntVector2 blockPositionBoardSpace = tetromino->position + RotatePosition(blockPositionLocalSpace,
			tetromino->rotation);

		const FVector newCubeLocation = GetCubeLocation(blockPositionBoardSpace);
		_cubesInstancedMesh->UpdateInstanceTransform(ownedCubeIndex, FTransform(
			                                             FRotator::ZeroRotator, newCubeLocation,
			                                             FVector(_cubeSize / 100.0f)), false);
	}

	_cubesInstancedMesh->MarkRenderStateDirty();
}

bool ATetrisPlayfield::CanMoveTetromino(const UTetromino* tetromino, FIntVector2 const& delta) const
{
	for (const FIntVector2& blockOffset : tetromino->configuration->blocks)
	{
		const FIntVector2 blockPositionBoardSpace = tetromino->position + RotatePosition(blockOffset,
			tetromino->rotation);
		const FIntVector2 newPosition = blockPositionBoardSpace + delta;
		if (IsOccupied(newPosition))
		{
			return false;
		}
	}

	return true;
}

void ATetrisPlayfield::RotateTetromino(UTetromino* tetromino)
{
	if (tetromino == nullptr)
	{
		return;
	}

	if (!CanRotateTetromino(tetromino))
	{
		return;
	}

	for (const int32 ownedCubeIndex : tetromino->ownedCubeIndices)
	{
		const FVector newCubeLocation = GetCubeLocation(tetromino->position + RotatePosition(
			tetromino->configuration->blocks[
				tetromino->ownedCubeIndices.Find(ownedCubeIndex)], tetromino->rotation + 90));
		_cubesInstancedMesh->UpdateInstanceTransform(ownedCubeIndex, FTransform(
			                                             FRotator::ZeroRotator, newCubeLocation,
			                                             FVector(_cubeSize / 100.0f)), false);
	}

	tetromino->rotation += 90;
	tetromino->rotation = tetromino->rotation % 360;

	_cubesInstancedMesh->MarkRenderStateDirty();
}

bool ATetrisPlayfield::CanRotateTetromino(const UTetromino* tetromino) const
{
	for (const FIntVector2& blockOffset : tetromino->configuration->blocks)
	{
		const FIntVector2 newPosition = tetromino->position + RotatePosition(blockOffset, tetromino->rotation + 90.0f);
		if (IsOccupied(newPosition))
		{
			return false;
		}
	}

	return true;
}

FVector ATetrisPlayfield::GetCubeLocation(FIntVector2 index) const
{
	return FVector(index.X * _cubeSize + _cubeSize / 2.0f, 0.0f, index.Y * _cubeSize + _cubeSize / 2.0f);
}

bool ATetrisPlayfield::IsRowFull(int32 rowIndex) const
{
	for (int32 x = 0; x < _playfieldWidth; ++x)
	{
		if (!IsOccupied(FIntVector2(x, rowIndex)))
		{
			return false;
		}
	}

	return true;
}

void ATetrisPlayfield::ClearRow(int32 rowIndex)
{
	TArray<int32> instancesToRemove;
	for (int32 Index = 0; Index < _cubesInstancedMesh->PerInstanceSMData.Num(); Index++)
	{
		const FMatrix& matrix = _cubesInstancedMesh->PerInstanceSMData[Index].Transform;

		if (matrix.GetOrigin().Z > rowIndex * _cubeSize && matrix.GetOrigin().Z < (rowIndex + 1) * _cubeSize)
		{
			instancesToRemove.Add(Index);
		}
	}

	_cubesInstancedMesh->RemoveInstances(instancesToRemove);

	for (int i = 0; i < _cubesInstancedMesh->GetNumRenderInstances(); ++i)
	{
		FTransform currentInstanceTransform;
		_cubesInstancedMesh->GetInstanceTransform(i, currentInstanceTransform);

		if (currentInstanceTransform.GetLocation().Z >= (rowIndex + 1) * _cubeSize)
		{
			currentInstanceTransform.AddToTranslation(FVector(0.0f, 0.0f, -_cubeSize));
			_cubesInstancedMesh->UpdateInstanceTransform(i, currentInstanceTransform, false);
		}
	}

	for (int32 y = rowIndex; y < _playfieldHeight - 1; ++y)
	{
		for (int32 x = 0; x < _playfieldWidth; ++x)
		{
			_playfield[x][y] = _playfield[x][y + 1];
			_playfield[x][y + 1] = false;
		}
	}

	_cubesInstancedMesh->MarkRenderStateDirty();

	if (const ATetrisGameModeBase* gameMode = Cast<ATetrisGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		gameMode->PlayerClearedRow(Cast<ATetrisPlayerController>(GetController()), _speed);
	}
}

bool ATetrisPlayfield::IsOutOfBounds(FIntVector2 index) const
{
	return index.X < 0 || index.X >= _playfieldWidth || index.Y < 0 || index.Y >= _playfieldHeight;
}

bool ATetrisPlayfield::IsOccupied(FIntVector2 index) const
{
	return IsOutOfBounds(index) || _playfield.columns[index.X].cells[index.Y] == true;
}

FIntVector2 ATetrisPlayfield::RotatePosition(const FIntVector2& position, int32 angle) const
{
	FVector rotatedVector = FVector(position.X, 0.0f, position.Y);
	rotatedVector = rotatedVector.RotateAngleAxis(angle, FVector(0.0f, 1.0f, 0.0f));

	return FIntVector2(FMath::RoundToInt32(rotatedVector.X),
	                   FMath::RoundToInt32(rotatedVector.Z));
}

FIntVector2 operator+(const FIntVector2& lhs, const FIntVector2& rhs)
{
	return FIntVector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
}
