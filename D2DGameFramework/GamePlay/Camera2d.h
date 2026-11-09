#pragma once
#ifndef __GAME_PLAY_FRAME_CAMERA_2D_H__
#define __GAME_PLAY_FRAME_CAMERA_2D_H__

#include "Component.h"
#include "../TypeDefine/Matrix3x3.h"
#include "../TypeDefine/Geometric.h"
#include "../TypeDefine/Vector2.h"

// 前置声明
class GameObject;

namespace GamePlay
{
	/**
	 * @brief 摄像机跟随模式枚举
	 */
	typedef enum class emCameraFollowMode
	{
		Follow_Hard = 0,      ///< 硬跟随：立即跟随目标位置
		Follow_Smooth,        ///< 平滑跟随：通过插值平滑跟随
		Follow_DeadZone,      ///< 死区跟随：在一定范围内不跟随
		Follow_Mode_Limit,    ///< 模式数量限制（非有效模式）
	}CFM;

	/**
	 * @brief 2D摄像机组件
	 *
	 * 负责管理游戏视角，支持多种跟随模式和屏幕震动效果
	 */
	class Camera : public Component
	{
		static Camera* mainCamera;  ///< 主摄像机实例

	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pObj 所属游戏对象指针
		 */
		Camera(class GameObject* pObj) : Component(pObj)
		{
			mType = COMTYPE::ComType_Camera;
			mCulllingMask = 0xffffffff;
			SetCamera(16 / 9.0f, 100);
		}

		~Camera() override
		{
			if (mainCamera == this) mainCamera = nullptr;
		}

		/** @brief 获取主摄像机 */
		static Camera* main() { return mainCamera; }

		/**
		 * @brief 设置摄像机参数
		 *
		 * @param aspect 宽高比
		 * @param sizey 垂直观察范围大小
		 */
		void SetCamera(float aspect, float sizey);

		/**
		 * @brief 窗口大小改变时调用
		 *
		 * @param clientWidth 客户端宽度
		 * @param clientHeight 客户端高度
		 */
		void OnAppSizeChange(int clientWidth, int clientHeight);

		/** @brief 更新摄像机矩阵 */
		void UpdateCamera();

		/** @brief 更新跟随摄像机（在所有物体更新后调用） */
		void FllowCameraUpdate(float deltatime);

		/** @brief 获取宽高比 */
		const float& Aspect() const { return mAspect; }

		/** @brief 获取垂直观察范围 */
		const float& SizeY() const { return mSizey; }

		/** @brief 获取水平观察范围 */
		const float& SizeX() const { return mSizex; }

		/** @brief 获取摄像机AABB包围盒 */
		const MathType::RectF& AABBRect() const { return mAABBRect; }

		/**
		 * @brief 设置垂直观察范围
		 *
		 * @param sy 垂直范围大小
		 */
		void SetSizeY(float sy);

		/**
		 * @brief 设置宽高比
		 *
		 * @param aspect 宽高比
		 */
		void SetAspect(float aspect);

		/**
		 * @brief 设置裁剪遮罩
		 *
		 * @param cullingmask 层级位掩码
		 */
		void SetCullingMask(unsigned int cullingmask) { mCulllingMask = cullingmask; }

		/** @brief 获取裁剪遮罩 */
		const unsigned int& CullingMask() const { return mCulllingMask; }

		/** @brief 获取视图-投影-视口变换矩阵 */
		const MathType::Mat3x3f& GetVPV() const;

		/**
		 * @brief 设置跟随目标
		 *
		 * @param pTarget 目标Transform组件
		 * @param withAngle 是否跟随角度（默认false）
		 */
		void SetTarget(Transform* pTarget, bool withAngle = false);

		/** @brief 获取当前跟随模式 */
		CFM getFollowMode() const;

		/**
		 * @brief 设置跟随模式
		 *
		 * @param followmode 跟随模式
		 */
		void SetFollowMode(CFM followmode);

		/**
		 * @brief 设置平滑跟随参数
		 *
		 * @param maxdistance 硬跟随距离阈值
		 * @param param 插值强度（0~1，不含0和1）
		 */
		void SetSmoothFollowParam(float speed, float maxdistance, float param);

