#pragma once
#include "Engine.h"
#include "Etc/Singleton.h"
#include "Utils/Define.h"
#include <unordered_map>
#include <string>
#include <vector>

using namespace Define;

/// <summary>
/// ���ӿ��� ����ϴ� ��� ���ҽ��� �����ϴ� �Ŵ���
/// ��� ���, �� ������, ����, �ؽ�ó ���� �ε��ϰ� ĳ��
/// �ۼ��� : ����
/// �ۼ��� : 25/08/04
/// </summary>

// ��� ��� ������ ����ü
struct BlockShapeData
{
	bool shape[4][4];  // 4x4 ����

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

// �� ������ ����ü
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
	// �ʱ�ȭ �� ���ҽ� �ε�
	void Initialize();
	void LoadAllResources();
	void ClearAllResources();

	// ��� ��� ����
	void LoadBlockShapes(const char* fileName);
	const BlockShapeData* GetBlockShape(int blockType, int rotation) const;
	bool IsValidBlockType(int blockType) const;

	// �� ������ ����
	void LoadMapData(const char* fileName);
	const MapData* GetMapData(const std::string& mapName) const;

	// ��ƿ��Ƽ �Լ�
	std::string GetResourcePath(const char* fileName, const char* extension = ".txt") const;

private:
	// ���� �Ľ� �Լ���
	bool ParseBlockShapeLine(const std::string& line);
	bool ParseMapFile(const char* filePath, const std::string& mapName);
	std::vector<std::string> SplitString(const std::string& str, char delimiter) const;
	std::string TrimString(const std::string& str) const;

private:
	// ��� ��� ������ [blockType][rotation]
	BlockShapeData blockShapes[7][4];
	bool blockShapesLoaded = false;

	// �� ������ �����̳�
	std::unordered_map<std::string, MapData> mapDataContainer;

	// ���ҽ� ���
	std::string resourcePath = "../Resources/";

	// ��� Ÿ�� �̸� ����
	static const std::unordered_map<std::string, int> blockTypeMap;
};