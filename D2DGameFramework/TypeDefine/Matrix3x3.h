#pragma once
#ifndef __MATH_TYPE_MATRIX_3x3_H__
#define __MATH_TYPE_MATRIX_3x3_H__

#include "Universal.h"
#include "Vector2.h"
#include "Vector3.h"

namespace MathType
{
	/**
	 * @brief 3x3变换矩阵类
	 *
	 * 提供3x3矩阵的基本运算，包括矩阵乘法、行列式、逆矩阵、转置等操作。
	 * 支持2D变换：平移、旋转、缩放、错切、镜像等。
	 */
	template<typename T>
	class Matrix3x3
	{
	public:
		/**
		 * @brief 数据联合体
		 *
		 * 支持多种访问方式：
		 * - 单个元素：_11, _12, _13, _21, ...
		 * - 二维数组：m33[3][3]
		 * - 一维数组：m9[9]
		 */
		union
		{
			struct
			{
				T _11, _12, _13;
				T _21, _22, _23;
				T _31, _32, _33;
			};

			T m33[3][3];   ///< 3x3二维数组形式
			T m9[9];       ///< 9元素一维数组形式
		};

		/**
		 * @brief 设置矩阵所有元素
		 *
		 * @param __11 第一行第一列元素
		 * @param __12 第一行第二列元素
		 * @param __13 第一行第三列元素
		 * @param __21 第二行第一列元素
		 * @param __22 第二行第二列元素
		 * @param __23 第二行第三列元素
		 * @param __31 第三行第一列元素
		 * @param __32 第三行第二列元素
		 * @param __33 第三行第三列元素
		 */
		void Set(T __11, T __12, T __13,
			T __21, T __22, T __23,
			T __31, T __32, T __33)
		{
			_11 = __11; _12 = __12; _13 = __13;
			_21 = __21; _22 = __22; _23 = __23;
			_31 = __31; _32 = __32; _33 = __33;
		}

		/**
		 * @brief 使用一维数组设置矩阵
		 *
		 * @param arr 9元素数组
		 */
		void Set(T arr[9])
		{
			memcpy(m9, arr, sizeof(T) * 9);
		}

		/**
		 * @brief 使用二维数组设置矩阵
		 *
		 * @param arr 3x3二维数组
		 */
		void Set(T arr[3][3])
		{
			memcpy(m33, arr, sizeof(T) * 9);
		}

		/**
		 * @brief 将矩阵设为单位矩阵
		 */
		void Identity()
		{
			_12 = _13 = _21 = _23 = _31 = _32 = 0;
			_11 = _22 = _33 = 1;
		}

		/**
		 * @brief 计算行列式
		 *
		 * @return 行列式值
		 */
		T Det()
		{
			return _11 * (_22 * _33 - _23 * _32) +
				_12 * (_23 * _31 - _21 * _33) +
				_13 * (_21 * _32 - _22 * _31);
		}

		/**
		 * @brief 计算余子式
		 *
		 * 余子式是去掉第i行第j列后的2x2矩阵的行列式。
		 *
		 * @param i 行索引（1-3）
		 * @param j 列索引（1-3）
		 * @return 余子式值
		 */
		T Cofactor(int i, int j)
		{
			int id = i * 10 + j;
			T m[4];
			switch (id)
			{
			case 11: {m[0] = _22; m[1] = _23; m[2] = _32; m[3] = _33; } break;
			case 12: {m[0] = _21; m[1] = _23; m[2] = _31; m[3] = _33; } break;
			case 13: {m[0] = _21; m[1] = _22; m[2] = _31; m[3] = _32; } break;
			case 21: {m[0] = _12; m[1] = _13; m[2] = _32; m[3] = _33; } break;
			case 22: {m[0] = _11; m[1] = _13; m[2] = _31; m[3] = _33; } break;
			case 23: {m[0] = _11; m[1] = _12; m[2] = _31; m[3] = _32; } break;
			case 31: {m[0] = _12; m[1] = _13; m[2] = _22; m[3] = _23; } break;
			case 32: {m[0] = _11; m[1] = _13; m[2] = _21; m[3] = _23; } break;
			case 33: {m[0] = _11; m[1] = _12; m[2] = _21; m[3] = _22; } break;
			default: {m[0] = _22; m[1] = _23; m[2] = _32; m[3] = _33; }
			}
			return m[0] * m[3] - m[1] * m[2];
		}

		/**
		 * @brief 计算伴随矩阵
		 *
		 * 伴随矩阵是余子式矩阵的转置。
		 *
		 * @return 伴随矩阵
		 */
		Matrix3x3<T> Adj()
		{
			Matrix3x3<T> adj;
			adj.Set(
				Cofactor(1, 1), Cofactor(2, 1), Cofactor(3, 1),
				Cofactor(1, 2), Cofactor(2, 2), Cofactor(3, 2),
				Cofactor(1, 3), Cofactor(2, 3), Cofactor(3, 3));
			return adj;
		}

