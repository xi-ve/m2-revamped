#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		namespace metin_structs
		{
			enum EType
			{
				TYPE_ENEMY,
				TYPE_NPC,
				TYPE_STONE,
				TYPE_WARP,
				TYPE_DOOR,
				TYPE_BUILDING,
				TYPE_PC,
				TYPE_POLY,
				TYPE_HORSE,
				TYPE_GOTO,

				TYPE_OBJECT, // Only For Client
			};
			enum
			{
				FUNC_WAIT,
				FUNC_MOVE,
				FUNC_ATTACK,
				FUNC_COMBO,
				FUNC_MOB_SKILL,
				FUNC_EMOTION,
				FUNC_SKILL = 0x80,
			};
			enum MapFlags
			{
				ATTRIBUTE_BLOCK = (1 << 0),
				ATTRIBUTE_WATER = (1 << 1),
				ATTRIBUTE_BANPK = (1 << 2),
			};
			class Point2D
			{
			public:
				Point2D(float x = 0.f, float y = 0.f) { this->x = x; this->y = y; }

				inline float distance(Point2D* p) { return sqrt((p->x - x) * (p->x - x) + (p->y - y) * (p->y - y)); }
				float angle(Point2D* p);
				inline void absoluteY() { y = abs(y); }

				inline int getIntX() { return (int)x; }
				inline int getIntY() { return (int)y; }

				inline void setIntX(int x) { this->x = (float)x; }
				inline void setIntY(int y) { this->y = (float)y; }

				bool operator==(Point2D& a) { return getIntX() == a.getIntX() && getIntY() == a.getIntY(); }

				float x;
				float y;
			};
		}
	}
}