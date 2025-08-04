#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// ��� Ÿ�� �̸� ����
const std::unordered_map<std::string, int> ResourceManager::blockTypeMap = {
	{"I", 0}, {"O", 1}, {"T", 2}, {"S", 3}, {"Z", 4}, {"J", 5}, {"L", 6}
};

ResourceManager::ResourceManager()
{
	// ��� ��� �迭 �ʱ�ȭ
	memset(blockShapes, 0, sizeof(blockShapes));
}

ResourceManager::~ResourceManager()
{
	ClearAllResources();
}

void ResourceManager::Initialize()
{
	std::cout << "[ResourceManager] �ʱ�ȭ ����\n";
	LoadAllResources();
	std::cout << "[ResourceManager] �ʱ�ȭ �Ϸ�\n";
}

void ResourceManager::LoadAllResources()
{
	// ��� ��� ������ �ε�
	LoadBlockShapes("BlockShapes");

	// �⺻ �� ������ �ε� (�ʿ��)
	// LoadMapData("DefaultMap");
}

void ResourceManager::ClearAllResources()
{
	// �� ������ Ŭ����
	mapDataContainer.clear();

	// ��� ��� ������ Ŭ����
	memset(blockShapes, 0, sizeof(blockShapes));
	blockShapesLoaded = false;

	std::cout << "[ResourceManager] ��� ���ҽ� ���� �Ϸ�\n";
}

void ResourceManager::LoadBlockShapes(const char* fileName)
{
	std::string filePath = GetResourcePath(fileName);
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		std::cout << "[ResourceManager] ��� ��� ���� ���� ����: " << filePath << "\n";
		__debugbreak();
		return;
	}

	std::string line;
	int loadedShapes = 0;

	std::cout << "[ResourceManager] ��� ��� �ε� ����: " << filePath << "\n";

	while (std::getline(file, line))
	{
		// �ּ��̳� �� �� �ǳʶٱ�
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

	std::cout << "[ResourceManager] ��� ��� �ε� �Ϸ�: " << loadedShapes << "�� �ε��\n";
}

const BlockShapeData* ResourceManager::GetBlockShape(int blockType, int rotation) const
{
	if (!blockShapesLoaded)
	{
		std::cout << "[ResourceManager] ���: ��� ����� �ε���� ����\n";
		return nullptr;
	}

	if (blockType < 0 || blockType >= 7 || rotation < 0 || rotation >= 4)
	{
		std::cout << "[ResourceManager] �߸��� ��� Ÿ�� �Ǵ� ȸ����: " << blockType << ", " << rotation << "\n";
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
		std::cout << "[ResourceManager] �� ������ �ε� �Ϸ�: " << mapName << "\n";
	}
	else
	{
		std::cout << "[ResourceManager] �� ������ �ε� ����: " << mapName << "\n";
	}
}

const MapData* ResourceManager::GetMapData(const std::string& mapName) const
{
	auto it = mapDataContainer.find(mapName);
	if (it != mapDataContainer.end())
	{
		return &it->second;
	}

	std::cout << "[ResourceManager] �� �����͸� ã�� �� ����: " << mapName << "\n";
	return nullptr;
}

std::string ResourceManager::GetResourcePath(const char* fileName, const char* extension) const
{
	return resourcePath + "Files/" + fileName + extension;
}

bool ResourceManager::ParseBlockShapeLine(const std::string& line)
{
	// ����: BlockType:Rotation:Row0,Row1,Row2,Row3
	// ��: I:0:0000,1111,0000,0000

	std::vector<std::string> parts = SplitString(line, ':');
	if (parts.size() != 3)
	{
		std::cout << "[ResourceManager] �߸��� ��� ����: " << line << "\n";
		return false;
	}

	// ��� Ÿ�� �Ľ�
	std::string blockTypeStr = TrimString(parts[0]);
	auto typeIt = blockTypeMap.find(blockTypeStr);
	if (typeIt == blockTypeMap.end())
	{
		std::cout << "[ResourceManager] �� �� ���� ��� Ÿ��: " << blockTypeStr << "\n";
		return false;
	}
	int blockType = typeIt->second;

	// ȸ���� �Ľ�
	int rotation = std::stoi(TrimString(parts[1]));
	if (rotation < 0 || rotation >= 4)
	{
		std::cout << "[ResourceManager] �߸��� ȸ����: " << rotation << "\n";
		return false;
	}

	// �� ������ �Ľ�
	std::vector<std::string> rows = SplitString(parts[2], ',');
	if (rows.size() != 4)
	{
		std::cout << "[ResourceManager] �߸��� �� ����: " << rows.size() << "\n";
		return false;
	}

	// 4x4 ���ڿ� ������ ����
	BlockShapeData& shapeData = blockShapes[blockType][rotation];

	for (int y = 0; y < 4; ++y)
	{
		std::string row = TrimString(rows[y]);
		if (row.length() != 4)
		{
			std::cout << "[ResourceManager] �߸��� �� ����: " << row << "\n";
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
		std::cout << "[ResourceManager] �� ���� ���� ����: " << filePath << "\n";
		return false;
	}

	MapData mapData;
	std::string line;

	// ������ �� �پ� �о �� ������ ����
	while (std::getline(file, line))
	{
		line = TrimString(line);
		if (line.empty() || line[0] == '#')
			continue;

		std::vector<int> row;

		// �޸��� ���е� ������ �Ľ�
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

	// �� ũ�� ����
	if (!mapData.tiles.empty())
	{
		mapData.height = static_cast<int>(mapData.tiles.size());
		mapData.width = static_cast<int>(mapData.tiles[0].size());

		// ��� ���� ���̰� �������� Ȯ��
		for (const auto& row : mapData.tiles)
		{
			if (static_cast<int>(row.size()) != mapData.width)
			{
				std::cout << "[ResourceManager] �� ������ �� ���� ����ġ\n";
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