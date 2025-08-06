#pragma once
#include "Level/Level.h"
#include "Actor/TetrisBlock.h"
#include "Interface/ITetrisGameLogic.h"

#include <vector>

/// <summary>
/// 테트리스 게임의 메인 레벨
/// 게임 보드, 블록 생성/관리, 라인 클리어 등을 담당
/// 작성자 : 임희섭
/// 작성일 : 25/08/04
/// </summary>
/// 
class TetrisController;
class TetrisLevel : public Level, public ITetrisGameLogic
{
	RTTI_DECLARATIONS(TetrisLevel, Level)

public:
	explicit TetrisLevel();
	virtual ~TetrisLevel();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
	void EndPlay();

	// 게임 상태 관리
	void StartGame();
	void PauseGame();
	void ResumeGame();
	void EndGame();

	// 블록 관리
	virtual void SpawnNewBlock() override;
	void HandleInput();
	bool IsGameOver() const;

	// 게임 로직 관리 - 인터페이스 오버라이딩
	virtual bool CanBlockMoveTo(const Vector2& position, EBlockType blockType, int rotation) const override;
	virtual void PlaceBlockOnBoard(TetrisBlock* block) override;
	virtual int ClearCompletedLines() override;
	virtual void ProcessCompletedLines() override;
	virtual void SwitchBlock() override;

	void RenderBoard();
	void RenderUI();

protected:
	// 다음 블록 (7-bag 시스템용)
	std::vector<EBlockType> nextBlocks;
	void GenerateNextBag();
	EBlockType GetNextBlockType();

	// 유틸리티
	void InitializeBoard();
	void LoadMapFromFile(const char* fileName);
	void UpdateShadowBlock();
	Vector2 GetSpawnPosition() const;

	Vector2 CalculateShadowPosition(const Vector2& currentPos, EBlockType blockType, int rotation) const;

protected:
	// 게임 상태
	bool isGameStarted = false;
	bool isGamePaused = false;
	bool isGameOver = false;

	// 입력을 제어하기 위한 컨트롤러
	TetrisController* controller = nullptr;

	// 현재 조작 중인 블록
	TetrisBlock* currentBlock = nullptr;
	TetrisBlock* shadowBlock = nullptr;  // 그림자 블록
	
	EBlockType saveBlockType = EBlockType::None; // 저장하고 스위칭 할 블럭

	const char* nextBlockUI[4][4] = { " " }; // 다음 블록이 무엇인지 그려줄 공간
	const char* saveBlockUI[4][4] = { " " }; // 저장하고 있는 블록이 무엇인지 그려줄 공간

	EBlockType newBlockType;
	EBlockType nextBlockType;
	
	// 게임 보드 (12x21 크기 - Map.txt와 맞춤)
	static const int BOARD_WIDTH = 12;
	static const int BOARD_HEIGHT = 21;
	int gameBoard[BOARD_HEIGHT][BOARD_WIDTH];

	const int BOARD_START_X = 20; // 보드 그리기 시작 위치 x
	const int BOARD_START_Y = 3; // 보드 그리기 시작 위치 y

	// 렌더링용 문자
	const char* Block_Types[4] = {
		"  ",  // 0: 빈 공간
		"▣",  // 1: 프레임/벽
		"□",  // 2: 고정된 블록
		"■"   // 3: 현재 떨어지는 블록
	};

	// 게임 타이머
	float blockDropTimer = 0.0f;
	float blockDropInterval = 1.0f;  // 1초마다 블록 자동 낙하

	// 점수 및 통계
	int score = 0;
	int linesCleared = 0;
	int level = 1;
};