		/**
		 * @brief 求逆矩阵
		 *
		 * 使用伴随矩阵法求逆：M^(-1) = adj(M) / det(M)
		 *
		 * @param inverseMatrix 输出的逆矩阵
		 * @return 成功返回true，失败（行列式为0）返回false
		 */
		bool Inverse(Matrix3x3<T>& inverseMatrix)
		{
			T det = Det();
			if (abs(det) < kEpsilon<T>())
			{
				return false;
			}

			inverseMatrix.Set(
				Cofactor(1, 1), Cofactor(2, 1), Cofactor(3, 1),
				Cofactor(1, 2), Cofactor(2, 2), Cofactor(3, 2),
				Cofactor(1, 3), Cofactor(2, 3), Cofactor(3, 3));

			inverseMatrix /= det;
			return true;
		}

		/**
		 * @brief 转置矩阵（原地修改）
		 *
		 * @return 转置后的矩阵引用
		 */
		Matrix3x3<T>& Transpos()
		{
			T temp = _12; _12 = _21; _21 = temp;
			temp = _13; _13 = _31; _31 = temp;
			temp = _23; _23 = _32; _32 = temp;
			return *this;
		}

		/**
		 * @brief 设置旋转矩阵（弧度）
		 *
		 * @param angle 旋转角度（弧度）
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetRotateRadian(T angle)
		{
			T c = cos(angle);
			T s = sin(angle);
			_13 = _23 = _31 = _32 = 0;
			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = 1;
			return *this;
		}

		/**
		 * @brief 设置旋转矩阵（角度）
		 *
		 * @param angle 旋转角度（角度）
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetRotateDegree(T angle)
		{
			angle = static_cast<T>(D2R(angle));
			T c = cos(angle);
			T s = sin(angle);
			_13 = _23 = _31 = _32 = 0;
			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = 1;
			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上旋转（弧度）
		 *
		 * @param angle 旋转角度（弧度）
		 * @return 旋转后的矩阵引用
		 */
		Matrix3x3<T>& RotateRadian(T angle)
		{
			Matrix3x3<T> mat;
			mat.SetRotateRadian(angle);
			*this *= mat;
			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上旋转（角度）
		 *
		 * @param angle 旋转角度（角度）
		 * @return 旋转后的矩阵引用
		 */
		Matrix3x3<T>& RotateDegree(T angle)
		{
			Matrix3x3<T> mat;
			mat.SetRotateDegree(angle);
			*this *= mat;
			return *this;
		}

		/**
		 * @brief 设置缩放矩阵
		 *
		 * @param sx X方向缩放因子
		 * @param sy Y方向缩放因子
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetScale(T sx, T sy)
		{
			_12 = _13 = _21 = _23 = _31 = _32 = 0;
			_33 = 1;
			_11 = sx;
			_22 = sy;
			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上缩放
		 *
		 * @param sx X方向缩放因子
		 * @param sy Y方向缩放因子
		 * @return 缩放后的矩阵引用
		 */
		Matrix3x3<T>& Scale(T sx, T sy)
		{
			Matrix3x3<T> mat;
			mat.SetScale(sx, sy);
			return *this *= mat;
		}

		/**
		 * @brief 设置沿任意轴的缩放矩阵
		 *
		 * @param n 缩放轴方向向量
		 * @param k 缩放因子
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetAxisScale(const Vector2<T>& n, T k)
		{
			T km1 = k - 1;
			T nx2km1 = n.x * n.x * km1;
			T ny2km1 = n.y * n.y * km1;
			T nxykm1 = n.x * n.y * km1;

			_11 = 1 + nx2km1; _12 = nxykm1;
			_21 = nxykm1;     _22 = 1 + ny2km1;

			_13 = _23 = _31 = _32 = 0;
			_33 = 1;

			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上沿任意轴缩放
		 *
		 * @param n 缩放轴方向向量
		 * @param k 缩放因子
		 * @return 缩放后的矩阵引用
		 */
		Matrix3x3<T>& AxisScale(const Vector2<T>& n, T k)
		{
			Matrix3x3<T> mat;
			mat.SetAxisScale(n, k);
			return *this *= mat;
		}

		/**
		 * @brief 设置平移矩阵
		 *
		 * @param dx X方向平移量
		 * @param dy Y方向平移量
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetTranslate(T dx, T dy)
		{
			_12 = _13 = _21 = _23 = 0;
			_11 = _22 = _33 = 1;
			_31 = dx;
			_32 = dy;
			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上平移
		 *
		 * @param dx X方向平移量
		 * @param dy Y方向平移量
		 * @return 平移后的矩阵引用
		 */
		Matrix3x3<T>& Translate(T dx, T dy)
		{
			Matrix3x3<T> mat;
			mat.SetTranslate(dx, dy);
			return *this *= mat;
		}

		/**
		 * @brief 设置错切变换矩阵
		 *
		 * @param sx X方向错切因子
		 * @param sy Y方向错切因子
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetShearing(T sx, T sy)
		{
			_13 = _23 = _31 = _32 = 0;
			_11 = _22 = _33 = 1;
			_21 = sx;
			_12 = sy;
			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上错切
		 *
		 * @param sx X方向错切因子
		 * @param sy Y方向错切因子
		 * @return 错切后的矩阵引用
		 */
		Matrix3x3<T>& Shearing(T sx, T sy)
		{
			Matrix3x3<T> mat;
			mat.SetShearing(sx, sy);
			return *this *= mat;
		}

		/**
		 * @brief 设置镜像变换矩阵
		 *
		 * @param mirror_axis 镜像轴向量
		 * @return 设置后的矩阵引用
		 */
		Matrix3x3<T>& SetMirroring(const Vector2<T>& mirror_axis)
		{
			Vector2<T> n = mirror_axis.Perpendicular().normalized();

			_11 = 1 - 2 * n.x * n.x;
			_12 = -2 * n.x * n.y;
			_21 = _12;
			_22 = 1 - 2 * n.y * n.y;

			_13 = _23 = _31 = _32 = 0;
			_33 = 1;

			return *this;
		}

		/**
		 * @brief 在当前矩阵基础上镜像
		 *
		 * @param mirror_axis 镜像轴向量
		 * @return 镜像后的矩阵引用
		 */
		Matrix3x3<T>& Mirroring(const Vector2<T>& mirror_axis)
		{
			Matrix3x3<T> mat;
			mat.SetMirroring(mirror_axis);
			return *this *= mat;
		}

		/**
		 * @brief 矩阵数乘运算符
		 *
		 * @param k 标量
		 * @return 数乘后的矩阵
		 */
		Matrix3x3<T> operator * (T k) const
		{
			Matrix3x3<T> mat;
			mat._11 = _11 * k; mat._12 = _12 * k; mat._13 = _13 * k;
			mat._21 = _21 * k; mat._22 = _22 * k; mat._23 = _23 * k;
			mat._31 = _31 * k; mat._32 = _32 * k; mat._33 = _33 * k;
			return mat;
		}

		/**
		 * @brief 矩阵数除运算符
		 *
		 * @param k 标量（不能为0）
		 * @return 数除后的矩阵
		 */
		Matrix3x3<T> operator / (T k) const
		{
			Matrix3x3<T> mat;
			if (abs(k) < kEpsilon<T>())
			{
				mat.Identity();
			}
			else
			{
				mat._11 = _11 / k; mat._12 = _12 / k; mat._13 = _13 / k;
				mat._21 = _21 / k; mat._22 = _22 / k; mat._23 = _23 / k;
				mat._31 = _31 / k; mat._32 = _32 / k; mat._33 = _33 / k;
			}
			return mat;
		}

		/**
		 * @brief 矩阵数乘赋值运算符
		 *
		 * @param k 标量
		 * @return 数乘后的矩阵引用
		 */
		Matrix3x3<T>& operator *= (T k)
		{
			_11 *= k; _12 *= k; _13 *= k;
			_21 *= k; _22 *= k; _23 *= k;
			_31 *= k; _32 *= k; _33 *= k;
			return *this;
		}

		/**
		 * @brief 矩阵数除赋值运算符
		 *
		 * @param k 标量（不能为0）
		 * @return 数除后的矩阵引用
		 */
		Matrix3x3<T>& operator /= (T k)
		{
			if (abs(k) < kEpsilon<T>())
			{
				Identity();
			}
			else
			{
				_11 /= k; _12 /= k; _13 /= k;
				_21 /= k; _22 /= k; _23 /= k;
				_31 /= k; _32 /= k; _33 /= k;
			}
			return *this;
		}

		/**
		 * @brief 矩阵乘法运算符
		 *
		 * @param m 另一个矩阵
		 * @return 矩阵相乘结果
		 */
		Matrix3x3<T> operator * (const Matrix3x3<T>& m) const
		{
			Matrix3x3<T> mat;
			mat._11 = _11 * m._11 + _12 * m._21 + _13 * m._31;
			mat._12 = _11 * m._12 + _12 * m._22 + _13 * m._32;
			mat._13 = _11 * m._13 + _12 * m._23 + _13 * m._33;

			mat._21 = _21 * m._11 + _22 * m._21 + _23 * m._31;
			mat._22 = _21 * m._12 + _22 * m._22 + _23 * m._32;
			mat._23 = _21 * m._13 + _22 * m._23 + _23 * m._33;

			mat._31 = _31 * m._11 + _32 * m._21 + _33 * m._31;
			mat._32 = _31 * m._12 + _32 * m._22 + _33 * m._32;
			mat._33 = _31 * m._13 + _32 * m._23 + _33 * m._33;

			return mat;
		}

		/**
		 * @brief 矩阵乘法赋值运算符
		 *
		 * @param m 另一个矩阵
		 * @return 矩阵相乘后的引用
		 */
		Matrix3x3<T>& operator *= (const Matrix3x3<T>& m)
		{
			Matrix3x3<T> mat;
			mat._11 = _11 * m._11 + _12 * m._21 + _13 * m._31;
			mat._12 = _11 * m._12 + _12 * m._22 + _13 * m._32;
			mat._13 = _11 * m._13 + _12 * m._23 + _13 * m._33;

			mat._21 = _21 * m._11 + _22 * m._21 + _23 * m._31;
			mat._22 = _21 * m._12 + _22 * m._22 + _23 * m._32;
			mat._23 = _21 * m._13 + _22 * m._23 + _23 * m._33;

			mat._31 = _31 * m._11 + _32 * m._21 + _33 * m._31;
			mat._32 = _31 * m._12 + _32 * m._22 + _33 * m._32;
			mat._33 = _31 * m._13 + _32 * m._23 + _33 * m._33;

			return *this = mat;
		}

		/**
		 * @brief 获取兼容D2D/GDI的3x2变换矩阵
		 *
		 * @return 6元素数组指针 [_11, _12, _21, _22, _31, _32]
		 */
		T* GetMat3x2()
		{
			static T m32[6] = { 0 };
			m32[0] = _11;
			m32[1] = _12;
			m32[2] = _21;
			m32[3] = _22;
			m32[4] = _31;
			m32[5] = _32;
			return m32;
		}
	};

