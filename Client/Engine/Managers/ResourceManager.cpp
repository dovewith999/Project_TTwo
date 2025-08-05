#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// 블록 타입 이름 매핑
const std::unordered_map<std::string, int> ResourceManager::blockTypeMap = {
	{"I", 2}, {"O", 3}, {"T", 4}, {"S", 5}, {"Z", 6}, {"J", 7}, {"L", 8}
};

ResourceManager::ResourceManager()
{
	// 블록 모양 배열 초기화
	memset(blockShapes, 0, sizeof(blockShapes));
}

ResourceManager::~ResourceManager()
{
	ClearAllResources();
}

void ResourceManager::Initialize()
{
	LoadAllResources();
}

void ResourceManager::LoadAllResources()
{
	// 블록 모양 데이터 로드
	LoadBlockShapes("BlockShapes");
}

void ResourceManager::ClearAllResources()
{
	// 맵 데이터 클리어
	mapDataContainer.clear();

	// 블록 모양 데이터 클리어
	memset(blockShapes, 0, sizeof(blockShapes));
	blockShapesLoaded = false;
}

void ResourceManager::LoadBlockShapes(const char* fileName)
{
	std::string filePath = GetResourcePath(fileName);
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		__debugbreak();
		return;
	}

	std::string line;
	int loadedShapes = 0;


	while (std::getline(file, line))
	{
		// 주석이나 빈 줄 건너뛰기
		line = TrimString(line);
		if (line.empty() || line[0] == '#')
			continue;

		if (ParseBlockShapeLine(line))
		{
			loadedShapes++;
		}
	}

	file.close();
	blockShapesLoaded = true;

}

const BlockShapeData* ResourceManager::GetBlockShape(int blockType, int rotation) const
{
	if (!blockShapesLoaded)
	{
		return nullptr;
	}

	if (blockType < 2 || blockType >= 9 || rotation < 0 || rotation >= 4)
	{
		return nullptr;
	}

	return &blockShapes[blockType][rotation];
}

bool ResourceManager::IsValidBlockType(int blockType) const
{
	return blockType >= 0 && blockType < 7 && blockShapesLoaded;
}

void ResourceManager::LoadMapData(const char* fileName)
{
	std::string mapName = fileName;
	std::string filePath = GetResourcePath(fileName);

	if (ParseMapFile(filePath.c_str(), mapName))
	{
	}
	else
	{
	}
}

const MapData* ResourceManager::GetMapData(const std::string& mapName) const
{
	auto it = mapDataContainer.find(mapName);
	if (it != mapDataContainer.end())
	{
		return &it->second;
	}

	return nullptr;
}

std::string ResourceManager::GetResourcePath(const char* fileName, const char* extension) const
{
	return resourcePath + "Files/" + fileName + extension;
}

bool ResourceManager::ParseBlockShapeLine(const std::string& line)
{
	// 형식: BlockType:Rotation:Row0,Row1,Row2,Row3
	// 예: I:0:0000,1111,0000,0000

	std::vector<std::string> parts = SplitString(line, ':');
	if (parts.size() != 3)
	{
		return false;
	}

	// 블록 타입 파싱
	std::string blockTypeStr = TrimString(parts[0]);
	auto typeIt = blockTypeMap.find(blockTypeStr);
	if (typeIt == blockTypeMap.end())
	{
		return false;
	}
	int blockType = typeIt->second;

	// 회전값 파싱
	int rotation = std::stoi(TrimString(parts[1]));
	if (rotation < 0 || rotation >= 4)
	{
		return false;
	}

	// 행 데이터 파싱
	std::vector<std::string> rows = SplitString(parts[2], ',');
	if (rows.size() != 4)
	{
		return false;
	}

	// 4x4 격자에 데이터 저장
	BlockShapeData& shapeData = blockShapes[blockType][rotation];

	for (int y = 0; y < 4; ++y)
	{
		std::string row = TrimString(rows[y]);
		if (row.length() != 4)
		{
			//std::cout << "[ResourceManager] 잘못된 행 길이: " << row << "\n";
			return false;
		}

		for (int x = 0; x < 4; ++x)
		{
			bool pixel = (row[x] == '1');
			shapeData.SetPixel(x, y, pixel);
		}
	}

	return true;
}

bool ResourceManager::ParseMapFile(const char* filePath, const std::string& mapName)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	MapData mapData;
	std::string line;

	// 파일을 한 줄씩 읽어서 맵 데이터 생성
	while (std::getline(file, line))
	{
		line = TrimString(line);
		if (line.empty() || line[0] == '#')
			continue;

		std::vector<int> row;

		// 콤마로 구분된 데이터 파싱
		std::vector<std::string> tokens = SplitString(line, ',');
		for (const std::string& token : tokens)
		{
			std::string trimmed = TrimString(token);
			if (!trimmed.empty())
			{
				row.push_back(std::stoi(trimmed));
			}
		}

		if (!row.empty())
		{
			mapData.tiles.push_back(row);
		}
	}

	file.close();

	// 맵 크기 설정
	if (!mapData.tiles.empty())
	{
		mapData.height = static_cast<int>(mapData.tiles.size());
		mapData.width = static_cast<int>(mapData.tiles[0].size());

		// 모든 행의 길이가 동일한지 확인
		for (const auto& row : mapData.tiles)
		{
			if (static_cast<int>(row.size()) != mapData.width)
			{
				return false;
			}
		}

		mapDataContainer[mapName] = mapData;
		return true;
	}

	return false;
}

std::vector<std::string> ResourceManager::SplitString(const std::string& str, char delimiter) const
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		tokens.push_back(token);
	}

	return tokens;
}

std::string ResourceManager::TrimString(const std::string& str) const
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";

	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}