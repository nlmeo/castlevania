#pragma once
#ifndef __MATH_TYPE_UNIVERSAL_METHED_H__
#define __MATH_TYPE_UNIVERSAL_METHED_H__
#pragma warning(disable:4244)
//通用方法
#include <corecrt_math_defines.h>//包含常用的数学常量定义
#include <cmath>

namespace MathType
{
	//Degree2Radian		angle/180*π
	#define D2R(a)	((a)*0.0174532925199432957692222222222)
	//Radian2Degree		angle/π*180	
	#define R2D(a)	((a)*57.295779513082320876798154814105)
	//将数据限制在指定范围
	#define  CLIMP(n,v,t) (n=n<(v)?(v):n>(t)?(t):n)
	//将数据限制到0~1范围
	#define  CLIMP_2_ONE(t) ((t)<0?0:(t)>1?1:(t))
	//判断浮点数是否为0
	#define  IS_FLOAT_ZERO(f) (((f)<0?-(f):(f))<1e-5)
	//判断是否2的N次方
	#define IS_POWER_OF2(n)  (((n)&((n)-1))==0)
	
	template<typename T>
	inline const T& kEpsilon()
	{
		static T _kEpsilon = static_cast<T>(1 / (pow(10, (sizeof(T) / 4)) * 10000));
		return _kEpsilon;
	}
	
	//将角度限制在 [-π~π] 之间
	template<typename T>
	inline T wrapPI(T angle)
	{
		if (abs(angle) > M_PI)
		{
			//const T TWOPPI = 6.28318530717958647692;//M_PI * 2
			//0.15915494309189533576901767873386 => 1/(2π)
			//T revolutions = floor((angle + M_PI) * 0.15915494309189533576901767873386);
			//angle -= revolutions * TWOPPI;
			angle -= static_cast<T>(floor((angle + M_PI) * 0.15915494309189533576901767873386) * 6.28318530717958647692);
		}
		return angle;
	}

	//将角度限制在 [0 ~ 2π] 之间
	template<typename T>
	inline T wrap2PI(T angle)
	{
		if (abs(angle) > M_PI)
		{
			angle -= static_cast<T>(floor((angle + M_PI) * 0.15915494309189533576901767873386) * 6.28318530717958647692);
		}
		if (angle<0)
		{
			angle += 6.28318530717958647692;
		}
		return angle;
	}

	template<typename T>
	inline T clamp(T target, T _min = 0, T _max = 1)
	{
		return target<_min ? _min : target>_max ? _max : target;
	}

	template<typename T>
	inline T _max(T a, T b)
	{
		return a < b ? b : a;
	}

	template<typename T>
	inline T _min(T a, T b)
	{
		return a < b ? a : b;
	}

}

#endif