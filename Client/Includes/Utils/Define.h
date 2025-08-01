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
		// TODO : 필요에 따라 UI로 교체 가능성 있음
		MAP,
	};
}