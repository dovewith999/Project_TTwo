#include <iostream>
#include "SokobanLevel.h"
#include "Math/Vector2.h"
#include "Actor/Player.h"
#include "Actor/Wall.h"
#include "Actor/Ground.h"
#include "Actor/Box.h"
#include "Actor/Target.h"
#include "Utils/Utils.h"

SokobanLevel::SokobanLevel()
{
	ReadMapFile("Stage1");
}

SokobanLevel::~SokobanLevel()
{
}

void SokobanLevel::Render()
{
	super::Render();

	if (isGameClear)
	{
		Utils::SetConsoleCursorPosition({ 30, 0 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::White));

		std::cout << "Game Clear!\n";

	}
}

bool SokobanLevel::CanPlayerMove(const Vector2& position, const Vector2& newPosition)
{
	if (isGameClear)
	{
		return false;
	}

	// 박스 처리.
	std::vector<Box*> boxActors;
	for (Actor* const actor : actors)
	{
		Box* box = actor->As<Box>();
		if (box)
		{
			boxActors.emplace_back(box);
		}
	}

	// 이동하려는 위치에 박스가 있는지 확인.
	Box* searchedBox = nullptr;
	for (Box* const boxActor : boxActors)
	{
		if (boxActor->GetPosition() == newPosition)
		{
			searchedBox = boxActor;
			break;
		}
	}
	
	if (searchedBox != nullptr)
	{
		// case 1 : 박스를 이동시키려는 위치에 다른 박스가 또 있는지 확인
		Vector2 direction = newPosition - position;
		Vector2 nextPosition = searchedBox->GetPosition() + direction;

		for (Box* const otherBox : boxActors)
		{
			if (otherBox == searchedBox)
			{
				continue;
			}

			if (otherBox->GetPosition() == nextPosition)
			{
				//플레이어 이동 못함
				return false;
			}
		}

		for (Actor* const actor : actors)
		{
			// case 2 : 박스는 없지만 벽이 있지 않은지 확인
			if (actor->GetPosition() == nextPosition)
			{
				if (actor->As<Wall>())
				{
					return false;
				}

				// case 3 : 이동 가능한 경우(그라운드, 타겟)에는 박스 이동 처리
				else if (actor->As<Ground>() || actor->As<Target>())
				{
					searchedBox->SetPosition(nextPosition);

					isGameClear = CheckGameClear();
					return true;
				}
			}
		}
	}

	// 플레이어가 이동하려는 위치에 박스가 없는 경우
	for (Actor* const actor : actors)
	{
		if (actor->GetPosition() == newPosition)
		{
			if (actor->As<Wall>())
			{
				return false;
			}

			return true;
		}
	}
	// ???
	return false;
}

void SokobanLevel::ReadMapFile(const char* fileName)
{
	// 최종 에셋 경로 완성.
	char filePath[256] = {};
	sprintf_s(filePath, 256, "../Assets/%s.txt", fileName);

	FILE* file = nullptr;
	fopen_s(&file, filePath, "rt");

	if (file == nullptr)
	{
		std::cout << "맵 파일 읽기 실패 : " << fileName << '\n';
		__debugbreak;
		return;
	}

	// 파싱(Parcing, 해석)
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
		case '#':
		case '1':
			AddActor(new Wall(position));
			break;
		case '.':
			AddActor(new Ground(position));
			break;
		case 'p':
			// Player Actor 생성
			// Player는 움직이기 때문에 땅도 같이 생성해야함
			AddActor(new Ground(position));
			AddActor(new Player(position));
			break;
		case 'b':
			AddActor(new Ground(position));
			AddActor(new Box(position));
			break;
		case 't':
			AddActor(new Target(position));
			++targetScore;
			break;
		default:
			break;
		}

		// x좌표 증가 처리
		++position.x;
	}

	// 버퍼 해제
	delete[] buffer;

	// 파일 닫기
	fclose(file);
}

bool SokobanLevel::CheckGameClear()
{
	// 박스가 타겟 위치에 모두 옮겨져있는지 확인
	int currentScore = 0;

	// 타겟 액터 벡터에 저장
	std::vector<Actor*> targetActors;
	std::vector<Actor*> boxActors;
	for (Actor* const actor : actors)
	{
		if (actor->As<Target>())
		{
			targetActors.emplace_back(actor);
		}

		else if (actor->As<Box>())
		{
			boxActors.emplace_back(actor);
		}
	}
	
	for (Actor* const targetActor : targetActors)
	{
		for (Actor* const boxActor : boxActors)
		{
			if (targetActor->GetPosition() == boxActor->GetPosition())
			{
				++currentScore;
			}
		}
	}

	return currentScore == targetScore;
}
 