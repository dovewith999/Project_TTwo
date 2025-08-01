#include "DemoLevel.h"
#include "TestActor.h"
#include <iostream>

DemoLevel::DemoLevel()
{
	AddActor(new TestActor());

#pragma region 테스트로 맵 파일 읽기
	FILE* file = nullptr;
	fopen_s(&file, "../Assets/Map.txt", "rb");

	//예외 처리
	if (file == nullptr)
	{
		std::cout << "Failed to open Map.txt file \n";
		__debugbreak;
		return;
	}

	//파일 크기 확인
	fseek(file, 0, SEEK_END); // 커서를 파일 맨 끝으로 이동
	size_t fileSize = ftell(file); // 그 지점의 위치 반환
	fseek(file, 0, SEEK_SET); // 커서를 처음으로 되돌림 rewind(file)을 사용해도 됨

	//char* buffer = new char[fileSize + 1]; // 버퍼 할당
	char buffer[256] = {};

	while (feof(file) == false) // 파일의 끝이 아니면 == 아직 다 안읽었다면
	{
		fgets(buffer, 256, file); // 한 줄씩 읽기

		int lineLenght = static_cast<int>(strlen(buffer)); // 문자열 한 줄의 길이 구하기

		// Parcing(해석)
		for (int i = 0; i < lineLenght; ++i)
		{
			char mapChar = buffer[i];

			switch (mapChar)
			{
			case '#':
				std::cout << "#";
				break;
			case '.':
				std::cout << ".";
				break;
			case 'p':
				std::cout << "p";
				break;
			case 'b':
				std::cout << "b";
				break;
			case 't':
				std::cout << "t";
				break;
			default:
				break;
			}
		}			
		
		// 개행
		std::cout << "\n";
	}

	fclose(file);
#pragma endregion
}
