#pragma once
#include "Engine.h"
#include "Etc/Singleton.h"
#include "Utils/Define.h"

using namespace Define;


/// <summary>
/// 파일을 입출력해오는 기능을 담당하는 매니저 클래스
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>

class Engine_API FileManager : public SafeSingleton<FileManager>
{
	friend class SafeSingleton<FileManager>;

public:
	explicit FileManager();
	virtual ~FileManager();

public:
	void ReadFile(const char* fileName, EFileType fileType);
	void ReadTitleFile(FILE* file);
	void ReadMapFile(FILE* file);
	void ReadBlockFile(FILE* file);

private:
	const char* Block_Types[3] =
	{
		"  ",  // 빈 공간
		"▣",  // 프레임
		"□"    // 블록
	};
};