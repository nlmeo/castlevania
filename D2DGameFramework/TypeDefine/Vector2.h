#pragma once
#ifndef __MATH_TYPE_VECTOR2_H__
#define __MATH_TYPE_VECTOR2_H__

#include <iostream>
#include <string>
#include <initializer_list>
#include "Universal.h"

namespace MathType
{
	/**
	 * @brief 二维向量类
	 *
	 * 提供二维向量的基本运算，包括加减乘除、点乘、叉乘、归一化、插值等操作。
	 * 支持float、double、int等多种数据类型。
	 */
	template<typename T>
	class Vector2
	{
		friend inline std::ostream& operator<<(std::ostream& o, const Vector2<T>& v);
		friend inline std::istream& operator>>(std::istream& i, Vector2<T>& v);

	public:
		/**
		 * @brief 数据联合体
		 *
		 * 支持多种访问方式：x/y坐标或u/v纹理坐标
		 */
		union
		{
			struct
			{
				T x, y;  ///< X和Y坐标
			};
			struct
			{
				T u, v;  ///< U和V纹理坐标
			};
		};

		/** @brief 默认构造函数，初始化为(0, 0) */
		Vector2() : x(0), y(0) {}

		/**
		 * @brief 带参数构造函数
		 *
		 * @param _x X坐标值
		 * @param _y Y坐标值
		 */
		Vector2(T _x, T _y) : x(_x), y(_y) {}

		/**
		 * @brief 列表初始化构造函数
		 *
		 * @param lt 初始化列表
		 */
		Vector2(std::initializer_list<T> lt)
		{
			x = 0;
			y = 0;
			int i = 0;
			for (auto& item : lt)
			{
				switch (i)
				{
				case 0: x = item; break;
				case 1: y = item; break;
				}
				++i;
			}
		}

		/** @brief 获取右方向向量 (1, 0) */
		constexpr static const Vector2& right()
		{
			static Vector2<T> _right(1, 0);
			return _right;
		}

		/** @brief 获取左方向向量 (-1, 0) */
		constexpr static const Vector2& left()
		{
			static Vector2<T> _left(-1, 0);
			return _left;
		}

		/** @brief 获取上方向向量 (0, 1) */
		constexpr static const Vector2& up()
		{
			static Vector2<T> _up(0, 1);
			return _up;
		}

		/** @brief 获取下方向向量 (0, -1) */
		constexpr static const Vector2& down()
		{
			static Vector2<T> _down(0, -1);
			return _down;
		}

		/** @brief 获取全1向量 (1, 1) */
		constexpr static const Vector2& one()
		{
			static Vector2<T> _one(1, 1);
			return _one;
		}

		/** @brief 获取零向量 (0, 0) */
		constexpr static const Vector2& zero()
		{
			static Vector2<T> _zero(0, 0);
			return _zero;
		}

		/**
		 * @brief 获取随机方向单位向量
		 *
		 * @return 随机方向的单位向量
		 */
		constexpr static const Vector2 random()
		{
			Vector2<T> _ret;
			T rangle = D2R(rand() % 1000 * 0.360360f);
			_ret.x = -sin(rangle);
			_ret.y = cos(rangle);
			return _ret;
		}

		/**
		 * @brief 计算向量的平方模长
		 *
		 * @return 向量长度的平方
		 */
		T sqrMagnitude() const
		{
			return x * x + y * y;
		}

		/**
		 * @brief 计算向量的模长
		 *
		 * @return 向量的长度
		 */
		T magnitude() const
		{
			return sqrt(x * x + y * y);
		}

		/**
		 * @brief 获取单位向量（不改变原向量）
		 *
		 * @return 与原向量同方向的单位向量
		 */
		Vector2 normalized() const
		{
			T mag = sqrt(x * x + y * y);
			if (mag <= kEpsilon<T>())
			{
				return Vector2<T>(0, 0);
			}
			return Vector2(x / mag, y / mag);
		}

		/**
		 * @brief 判断是否为零向量
		 *
		 * @return 如果向量模长接近零则返回true
		 */
		bool isZero() const
		{
			return abs(x) <= kEpsilon<T>() && abs(y) <= kEpsilon<T>();
		}