	/**
	 * @brief 向量与矩阵乘法（Vector2）
	 *
	 * @param v2 二维向量
	 * @param m 3x3矩阵
	 * @return 变换后的二维向量
	 */
	template<typename T>
	inline Vector2<T> operator * (const Vector2<T>& v2, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v2.x * m._11 + v2.y * m._21 + m._31;
		n3.y = v2.x * m._12 + v2.y * m._22 + m._32;
		n3.z = v2.x * m._13 + v2.y * m._23 + m._33;
		if (abs(n3.z) < kEpsilon<T>())
		{
			return Vector2<T>(0, 0);
		}
		return Vector2<T>(n3.x / n3.z, n3.y / n3.z);
	}

	/**
	 * @brief 向量与矩阵乘法赋值（Vector2）
	 *
	 * @param v2 二维向量
	 * @param m 3x3矩阵
	 * @return 变换后的二维向量引用
	 */
	template<typename T>
	inline Vector2<T>& operator *= (Vector2<T>& v2, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v2.x * m._11 + v2.y * m._21 + m._31;
		n3.y = v2.x * m._12 + v2.y * m._22 + m._32;
		n3.z = v2.x * m._13 + v2.y * m._23 + m._33;
		if (abs(n3.z) < kEpsilon<T>())
		{
			v2.x = 0;
			v2.y = 0;
			return v2;
		}
		v2.x = n3.x / n3.z;
		v2.y = n3.y / n3.z;
		return v2;
	}

