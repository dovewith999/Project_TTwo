#pragma once
#include "Engine.h"
#include "Etc/Singleton.h"
#include "Utils/Define.h"

using namespace Define;

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

};