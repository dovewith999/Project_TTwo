#pragma once
/// <summary>
/// enum, const 같은 상수적인 값, 대명사들을 정의하는 네임 스페이스
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>
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