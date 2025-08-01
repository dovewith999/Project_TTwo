#include "Ground.h"

Ground::Ground(const Vector2& position)
	: super('.', Color::White, position)
{	
	// 그릴 때 사용할 정렬 순서 설정
	SetSortingOrder(0);
}