		//
		void SetDeadZoonFollowParam(float deadzoonx,float deadzoony, float param);

		/** @brief 取消跟随目标 */
		void FreeCamera();

		/**
		 * @brief 设置屏幕震动效果
		 *
		 * @param shaketimelong 震动时长（秒）
		 * @param strength 震动强度
		 */
		void Shake(float shaketimelong, float strength);

		/**
		 * @brief 更新屏幕震动
		 *
		 * @param deltatimesecond 时间增量（秒）
		 */
		void ShakeCameraUpdate(float deltatimesecond);

		/** @brief 判断是否正在震动 */
		bool IsShakeing() const { return mShaking; }

		void SetCameraBound(const MathType::RectF& r) { mCameraBound = r; }

		const MathType::RectF& CameraBound() const { return mCameraBound; }

		const MathType::vec2f& GetPredictedPosition();

		/**
		 * @brief 将世界坐标转换为屏幕坐标
		 *
		 * 计算 GameObject 的世界位置在窗口中的像素坐标。
		 *
		 * @param pObj 目标游戏对象指针
		 * @return 屏幕像素坐标（左上角为原点）
		 */
		MathType::vec2f ToScreenPos(GameObject& pObj) const;

		/**
		 * @brief 将世界坐标点转换为屏幕坐标
		 *
		 * @param worldPos 世界坐标
		 * @return 屏幕像素坐标
		 */
		MathType::vec2f ToScreenPos(const MathType::vec2f& worldPos) const;

	protected:
		/** @brief 更新视口角点 */
		void _UpdateCornerPoints();

		//死区范围更新
		void _UpdateDeadZoon();

		//检验点是否在死区范围内
		bool _IsPointInDeadZoonRect(MathType::vec2f p);

	protected:
		float mSizex;                     ///< 水平观察范围
		float mSizey;                     ///< 垂直观察范围
		float mAspect;                    ///< 宽高比

		unsigned int mCulllingMask;       ///< 裁剪遮罩（层级过滤）

		bool mFollowTargetWithAngle;      ///< 是否跟随目标角度
		Transform* mFollowTarget;         ///< 跟随目标
		CFM mFollowMode;                  ///< 跟随模式
		MathType::vec2f mLastTargetPos;   ///< 上一次目标的位置

		float mSmoothFollowMaxdistance;   ///< 平滑跟随距离阈值
		float mSmoothFollowParam;         ///< 平滑插值参数
		float mSmoothFollowSpeed;		  ///< 平滑跟随速度

		float mDeadZoneFollowSizeX;		  ///< 死区跟随x方向大小,这个是一个比例值,比如0.5意味着水平方向的死区宽度是摄像机当前宽度乘以0.5
		float mDeadZoneFollowSizeY;		  ///< 死区跟随y方向大小,这个是一个比例值,比如0.3意味着竖直方向的死区高度是摄像机当前高度乘以0.3
		MathType::vec2f mDeadZoonRectCorner[4];///< 死区矩形范围四个角点
		float mDeadZoonFollowParam;       ///< 死区跟随插值参数,类似
		float mBack2CenterTime;			  ///停止移动后回归中心跟随的间隔时间


		bool mShaking;                    ///< 是否正在震动
		float mShakingtime;               ///< 已震动时长
		float mShakeTimeLong;             ///< 震动总时长
		float mShakeStrength;             ///< 震动强度
		MathType::vec2f mShakePosOrig;    ///< 震动原点
		



		MathType::vec2f mViewCornerPoint[4];	///< 视口四角坐标
		MathType::RectF mAABBRect;				///< 观察范围AABB
		MathType::RectF mCameraBound;			///<摄像机边界
		MathType::Mat3x3f mMatrixView;			///< 视图矩阵（平移+旋转）
		MathType::Mat3x3f mMatrixProjection;	///< 投影矩阵（缩放至2x2范围）
		MathType::Mat3x3f mMatrixViewport;		///< 视口矩阵（变换到窗口）
	};

	/** @brief 主摄像机宏定义 */
#define MainCamera GamePlay::Camera::main()
}

#endif
