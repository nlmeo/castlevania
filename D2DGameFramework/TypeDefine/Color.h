#pragma once
#ifndef __COLOR_TYPE_H__
#define __COLOR_TYPE_H__
#include "Vector3.h"
namespace ColorType
{
	//定义的32位的颜色数据
	typedef struct stColor32
	{
		union
		{
			unsigned char cc[4];
			int color32;
			struct
			{
				unsigned char r, g, b, a;
			};
		};

		/*
		#define RGB(r,g,b)
		((COLORREF)(((BYTE)(r)|
		((WORD)((BYTE)(g))<<8))|
		(((DWORD)(BYTE)(b))<<16)))
		*/
		stColor32() :color32(0) {}//成员初始化列表
		//可以实现整数的赋值
		stColor32(const int& that) :color32(that)
		{
			r = 0x000000ff & that;
			g = (0x0000ff00 & that) >> 8;
			b = (0x00ff0000 & that) >> 16;
			a = 255;
		}
		//重载强制类型转换，可以让我们的颜色转换到对应的其他类型
		operator int() const
		{
			unsigned char color[4] = { r,g,b,0 };
			return *((int*)color);
			//return *((int*)this);
		}


		//用vec3构造颜色结构体
		stColor32(const MathType::vec3f& v)
		{
			r = (unsigned char)(CLIMP_2_ONE(v.x) * 255);
			g = (unsigned char)(CLIMP_2_ONE(v.y) * 255);
			b = (unsigned char)(CLIMP_2_ONE(v.z) * 255);
			a = 255;
		}

		//将颜色结构体强制类型转换到vec3
		operator MathType::vec3f() const
		{
			return MathType::vec3f(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		stColor32(int _a, int _r, int _g, int _b)
		{
			a = (unsigned char)_a;
			r = (unsigned char)_r;
			g = (unsigned char)_g;
			b = (unsigned char)_b;
		}

		stColor32(int _r, int _g, int _b)
		{
			a = 255;
			r = (unsigned char)_r;
			g = (unsigned char)_g;
			b = (unsigned char)_b;
		}

		void Set(int _a, int _r, int _g, int _b)
		{
			a = (unsigned char)_a;
			r = (unsigned char)_r;
			g = (unsigned char)_g;
			b = (unsigned char)_b;
		}

		void Set(int _r, int _g, int _b)
		{
			a = 255;
			r = (unsigned char)_r;
			g = (unsigned char)_g;
			b = (unsigned char)_b;
		}

		//求颜色的反色
		stColor32 InvColor()
		{
			stColor32 ret(255 - r, 255 - g, 255 - b);
			return ret;
		}

		static stColor32 Random()
		{
			static stColor32 color;
			color.Set(rand() % 256, rand() % 256, rand() % 256);
			return color;
		}

		static stColor32 Black() { return 0x00000000; }
		static stColor32 White() { return 0x00ffffff; }
		static stColor32 Gray() { return 0x007f7f7f; }
		
		static stColor32 Red() { return 0x000000ff; }
		static stColor32 Green() { return 0x0000ff00; }
		static stColor32 Blue() { return 0x00ff0000; }

		static stColor32 Yellow() { return 0x0000ffff; }
		static stColor32 Cyan() { return 0x00ffff00; }
		static stColor32 Purple() { return 0x00ff00ff; }

		static stColor32 Pink() { return 0x00c1b6ff; }
		static stColor32 Gold() { return 0x0000d7ff; }
	}COLOR32, * LPCOLOR32, ** LPPCOLOR32;


	/**
 * @brief 颜色结构
 *
 * 表示RGBA颜色，每个分量范围为0.0f到1.0f
 */
	typedef struct stColorF
	{
		float r;  ///< 红色分量 (0.0f - 1.0f)
		float g;  ///< 绿色分量 (0.0f - 1.0f)
		float b;  ///< 蓝色分量 (0.0f - 1.0f)
		float a;  ///< 透明度分量 (0.0f - 1.0f)

		/** @brief 默认构造函数，初始化为黑色不透明 */
		stColorF() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

		/** @brief 使用RGBA分量构造颜色 */
		stColorF(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

		bool Equal(const stColorF& other) const
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		/** @brief 获取白色 */
		static stColorF White() { return stColorF(1.0f, 1.0f, 1.0f, 1.0f); }

		/** @brief 获取黑色 */
		static stColorF Black() { return stColorF(0.0f, 0.0f, 0.0f, 1.0f); }

		/** @brief 获取灰色 */
		static stColorF Gray() { return stColorF(0.5f, 0.5f, 0.5f, 1.0f); }

		/** @brief 获取红色 */
		static stColorF Red() { return stColorF(1.0f, 0.0f, 0.0f, 1.0f); }

		/** @brief 获取绿色 */
		static stColorF Green() { return stColorF(0.0f, 1.0f, 0.0f, 1.0f); }

		/** @brief 获取蓝色 */
		static stColorF Blue() { return stColorF(0.0f, 0.0f, 1.0f, 1.0f); }

		/** @brief 获取黄色 */
		static stColorF Yellow() { return stColorF(1.0f, 1.0f, 0.0f, 1.0f); }

		/** @brief 获取青色 */
		static stColorF Cyan() { return stColorF(0.0f, 1.0f, 1.0f, 1.0f); }

		/** @brief 获取紫色 */
		static stColorF Purple() { return stColorF(1.0f, 0.0f, 1.0f, 1.0f); }

		/** @brief 获取粉色 */
		static stColorF Pink() { return stColorF(1.0f, 0.71f, 0.75f, 1.0f); }
		/** @brief 获取金色 */
		static stColorF Gold() { return stColorF(1.0f, 0.843f, 0.0f, 1.0f); }



	}COLORF,*LPCOLORF;
}
#endif