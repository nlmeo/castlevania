#include "GameObject.h"

namespace GamePlay
{
	void Transform::rotate(const float& angle)
	{
		MathType::Mat3x3f mr;
		//得到本次旋转角度对应的旋转矩阵
		mr.SetRotateDegree(angle);
		//在自身的基础上进行旋转，应该将当前本身的旋转矩阵乘以本次角度对应的旋转矩阵
		matrixRotate *= mr;

		//已知旋转矩阵结果如下，那么可以通过下面的结果计算出反三角函数得到当前角度值
		//			| cz  sz  0 |
		//R(z) = 	|-sz  cz  0 |
		//			| 0   0   1 |
	
		/*
			0~90		90~180			180~270		270~360
		sin	+			+					-			-
		cos	+			-					-			+
		*/
		float& sinValue = matrixRotate._12;
		float& cosValue = matrixRotate._11;

		// 使用atan2计算弧度（注意参数顺序：y = sin, x = cos）
		float radians = std::atan2(sinValue, cosValue);
		// 弧度转角度（使用更精确的PI常量）
		float degrees = R2D(radians);

		// 调整到[0, 360)范围
		mRotation = fmod(degrees, 360.0f);  // 取模处理
		if (mRotation < 0) {
			mRotation += 360.0f;  // 修正负值
		}

		mRight = MathType::vec2f::right() * matrixRotate;
		mUp = MathType::vec2f::up() * matrixRotate;
		mRight.Normalize();
		mUp.Normalize();

		mNeedUpdate = true;
	}

	void Transform::_update_Matrix_Translate()
	{
		matrixTranslate.SetTranslate(mPosition.x, mPosition.y);
	}

	void Transform::_update_Matrix_Scale()
	{
		matrixScale.SetScale(mScale.x, mScale.y);
	}

	void Transform::_update_Matrix_Rotate()
	{
		matrixRotate.SetRotateDegree(mRotation);
		mRight = MathType::vec2f::right() * matrixRotate;
		mUp = MathType::vec2f::up() * matrixRotate;
		mRight.Normalize();
		mUp.Normalize();
	}
}