		/**
		 * @brief 原地单位化（改变原向量）
		 *
		 * @return 单位化后的向量引用
		 */
		Vector2 Normalize()
		{
			T mag = sqrt(x * x + y * y);
			if (mag <= kEpsilon<T>())
			{
				return Vector2<T>(0, 0);
			}
			x /= mag;
			y /= mag;
			return *this;
		}

		/**
		 * @brief 计算点积
		 *
		 * @param to 另一个向量
		 * @return 点积结果
		 */
		T Dot(const Vector2<T>& to) const
		{
			return x * to.x + y * to.y;
		}

		/**
		 * @brief 计算叉积（返回标量）
		 *
		 * @param to 另一个向量
		 * @return 叉积结果（标量，代表两个向量组成的平行四边形面积）
		 */
		T Cross(const Vector2<T>& to) const
		{
			return x * to.y - to.x * y;
		}

		/**
		 * @brief 计算两点之间的距离
		 *
		 * @param to 目标点
		 * @return 两点之间的距离
		 */
		T Distance(const Vector2<T>& to) const
		{
			return sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y));
		}

		/**
		 * @brief 计算两点之间距离的平方
		 *
		 * @param to 目标点
		 * @return 距离的平方
		 */
		T SqrDistance(const Vector2<T>& to) const
		{
			return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y);
		}

		/**
		 * @brief 获取正交向量
		 *
		 * 对于正Y轴向上的2D坐标系，结果始终沿逆时针方向旋转90度。
		 *
		 * @return 正交向量
		 */
		Vector2 Perpendicular() const
		{
			if (abs(x) <= kEpsilon<T>())
			{
				return Vector2(-y, x);
			}
			return Vector2(y, -x);
		}

		/**
		 * @brief 计算与另一个向量的夹角（弧度）
		 *
		 * @param to 另一个向量
		 * @return 夹角（弧度，范围[0, π]）
		 */
		T AngleRadian(const Vector2<T>& to) const
		{
			T mags = sqrt(x * x + y * y) * sqrt(to.x * to.x + to.y * to.y);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			T dot = x * to.x + y * to.y;
			return acos(dot / mags);
		}

		/**
		 * @brief 计算与另一个向量的夹角（角度）
		 *
		 * @param to 另一个向量
		 * @return 夹角（角度，范围[0, 180]）
		 */
		T AngleDegree(const Vector2<T>& to) const
		{
			T mags = sqrt(x * x + y * y) * sqrt(to.x * to.x + to.y * to.y);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			T dot = x * to.x + y * to.y;
			return static_cast<T>(R2D(acos(dot / mags)));
		}

		/**
		 * @brief 计算带符号的夹角（弧度）
		 *
		 * @param to 另一个向量
		 * @return 带符号夹角（弧度，范围[-π, π]）
		 */
		T SignedAngleRadian(const Vector2<T>& to) const
		{
			T cross = x * to.y - to.x * y;
			T angle = AngleRadian(to);
			if (cross < 0)
			{
				angle = -angle;
			}
			return angle;
		}

		/**
		 * @brief 计算带符号的夹角（角度）
		 *
		 * @param to 另一个向量
		 * @return 带符号夹角（角度，范围[-180, 180]）
		 */
		T SignedAngleDegree(const Vector2<T>& to) const
		{
			T cross = x * to.y - to.x * y;
			T angle = AngleDegree(to);
			if (cross < 0)
			{
				angle = -angle;
			}
			return angle;
		}

		/**
		 * @brief 线性插值（Lerp）
		 *
		 * @param to 目标向量
		 * @param t 插值参数（0~1）
		 * @return 插值结果
		 */
		Vector2 Lerp(const Vector2<T>& to, float t)
		{
			float onemt = 1 - t;
			return Vector2<T>(x * onemt + to.x * t, y * onemt + to.y * t);
		}

		/**
		 * @brief 计算向量在指定方向上的投影
		 *
		 * @param n 投影方向向量
		 * @return 投影向量
		 */
		Vector2 Projection(const Vector2<T> n) const
		{
			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y;
			T smg = n.x * n.x + n.y * n.y;
			T s = dot / smg;
			return Vector2<T>(n.x * s, n.y * s);
		}

		/**
		 * @brief 计算向量在指定方向上投影的单位向量
		 *
		 * @param n 投影方向向量
		 * @return 投影方向的单位向量
		 */
		Vector2 ProjectionDir(const Vector2<T> n) const
		{
			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y;
			T smg = n.x * n.x + n.y * n.y;
			T s = dot / smg;
			return Vector2<T>(n.x * s, n.y * s).normalized();
		}

		/**
		 * @brief 计算反射向量
		 *
		 * @param normal 法线向量
		 * @return 反射后的向量
		 */
		Vector2 Reflect(const Vector2<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector2<T>(0, 0); }
			Vector2 n = normal.Perpendicular();
			Vector2 pt = Projection(n) * 2;
			return pt - *this;
		}

		/**
		 * @brief 计算反射向量的方向（单位向量）
		 *
		 * @param normal 法线向量
		 * @return 反射方向的单位向量
		 */
		Vector2 ReflectDir(const Vector2<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector2<T>(0, 0); }
			Vector2 n = normal.Perpendicular();
			Vector2 pt = Projection(n) * 2;
			return (pt - *this).normalized();
		}

		/**
		 * @brief 取负运算符
		 *
		 * @return 取负后的向量
		 */
		Vector2 operator -() const
		{
			return Vector2(-x, -y);
		}

		/**
		 * @brief 向量加法运算符
		 *
		 * @param b 另一个向量
		 * @return 相加后的向量
		 */
		Vector2 operator + (const Vector2<T>& b) const
		{
			return Vector2<T>(x + b.x, y + b.y);
		}

		/**
		 * @brief 向量减法运算符
		 *
		 * @param b 另一个向量
		 * @return 相减后的向量
		 */
		Vector2 operator - (const Vector2<T>& b) const
		{
			return Vector2<T>(x - b.x, y - b.y);
		}

		/**
		 * @brief 向量数乘运算符
		 *
		 * @param d 标量
		 * @return 数乘后的向量
		 */
		Vector2 operator * (float d) const
		{
			return Vector2<T>(x * d, y * d);
		}

		/**
		 * @brief 向量点乘运算符（分量相乘）
		 *
		 * @param b 另一个向量
		 * @return 分量相乘后的向量
		 */
		Vector2 operator *(const Vector2<T>& b) const
		{
			return Vector2<T>(x * b.x, y * b.y);
		}

		/**
		 * @brief 向量数除运算符
		 *
		 * @param d 标量（不能为0）
		 * @return 数除后的向量
		 */
		Vector2 operator /(float d) const
		{
			if (abs(d) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector2<T>(x / d, y / d);
		}

		/**
		 * @brief 向量分量相除运算符
		 *
		 * @param b 另一个向量（分量不能为0）
		 * @return 分量相除后的向量
		 */
		Vector2 operator /(const Vector2<T>& b) const
		{
			if (abs(b.x) <= kEpsilon<T>() || abs(b.y) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector2<T>(x / b.x, y / b.y);
		}

		/**
		 * @brief 向量加法赋值运算符
		 *
		 * @param b 另一个向量
		 * @return 相加后的向量引用
		 */
		Vector2<T>& operator += (const Vector2<T>& b)
		{
			x += b.x;
			y += b.y;
			return *this;
		}

		/**
		 * @brief 向量减法赋值运算符
		 *
		 * @param b 另一个向量
		 * @return 相减后的向量引用
		 */
		Vector2<T>& operator -= (const Vector2<T>& b)
		{
			x -= b.x;
			y -= b.y;
			return *this;
		}

		/**
		 * @brief 向量数乘赋值运算符
		 *
		 * @param d 标量
		 * @return 数乘后的向量引用
		 */
		Vector2<T>& operator *= (float d)
		{
			x *= d;
			y *= d;
			return *this;
		}

		/**
		 * @brief 向量点乘赋值运算符（分量相乘）
		 *
		 * @param b 另一个向量
		 * @return 分量相乘后的向量引用
		 */
		Vector2<T>& operator *= (const Vector2<T>& b)
		{
			x *= b.x;
			y *= b.y;
			return *this;
		}

		/**
		 * @brief 向量数除赋值运算符
		 *
		 * @param d 标量（不能为0）
		 * @return 数除后的向量引用
		 */
		Vector2<T>& operator /= (float d)
		{
			if (abs(d) <= kEpsilon<T>())
			{
				x = 0; y = 0;
			}
			else
			{
				x /= d;
				y /= d;
			}
			return *this;
		}

		/**
		 * @brief 向量分量相除赋值运算符
		 *
		 * @param b 另一个向量（分量不能为0）
		 * @return 分量相除后的向量引用
		 */
		Vector2<T>& operator /= (const Vector2<T>& b)
		{
			if (abs(b.x) <= kEpsilon<T>() || abs(b.y) <= kEpsilon<T>())
			{
				x = 0; y = 0;
			}
			else
			{
				x /= b.x;
				y /= b.y;
			}
			return *this;
		}

		/**
		 * @brief 相等比较运算符
		 *
		 * @param rhs 另一个向量
		 * @return 若相等返回true
		 */
		bool operator ==(const Vector2<T>& rhs) const
		{
			return abs(x - rhs.x) <= kEpsilon<T>() && abs(y - rhs.y) <= kEpsilon<T>();
		}

		/**
		 * @brief 不等比较运算符
		 *
		 * @param rhs 另一个向量
		 * @return 若不相等返回true
		 */
		bool operator !=(const Vector2<T>& rhs) const
		{
			return !(abs(x - rhs.x) <= kEpsilon<T>() && abs(y - rhs.y) <= kEpsilon<T>());
		}

		/**
		 * @brief 小于比较运算符（用于排序）
		 *
		 * @param r 另一个向量
		 * @return 若当前向量字符串表示更小则返回true
		 */
		bool operator < (const Vector2<T>& r) const
		{
			return ToString() < r.ToString();
		}

		/**
		 * @brief 转换为字符串表示
		 *
		 * @return 向量的字符串表示形式
		 */
		std::string ToString() const
		{
			char buff[100] = {};
			sprintf_s(buff, 100, "(%f,%f)", x, y);
			return buff;
		}

		/**
		 * @brief 静态线性插值方法
		 *
		 * @param from 起始向量
		 * @param to 目标向量
		 * @param t 插值参数（0~1）
		 * @return 插值结果
		 */
		static Vector2<T> Lerp(const Vector2<T>& from, const Vector2<T>& to, float t)
		{
			return from + (to - from) * t;
		}
	};

	/**
	 * @brief 数左乘向量运算符
	 *
	 * @param d 标量
	 * @param v 向量
	 * @return 数乘后的向量
	 */
	template<typename T>
	inline Vector2<T> operator * (float d, const Vector2<T>& v)
	{
		return Vector2<T>(v.x * d, v.y * d);
	}

	/**
	 * @brief float类型向量输出运算符
	 */
	inline std::ostream& operator<<(std::ostream& o, const Vector2<float>& v)
	{
		o << "(" << v.x << "," << v.y << ")";
		return o;
	}

	/**
	 * @brief double类型向量输出运算符
	 */
	inline std::ostream& operator<<(std::ostream& o, const Vector2<double>& v)
	{
		o << "(" << v.x << "," << v.y << ")";
		return o;
	}

	/**
	 * @brief float类型向量输入运算符
	 */
	inline std::istream& operator>>(std::istream& i, Vector2<float>& v)
	{
		i >> v.x >> v.y;
		return i;
	}

	/**
	 * @brief double类型向量输入运算符
	 */
	inline std::istream& operator>>(std::istream& i, Vector2<double>& v)
	{
		i >> v.x >> v.y;
		return i;
	}

	typedef Vector2<float> vec2f;   ///< float类型二维向量
	typedef Vector2<double> vec2d;  ///< double类型二维向量
	typedef Vector2<int> vec2i;     ///< int类型二维向量
}

#endif
