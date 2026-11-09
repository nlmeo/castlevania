#pragma once
#ifndef __MATH_TYPE_VECTOR3_H__
#define __MATH_TYPE_VECTOR3_H__

#include <iostream>
#include <string>
#include <initializer_list>
#include "Universal.h"

namespace MathType
{
	/**
	 * @brief 三维向量类
	 *
	 * 提供三维向量的基本运算，包括加减乘除、点乘、叉乘、归一化、插值等操作。
	 * 支持float、double等多种数据类型。
	 */
	template<typename T>
	class Vector3
	{
		friend inline std::ostream& operator<<(std::ostream& o, const Vector3<T>& v);
		friend inline std::istream& operator>>(std::istream& i, Vector3<T>& v);

	public:
		T x, y, z;  ///< X、Y、Z坐标

		/** @brief 默认构造函数，初始化为(0, 0, 0) */
		Vector3() : x(0), y(0), z(0) {}

		/**
		 * @brief 带参数构造函数
		 *
		 * @param _x X坐标值
		 * @param _y Y坐标值
		 * @param _z Z坐标值
		 */
		Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

		/**
		 * @brief 列表初始化构造函数
		 *
		 * @param lt 初始化列表
		 */
		Vector3(std::initializer_list<T> lt)
		{
			x = 0;
			y = 0;
			z = 0;
			int i = 0;
			for (auto& item : lt)
			{
				switch (i)
				{
				case 0: x = item; break;
				case 1: y = item; break;
				case 2: z = item; break;
				}
				++i;
			}
		}

		/** @brief 获取右方向向量 (1, 0, 0) */
		constexpr static const Vector3& right()
		{
			static Vector3<T> _right(1, 0, 0);
			return _right;
		}

		/** @brief 获取左方向向量 (-1, 0, 0) */
		constexpr static const Vector3& left()
		{
			static Vector3<T> _left(-1, 0, 0);
			return _left;
		}

		/** @brief 获取上方向向量 (0, 1, 0) */
		constexpr static const Vector3& up()
		{
			static Vector3<T> _up(0, 1, 0);
			return _up;
		}

		/** @brief 获取下方向向量 (0, -1, 0) */
		constexpr static const Vector3& down()
		{
			static Vector3<T> _down(0, -1, 0);
			return _down;
		}

		/** @brief 获取前方向向量 (0, 0, 1) */
		constexpr static const Vector3& forward()
		{
			static Vector3<T> _forward(0, 0, 1);
			return _forward;
		}

		/** @brief 获取后方向向量 (0, 0, -1) */
		constexpr static const Vector3& back()
		{
			static Vector3<T> _back(0, 0, -1);
			return _back;
		}

		/** @brief 获取全1向量 (1, 1, 1) */
		constexpr static const Vector3& one()
		{
			static Vector3<T> _one(1, 1, 1);
			return _one;
		}

		/** @brief 获取零向量 (0, 0, 0) */
		constexpr static const Vector3& zero()
		{
			static Vector3<T> _zero(0, 0, 0);
			return _zero;
		}

		/**
		 * @brief 计算向量的平方模长
		 *
		 * @return 向量长度的平方
		 */
		T sqrMagnitude() const
		{
			return x * x + y * y + z * z;
		}

		/**
		 * @brief 计算向量的模长
		 *
		 * @return 向量的长度
		 */
		T magnitude() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		/**
		 * @brief 获取单位向量（不改变原向量）
		 *
		 * @return 与原向量同方向的单位向量
		 */
		Vector3 normalized() const
		{
			T mag = sqrt(x * x + y * y + z * z);
			if (mag <= kEpsilon<T>())
			{
				return Vector3<T>(0, 0, 0);
			}
			return Vector3(x / mag, y / mag, z / mag);
		}

		/**
		 * @brief 判断是否为零向量
		 *
		 * @return 如果向量模长接近零则返回true
		 */
		bool isZero() const
		{
			return abs(x) <= kEpsilon<T>() && abs(y) <= kEpsilon<T>() && abs(z) <= kEpsilon<T>();
		}

		/**
		 * @brief 原地单位化（改变原向量）
		 *
		 * @return 单位化后的向量引用
		 */
		Vector3 Normalize()
		{
			T mag = sqrt(x * x + y * y + z * z);
			if (mag <= kEpsilon<T>())
			{
				return Vector3<T>(0, 0, 0);
			}
			x /= mag;
			y /= mag;
			z /= mag;
			return *this;
		}

		/**
		 * @brief 计算点积
		 *
		 * @param to 另一个向量
		 * @return 点积结果
		 */
		T Dot(const Vector3<T>& to) const
		{
			return x * to.x + y * to.y + z * to.z;
		}

