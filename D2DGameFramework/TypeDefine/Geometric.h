#pragma once
#ifndef __MATH_TYPE_GEOMETRIC_H__
#define __MATH_TYPE_GEOMETRIC_H__
#include <d2d1.h>
#include "Vector2.h"
#include "Universal.h"

namespace MathType
{
	/**
	 * @brief 2D点结构
	 *
	 * 表示二维平面上的点坐标
	 */
	struct Point2F
	{
		float x;  ///< X坐标
		float y;  ///< Y坐标

		/** @brief 默认构造函数，初始化为(0,0) */
		Point2F() : x(0.0f), y(0.0f) {}

		/** @brief 使用坐标值构造点 */
		Point2F(float x, float y) : x(x), y(y) {}
	};

	/**
	 * @brief 矩形结构
	 *
	 * 表示二维平面上的矩形区域
	 */
	struct RectF:public D2D1_RECT_F
	{
		/** @brief 默认构造函数，初始化为空矩形 */
		RectF()
		{
			left = top = right = bottom = 0;
		}
		/** @brief 使用坐标和尺寸构造矩形 */
		RectF(float x, float y, float width, float height)
		{
			left = x;
			right = x+width;
			top = y;
			bottom = y + height;
		}
		float Width() const { return right - left; }
		float Height()const { return bottom - top; }

		/** @brief 获取中心点X坐标 */
		float CenterX() const { return (left + right) * 0.5f; }

		/** @brief 获取中心点Y坐标 */
		float CenterY() const { return (top + bottom) * 0.5f; }

		/** @brief 判断点是否在矩形内 */
		bool Contains(float px, float py) const
		{
			return px >= left && px <= right && py >= top && py <= bottom;
		}


		bool Overlap(const RectF& other,RectF* crossRect = nullptr) const
		{
			if (!(left > other.right || right < other.left || top > other.bottom || bottom < other.top))
			{
				if (crossRect != nullptr)//需要相交区域具体数据
				{
					crossRect->left = max(left, other.left);
					crossRect->top = max(top, other.top);
					crossRect->right = min(right, other.right);
					crossRect->bottom = min(bottom, other.bottom);
				}
				return true;
			}
			return false;
		}
		bool OverlapSphere(float x, float y, float r) const
		{
			// 1. 将圆心限制在矩形范围内，得到矩形上离圆心最近的点
			float closestX = (x < left) ? left :
				(x > right) ? right : x;
			float closestY = (y < top) ? top :
				(y > bottom) ? bottom : y;

			// 2. 计算最近点与圆心的距离平方
			float dx = x - closestX;
			float dy = y - closestY;
			float distSq = dx * dx + dy * dy;

			// 3. 比较距离平方与半径平方
			return distSq <= r * r;
		}
	};

	struct Triangle 
	{
		union 
		{
			struct  
			{
				vec2f p1, p2, p3;
			};
			vec2f ps[3];
		};

		Triangle(const vec2f& v1, const vec2f& v2, const vec2f& v3)
		{
			p1 = v1;
			p2 = v2;
			p3 = v3;
		}

		void Set(const vec2f& v1, const vec2f& v2, const vec2f& v3)
		{
			p1 = v1;
			p2 = v2;
			p3 = v3;
		}

		bool PointInTriangle(const vec2f& p) const
		{
			bool a = (p2 - p1).Cross(p - p1) > 0;
			bool b = (p3 - p2).Cross(p - p2) > 0;
			bool c = (p1 - p3).Cross(p - p3) > 0;
			return a == b && b == c;
		}
	};
}

#endif