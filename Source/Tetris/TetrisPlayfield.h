// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "TetrisPlayfield.generated.h"

class UInputAction;
class UCameraComponent;
class UInputMappingContext;
struct FInputActionValue;

USTRUCT()
struct FPlayfieldColumn
{
public:
	GENERATED_BODY()

	UPROPERTY()
	TArray<bool> cells;

	FPlayfieldColumn()
	{
		FPlayfieldColumn(0);
	}

	FPlayfieldColumn(int32 yCount)
	{
		cells.Init(false, yCount);
	}

	void ClearRow()
	{
		cells.Init(false, cells.Num());
	}

	bool& operator[](int32 index)
	{
		return cells[index];
	}
};

USTRUCT()
struct FPlayfield
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayfieldColumn> columns;

	FPlayfield()
	{
		FPlayfield(0, 0);
	}

	FPlayfield(int32 xCount, int32 yCount)
	{
		columns.Init(FPlayfieldColumn(yCount), xCount);
	}

	FPlayfieldColumn& operator[](int32 row)
	{
		return columns[row];
	}
};

USTRUCT()
struct FTetrominoConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Tetromino")
	TArray<FIntVector2> blocks;

	UPROPERTY(EditDefaultsOnly, Category = "Tetromino")
	FLinearColor color{0.0f, 0.0f, 0.0f, 1.0f};
};

UCLASS()
class UTetromino : public UObject
{
public:
	GENERATED_BODY()

	FIntVector2 position;
	int32 rotation;

	TArray<int32> ownedCubeIndices;

	FTetrominoConfiguration const* configuration;
};

UCLASS()
class TETRIS_API ATetrisPlayfield : public APawn
{
	GENERATED_BODY()

public:
	ATetrisPlayfield();

protected:
	virtual void BeginPlay() override;
public:
	virtual void PostInitProperties() override;
	virtual void Tick(float deltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;
	
	virtual void PawnClientRestart() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* _root;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* _playfieldRoot;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* _playfieldMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UInstancedStaticMeshComponent* _cubesInstancedMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCameraComponent* _camera;

	FPlayfield _playfield;

	UPROPERTY(EditDefaultsOnly, Category = "Playfield")
	int32 _playfieldWidth = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Playfield")
	int32 _playfieldHeight = 24;

	UPROPERTY(EditDefaultsOnly, Category = "Playfield")
	float _cubeSize = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float _baseSpeed = 1.0f;

	float _speed = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float _speedMultiplier = 1.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* _moveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* _rotateAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* _moveActionP2;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* _rotateActionP2;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* _inputMappingContext;
	
	UPROPERTY()
	UTetromino* _currentTetromino;

	UPROPERTY(EditDefaultsOnly, Category = "Tetromino")
	TArray<FTetrominoConfiguration> _tetrominoConfigurations;

	bool _isGameOver = false;

private:
	void Move(const FInputActionValue& inputActionValue);
	void Rotate(const FInputActionValue& inputActionValue);

	void MoveP2(const FInputActionValue& inputActionValue);
	void RotateP2(const FInputActionValue& inputActionValue);

	bool CanSpawnTetromino(FIntVector2 position, const FTetrominoConfiguration& configuration) const;
	void SpawnTetromino();
	void DestroyTetromino();

	void MoveTetromino(UTetromino* tetromino, FIntVector2 delta);
	bool CanMoveTetromino(const UTetromino* tetromino, FIntVector2 const& offset) const;

	void RotateTetromino(UTetromino* tetromino);
	bool CanRotateTetromino(const UTetromino* tetromino) const;

	FVector GetCubeLocation(FIntVector2 index) const;

	bool IsRowFull(int32 rowIndex) const;
	void ClearRow(int32 rowIndex);

	bool IsOutOfBounds(FIntVector2 index) const;

	bool IsOccupied(FIntVector2 index) const;

	FIntVector2 RotatePosition(const FIntVector2& position, int32 angle) const;
};

FIntVector2 operator+(const FIntVector2& lhs, const FIntVector2& rhs);