	/**
	 * @brief 向量与矩阵乘法（Vector3）
	 *
	 * @param v3 三维向量
	 * @param m 3x3矩阵
	 * @return 变换后的三维向量
	 */
	template<typename T>
	inline Vector3<T> operator * (const Vector3<T>& v3, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v3.x * m._11 + v3.y * m._21 + v3.z * m._31;
		n3.y = v3.x * m._12 + v3.y * m._22 + v3.z * m._32;
		n3.z = v3.x * m._13 + v3.y * m._23 + v3.z * m._33;
		return n3;
	}

	/**
	 * @brief 向量与矩阵乘法赋值（Vector3）
	 *
	 * @param v3 三维向量
	 * @param m 3x3矩阵
	 * @return 变换后的三维向量引用
	 */
	template<typename T>
	inline Vector3<T>& operator *= (Vector3<T>& v3, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v3.x * m._11 + v3.y * m._21 + v3.z * m._31;
		n3.y = v3.x * m._12 + v3.y * m._22 + v3.z * m._32;
		n3.z = v3.x * m._13 + v3.y * m._23 + v3.z * m._33;
		return v3 = n3;
	}

	typedef Matrix3x3<float> Mat3x3f;   ///< float类型3x3矩阵
	typedef Matrix3x3<double> Mat3x3d;  ///< double类型3x3矩阵
}

#endif