#pragma once
#include "Engine.h"
#include "Etc/Singleton.h"
#include "Utils/Define.h"
#include <unordered_map>
#include <string>
#include <vector>

using namespace Define;

/// <summary>
/// 게임에서 사용하는 모든 리소스를 관리하는 매니저
/// 블록 모양, 맵 데이터, 사운드, 텍스처 등을 로드하고 캐싱
/// 작성자 : 임희섭
/// 작성일 : 25/08/04
/// </summary>

// 블록 모양 데이터 구조체
struct BlockShapeData
{
	bool shape[4][4];  // 4x4 격자

	BlockShapeData()
	{
		memset(shape, false, sizeof(shape));
	}

	bool GetPixel(int x, int y) const
	{
		if (x < 0 || x >= 4 || y < 0 || y >= 4)
			return false;
		return shape[y][x];
	}

	void SetPixel(int x, int y, bool value)
	{
		if (x >= 0 && x < 4 && y >= 0 && y < 4)
			shape[y][x] = value;
	}
};

// 맵 데이터 구조체
struct MapData
{
	std::vector<std::vector<int>> tiles;
	int width = 0;
	int height = 0;

	MapData() = default;

	int GetTile(int x, int y) const
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return -1;
		return tiles[y][x];
	}

	void SetTile(int x, int y, int value)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			tiles[y][x] = value;
	}
};

class Engine_API ResourceManager : public SafeSingleton<ResourceManager>
{
	friend class SafeSingleton<ResourceManager>;

public:
	explicit ResourceManager();
	virtual ~ResourceManager();

public:
	// 초기화 및 리소스 로드
	void Initialize();
	void LoadAllResources();
	void ClearAllResources();

	// 블록 모양 관리
	void LoadBlockShapes(const char* fileName);
	const BlockShapeData* GetBlockShape(int blockType, int rotation) const;
	//bool IsValidBlockType(int blockType) const;

	// 맵 데이터 관리
	//void LoadMapData(const char* fileName);
	//const MapData* GetMapData(const std::string& mapName) const;

	// 유틸리티 함수
	std::string GetResourcePath(const char* fileName, const char* extension = ".txt") const;

private:
	// 파일 파싱 함수들
	bool ParseBlockShapeLine(const std::string& line);
	bool ParseMapFile(const char* filePath, const std::string& mapName);
	std::vector<std::string> SplitString(const std::string& str, char delimiter) const;
	std::string TrimString(const std::string& str) const;

private:
	// 블록 모양 데이터 [blockType][rotation]
	// blockType을 편의상 I = 2로 시작하게 해놨기 때문에 배열에 인덱스로 이용할 때는 -2해줌
	BlockShapeData blockShapes[7][4];
	bool blockShapesLoaded = false;
	bool isInitialize = false;

	// 맵 데이터 컨테이너
	std::unordered_map<std::string, MapData> mapDataContainer;

	// 리소스 경로
	std::string resourcePath = "../Resources/";

	// 블록 타입 이름 매핑
	static const std::unordered_map<std::string, int> blockTypeMap;
};