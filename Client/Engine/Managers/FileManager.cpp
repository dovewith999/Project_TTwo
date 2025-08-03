#include "FileManager.h"
#include "Math/Vector2.h"

#include <iostream>

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

void FileManager::ReadFile(const char* fileName, EFileType fileType)
{
	// 최종 에셋 경로 완성.
	char filePath[256] = {};
	sprintf_s(filePath, 256, "../Resources/Files/%s.txt", fileName);

	FILE* file = nullptr;
	fopen_s(&file, filePath, "rb");

	if (file == nullptr)
	{
		std::cout << "파일 읽기 실패 : " << fileName << '\n';
		__debugbreak();
		return;
	}

	switch (fileType)
	{
	case Define::EFileType::TITLE:
		ReadTitleFile(file);
		break;
	case Define::EFileType::BLOCK:
		ReadBlockFile(file);
		break;
	case Define::EFileType::MAP:
		ReadMapFile(file);
		break;
	default:
		break;
	}

	fclose(file);
}

void FileManager::ReadTitleFile(FILE* file)
{
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* buffer = new char[fileSize + 1];
	buffer[fileSize] = '\0';
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	//if (readSize != fileSize)
	//{
	//	std::cout << "fileSize is not matched with readSize\n";
	//}

	int index = 0;
	int size = static_cast<int>(readSize);

	Vector2 position(0, 0);

	while (index < size)
	{
		char mapCharacter = buffer[index++];

		// 개행 문자 처리
		if (mapCharacter == '\n')
		{
			position.x = 0;
			++position.y;

			//std::cout << "\n";
			continue;
		}

		// 각 문자 별로 처리
		switch (mapCharacter)
		{
		case '~':
			std::cout << '~';
			break;
		default:
			break;
		}

		// x좌표 증가 처리
		++position.x;
	}

	// 버퍼 해제
	delete[] buffer;
}

void FileManager::ReadMapFile(FILE* file)
{
	// 파싱(Parcing, 해석)
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* buffer = new char[fileSize + 1];
	buffer[fileSize] = '\0';
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	if (readSize != fileSize)
	{
		std::cout << "fileSize is not matched with readSize\n";
	}

	int index = 0;
	int size = static_cast<int>(readSize);

	Vector2 position(3, 3);

	while (index < size)
	{
		char mapCharacter = buffer[index++];

		// 개행 문자 처리
		if (mapCharacter == ',')
		{
			std::cout << '\n'; // 콘솔에 개행 출력
			position.x = 3;
			++position.y;
			continue;
		}

		// 실제 개행 문자들 무시 (이미 콤마로 처리했으므로)
		if (mapCharacter == '\n' || mapCharacter == '\r')
		{
			continue;
		}

		// 각 문자 별로 처리
		switch (mapCharacter)
		{
		case '1':
			std::cout << Block_Types[1];
			break;
		case '0' : 
			std::cout << Block_Types[0];
			break;
		}

		// x좌표 증가 처리
		++position.x;
	}

	// 버퍼 해제
	delete[] buffer;

}

void FileManager::ReadBlockFile(FILE* file)
{
}