#pragma once
namespace Define
{
	enum class ESoundChannelID
	{
		NONE		= -1,
		BGM	,			// 배경음
		EFFECT,			// 효과음

		MAXCHANNEL,		// 채널의 수
	};

	enum class EFileType
	{
		TITLE,
		BLOCK,
		MAP,
	};

	enum class ELevel
	{
		TITLE,
		TETRIS_SINGLE,
		TETRIS_MULTI,
	};
}