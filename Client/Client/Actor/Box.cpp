#include "Box.h"

Box::Box(const Vector2& position)
	: super('B', Color::Green, position)
{	
	// 그릴 때 사용할 정렬 순서 설정
	SetSortingOrder(2);
}
