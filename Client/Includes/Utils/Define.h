#pragma once
/// <summary>
/// enum, const 같은 상수적인 값, 대명사들을 정의하는 네임 스페이스
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>
namespace Define
{
	enum class ESoundChannelID // 동시에 재생하고 싶은 사운드의 갯수만큼 정의해서 사용(최대 32개)
	{
		NONE		= -1,
		BGM			=  1, // 배경음 (배경음)
		EFFECT,			  // 효과음 (이동, 회전)
		SUBEFFECT,		  // 서브효과음 (라인 클리어)
		ATTACKEFFECT,     // 이게 하나 더 필요하네 (공격)
		MAXCHANNEL,		  // 채널의 수를 체크하기 위한 용도
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