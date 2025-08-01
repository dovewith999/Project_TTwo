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

	// 최대 32 채널까지 동시 재생이 가능
	//enum class ESoundChannelID
	//{ 
	//	SOUND_BGM, SOUND_PLAYER, SOUND_MONSTER, SOUND_EFFECT, SOUND_UI, MAXCHANNEL
	//};
}