		/**
		 * @brief 计算叉积（返回向量）
		 *
		 * @param to 另一个向量
		 * @return 叉积结果向量，垂直于两个输入向量
		 */
		Vector3<T> Cross(const Vector3<T>& to) const
		{
			return Vector3<T>(y * to.z - z * to.y, z * to.x - x * to.z, x * to.y - y * to.x);
		}

		/**
		 * @brief 计算两点之间的距离
		 *
		 * @param to 目标点
		 * @return 两点之间的距离
		 */
		T Distance(const Vector3<T>& to) const
		{
			return sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y) + (z - to.z) * (z - to.z));
		}

		/**
		 * @brief 计算两点之间距离的平方
		 *
		 * @param to 目标点
		 * @return 距离的平方
		 */
		T SqrDistance(const Vector3<T>& to) const
		{
			return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y) + (z - to.z) * (z - to.z);
		}

		/**
		 * @brief 计算与另一个向量的夹角（弧度）
		 *
		 * @param to 另一个向量
		 * @return 夹角（弧度，范围[0, π]）
		 */
		T AngleRadian(const Vector3<T>& to) const
		{
			T mags = sqrt(x * x + y * y + z * z) * sqrt(to.x * to.x + to.y * to.y + to.z * to.z);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			T dot = x * to.x + y * to.y + z * to.z;
			return acos(dot / mags);
		}

		/**
		 * @brief 计算与另一个向量的夹角（角度）
		 *
		 * @param to 另一个向量
		 * @return 夹角（角度，范围[0, 180]）
		 */
		T AngleDegree(const Vector3<T>& to) const
		{
			T mags = sqrt(x * x + y * y + z * z) * sqrt(to.x * to.x + to.y * to.y + to.z * to.z);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			T dot = x * to.x + y * to.y + z * to.z;
			return static_cast<T>(R2D(acos(dot / mags)));
		}

		/**
		 * @brief 计算带符号的夹角（弧度）
		 *
		 * @param to 另一个向量
		 * @return 带符号夹角（弧度，范围[-π, π]）
		 */
		T SignedAngleRadian(const Vector3<T>& to) const
		{
			T angle = AngleRadian(to);
			if (Cross(to).y < 0)
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
		T SignedAngleDegree(const Vector3<T>& to) const
		{
			T angle = AngleDegree(to);
			if (Cross(to).y < 0)
			{
				angle = -angle;
			}
			return angle;
		}

		/**
		 * @brief 计算绕指定轴的带符号夹角（弧度）
		 *
		 * @param to 另一个向量
		 * @param axis 旋转轴向量
		 * @return 带符号夹角（弧度）
		 */
		T SignedAngleRadian(const Vector3<T>& to, const Vector3<T>& axis) const
		{
			Vector3<T> v1 = *this - Projection(axis);
			Vector3<T> v2 = to - to.Projection(axis);
			return v1.SignedAngleRadian(v2);
		}

		/**
		 * @brief 计算绕指定轴的带符号夹角（角度）
		 *
		 * @param to 另一个向量
		 * @param axis 旋转轴向量
		 * @return 带符号夹角（角度）
		 */
		T SignedAngleDegree(const Vector3<T>& to, const Vector3<T>& axis) const
		{
			Vector3<T> v1 = *this - Projection(axis);
			Vector3<T> v2 = to - to.Projection(axis);
			return v1.SignedAngleDegree(v2);
		}

		/**
		 * @brief 球面线性插值（SLerp）
		 *
		 * 保持向量长度不变，沿球面进行插值，适合旋转插值。
		 *
		 * @param to 目标向量
		 * @param t 插值参数（0~1）
		 * @return 插值结果
		 */
		Vector3 SLerp(const Vector3<T>& to, float t) const
		{
			float dot = Dot(to);
			dot = dot < -1.0f ? -1.0f : dot > 1.0f ? 1.0f : dot;
			float theta = acos(dot) * t;
			Vector3 RelativeVec = to - (*this) * dot;
			RelativeVec.Normalize();
			return (((*this) * cos(theta)) + (RelativeVec * sin(theta)));
		}

		/**
		 * @brief 线性插值（Lerp）
		 *
		 * @param to 目标向量
		 * @param t 插值参数（0~1）
		 * @return 插值结果
		 */
		Vector3 Lerp(const Vector3<T>& to, float t) const
		{
			float onemt = 1 - t;
			return Vector3<T>(x * onemt + to.x * t, y * onemt + to.y * t, z * onemt + to.z * t);
		}

		/**
		 * @brief 归一化线性插值（NLerp）
		 *
		 * 先进行线性插值，再归一化，适合方向向量插值。
		 *
		 * @param to 目标向量
		 * @param t 插值参数（0~1）
		 * @return 归一化后的插值结果
		 */
		Vector3 NLerp(const Vector3<T>& to, float t) const
		{
			return Lerp(to, t).normalized();
		}

		/**
		 * @brief 计算向量在指定方向上的投影
		 *
		 * @param n 投影方向向量
		 * @return 投影向量
		 */
		Vector3 Projection(const Vector3<T> n) const
		{
			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y + z * n.z;
			T smg = n.x * n.x + n.y * n.y + n.z * n.z;
			T s = dot / smg;
			return Vector3<T>(n.x * s, n.y * s, n.z * s);
		}

		/**
		 * @brief 计算向量在指定方向上投影的单位向量
		 *
		 * @param n 投影方向向量
		 * @return 投影方向的单位向量
		 */
		Vector3 ProjectionDir(const Vector3<T> n) const
		{
			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y + z * n.z;
			T smg = n.x * n.x + n.y * n.y + n.z * n.z;
			T s = dot / smg;
			return Vector3<T>(n.x * s, n.y * s, n.z * s).normalized();
		}

		/**
		 * @brief 投影到指定平面上
		 *
		 * 平面通过原点，参数为平面法线向量。
		 *
		 * @param planeNormal 平面法线向量
		 * @return 投影到平面上的向量
		 */
		Vector3 ProjectOnPlane(Vector3 planeNormal) const
		{
			if (isZero() || planeNormal.isZero()) { return Vector3<T>(0, 0, 0); }
			Vector3 nz = Cross(planeNormal);
			Vector3 n = planeNormal.Cross(nz);
			return Projection(n);
		}

		/**
		 * @brief 投影到XZ平面
		 *
		 * @return 在XZ平面上的投影向量（Y=0）
		 */
		Vector3 ProjectOnXZ() const
		{
			return Vector3<T>(x, 0, z);
		}

		/**
		 * @brief 投影到YZ平面
		 *
		 * @return 在YZ平面上的投影向量（X=0）
		 */
		Vector3 ProjectOnYZ() const
		{
			return Vector3<T>(0, y, z);
		}

		/**
		 * @brief 投影到XY平面
		 *
		 * @return 在XY平面上的投影向量（Z=0）
		 */
		Vector3 ProjectOnXY() const
		{
			return Vector3<T>(x, y, 0);
		}

		/**
		 * @brief 计算反射向量
		 *
		 * @param normal 法线向量
		 * @return 反射后的向量
		 */
		Vector3 Reflect(const Vector3<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector3<T>(0, 0, 0); }
			Vector3 nz = Cross(normal);
			Vector3 n = normal.Cross(nz);
			Vector3 pt = Projection(n) * 2;
			return pt - *this;
		}

		/**
		 * @brief 计算反射向量的方向（单位向量）
		 *
		 * @param normal 法线向量
		 * @return 反射方向的单位向量
		 */
		Vector3 ReflectDir(const Vector3<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector3<T>(0, 0, 0); }
			Vector3 nz = Cross(normal);
			Vector3 n = normal.Cross(nz);
			Vector3 pt = Projection(n) * 2;
			return (pt - *this).normalized();
		}

		/**
		 * @brief 取负运算符
		 *
		 * @return 取负后的向量
		 */
		Vector3 operator -() const
		{
			return Vector3(-x, -y, -z);
		}

		/**
		 * @brief 向量加法运算符
		 *
		 * @param b 另一个向量
		 * @return 相加后的向量
		 */
		Vector3 operator + (const Vector3<T>& b) const
		{
			return Vector3<T>(x + b.x, y + b.y, z + b.z);
		}

		/**
		 * @brief 向量减法运算符
		 *
		 * @param b 另一个向量
		 * @return 相减后的向量
		 */
		Vector3 operator - (const Vector3<T>& b) const
		{
			return Vector3<T>(x - b.x, y - b.y, z - b.z);
		}

		/**
		 * @brief 向量数乘运算符
		 *
		 * @param d 标量
		 * @return 数乘后的向量
		 */
		Vector3 operator * (float d) const
		{
			return Vector3<T>(x * d, y * d, z * d);
		}

		/**
		 * @brief 向量点乘运算符（分量相乘）
		 *
		 * @param b 另一个向量
		 * @return 分量相乘后的向量
		 */
		Vector3 operator *(const Vector3<T>& b) const
		{
			return Vector3<T>(x * b.x, y * b.y, z * b.z);
		}

		/**
		 * @brief 向量数除运算符
		 *
		 * @param d 标量（不能为0）
		 * @return 数除后的向量
		 */
		Vector3 operator /(float d) const
		{
			if (abs(d) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector3<T>(x / d, y / d, z / d);
		}

		/**
		 * @brief 向量加法赋值运算符
		 *
		 * @param b 另一个向量
		 * @return 相加后的向量引用
		 */
		Vector3<T>& operator += (const Vector3<T>& b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
			return *this;
		}

		/**
		 * @brief 向量减法赋值运算符
		 *
		 * @param b 另一个向量
		 * @return 相减后的向量引用
		 */
		Vector3<T>& operator -= (const Vector3<T>& b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
			return *this;
		}

		/**
		 * @brief 向量数乘赋值运算符
		 *
		 * @param d 标量
		 * @return 数乘后的向量引用
		 */
		Vector3<T>& operator *= (float d)
		{
			x *= d;
			y *= d;
			z *= d;
			return *this;
		}

		/**
		 * @brief 向量点乘赋值运算符（分量相乘）
		 *
		 * @param b 另一个向量
		 * @return 分量相乘后的向量引用
		 */
		Vector3<T>& operator *= (const Vector3<T>& b)
		{
			x *= b.x;
			y *= b.y;
			z *= b.z;
			return *this;
		}

		/**
		 * @brief 向量数除赋值运算符
		 *
		 * @param d 标量（不能为0）
		 * @return 数除后的向量引用
		 */
		Vector3<T>& operator /= (float d)
		{
			if (abs(d) <= kEpsilon<T>())
			{
				x = 0; y = 0; z = 0;
			}
			else
			{
				x /= d;
				y /= d;
				z /= d;
			}
			return *this;
		}

		/**
		 * @brief 相等比较运算符
		 *
		 * @param rhs 另一个向量
		 * @return 若相等返回true
		 */
		bool operator ==(const Vector3<T>& rhs) const
		{
			return abs(x - rhs.x) <= kEpsilon<T>() &&
				abs(y - rhs.y) <= kEpsilon<T>() &&
				abs(z - rhs.z) <= kEpsilon<T>();
		}

		/**
		 * @brief 不等比较运算符
		 *
		 * @param rhs 另一个向量
		 * @return 若不相等返回true
		 */
		bool operator !=(const Vector3<T>& rhs) const
		{
			return !(abs(x - rhs.x) <= kEpsilon<T>() &&
				abs(y - rhs.y) <= kEpsilon<T>() &&
				abs(z - rhs.z) <= kEpsilon<T>());
		}

		/**
		 * @brief 小于比较运算符（用于排序）
		 *
		 * @param r 另一个向量
		 * @return 若当前向量字符串表示更小则返回true
		 */
		bool operator < (const Vector3<T>& r) const
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
			sprintf_s(buff, 100, "(%f,%f,%f)", x, y, z);
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
		static Vector3 Lerp(const Vector3<T>& from, const Vector3<T>& to, float t)
		{
			float onemt = 1 - t;
			return Vector3<T>(from.x * onemt + to.x * t, from.y * onemt + to.y * t, from.z * onemt + to.z * t);
		}

	protected:
	private:
	};

	/**
	 * @brief 数左乘向量运算符
	 *
	 * @param d 标量
	 * @param v 向量
	 * @return 数乘后的向量
	 */
	template<typename T>
	inline Vector3<T> operator * (float d, const Vector3<T>& v)
	{
		return Vector3<T>(v.x * d, v.y * d, v.z * d);
	}

	/**
	 * @brief float类型向量输出运算符
	 */
	inline std::ostream& operator<<(std::ostream& o, const Vector3<float>& v)
	{
		o << "(" << v.x << "," << v.y << "," << v.z << ")";
		return o;
	}

	/**
	 * @brief double类型向量输出运算符
	 */
	inline std::ostream& operator<<(std::ostream& o, const Vector3<double>& v)
	{
		o << "(" << v.x << "," << v.y << "," << v.z << ")";
		return o;
	}

	/**
	 * @brief float类型向量输入运算符
	 */
	inline std::istream& operator>>(std::istream& i, Vector3<float>& v)
	{
		i >> v.x >> v.y >> v.z;
		return i;
	}

	/**
	 * @brief double类型向量输入运算符
	 */
	inline std::istream& operator>>(std::istream& i, Vector3<double>& v)
	{
		i >> v.x >> v.y >> v.z;
		return i;
	}

	typedef Vector3<float> vec3f;   ///< float类型三维向量
	typedef Vector3<double> vec3d;  ///< double类型三维向量
}

#endif