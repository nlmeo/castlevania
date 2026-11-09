#include "../GameCore/Application.h"
#include "GameObject.h"


namespace GamePlay
{
	Camera* Camera::mainCamera = nullptr;

	void Camera::SetCamera(float aspect, float sizey)
	{
		mainCamera = this;

		mAspect = aspect;
		mSizey = sizey;
		mSizex = mSizey * mAspect;

		const MathType::vec2f& pos = transform()->position();
		mMatrixView.SetTranslate(-pos.x, -pos.y).RotateDegree(transform()->rotation());

		mMatrixProjection.SetScale(2 / mSizex, 2 / mSizey);

		float w = App.GetWidth() / 2.0f;
		float h = App.GetHeight() / 2.0f;
		//翻转坐标系为笛卡尔坐标系
		//mMatrixViewport.SetScale(w, -h).Translate(w, h);
		mMatrixViewport.SetScale(w, h).Translate(w, h);
		mFollowTargetWithAngle = false;
		mFollowTarget = nullptr;
		mFollowMode = CFM::Follow_Hard;

		mSmoothFollowMaxdistance = 100.0f;
		mSmoothFollowParam = 0.9f;
		mSmoothFollowSpeed = 10.0f;

		mDeadZoneFollowSizeX = 0.5f;
		mDeadZoneFollowSizeY = 0.5f;
		mDeadZoonFollowParam = 0.95f;
		mBack2CenterTime = 0.0f;

		mShaking = false;
		mShakingtime = 0;//已经震动的时长
		mShakeTimeLong = 3;//震动时长
		mShakeStrength = 1;//震动烈度
		mShakePosOrig = transform()->position();//震动的原点

		_UpdateCornerPoints();
	}
	void Camera::OnAppSizeChange(int clientWidth, int clientHeight)
	{
		if (clientWidth <= 0 || clientHeight <= 0) return;
		// Preserve the scene's logical view (e.g. 768x576), just as on scene entry.
		// Changing its aspect here exposes extra world area instead of resizing the background.
		float w = clientWidth / 2.0f;
		float h = clientHeight / 2.0f;
		//为什么是-h,就是因为窗口坐标系和世界的坐标系y轴相反
		//mMatrixViewport.SetScale(w, -h).Translate(w, h);
		//还原非笛卡尔坐标系（win原生坐标系）
		mMatrixViewport.SetScale(w, h).Translate(w, h);
		UpdateCamera();
	}
	void Camera::UpdateCamera()
	{
		const MathType::vec2f& pos = transform()->position();
		mMatrixView.SetTranslate(-pos.x, -pos.y).RotateDegree(transform()->rotation());

		//计算AABB包围盒的
		_UpdateCornerPoints();
	}
	void Camera::FllowCameraUpdate(float deltatime)
	{
		if (mFollowTarget != nullptr)
		{
			if (mFollowMode == CFM::Follow_Hard)
			{
				transform()->SetPosition(GetPredictedPosition());
				if (mFollowTargetWithAngle)
				{
					transform()->SetRotation(mFollowTarget->rotation());
				}
				mLastTargetPos = mFollowTarget->position();
			}
			else if (mFollowMode == CFM::Follow_Smooth)
			{
				auto& targetPos = mFollowTarget->position();
				//auto& targetPos = GetPredictedPosition();
				
				//当前位置减去上一次的位置就是移动方向
				MathType::vec2f movedir = targetPos - mLastTargetPos;
				//说明目标在移动
				if (!movedir.isZero())
				{
					//更新上一次移动位置
					mLastTargetPos = targetPos;

					auto& camPos = transform()->position();
					//当前摄像机和目标的相对位置向量，从目标指向摄像机方向
					auto dirvector = camPos - targetPos;
					//当前剩余的路程
					float curdistance = dirvector.magnitude();
					//理论上在现在的时间内能移动的距离
					float curCamMoveDistance = mSmoothFollowSpeed * deltatime;

					//如果理论移动距离大于剩余路程，直接设置到终点
					if (curdistance < curCamMoveDistance)
					{
						transform()->SetPosition(targetPos);
					}
					else
					{
						float leftdistance = curdistance - curCamMoveDistance;

						//移动以后的距离如果大于最大距离
						if (leftdistance > mSmoothFollowMaxdistance)
						{
							//以最大距离作为跟随距离
							leftdistance = mSmoothFollowMaxdistance;
						}
						else
						{
							//在最大距离范围内，以剩余距离作为跟随距离
						}

						transform()->SetPosition(targetPos + dirvector.normalized() * leftdistance);
					}
				}
				else
				{
					//目标停止移动了以后
					auto& camPos = transform()->position();
					//当前摄像机和目标的相对位置向量，从目标指向摄像机方向
					auto dirvector = camPos - targetPos;
					transform()->SetPosition(targetPos + dirvector * mSmoothFollowParam);
				}

				if (mFollowTargetWithAngle)
				{
					float tgRot = mFollowTarget->rotation();
					float camRot = transform()->rotation();

					if (IS_FLOAT_ZERO(tgRot - camRot))
					{
						transform()->SetRotation(mFollowTarget->rotation());
					}
					else
					{
						camRot = camRot + (tgRot - camRot) * mSmoothFollowParam;
						transform()->SetRotation(camRot);
					}
				}
			}
			else if (mFollowMode == CFM::Follow_DeadZone)
			{
				//先获取跟随目标的位置以及摄像机的位置
				auto& targetPos = mFollowTarget->position();
				auto& camPos = transform()->position();
				auto dirvector = camPos - targetPos;
				
				//当前位置减去上一次的位置就是移动方向
				MathType::vec2f movedir = targetPos - mLastTargetPos;
				//说明目标在移动
				if (!movedir.isZero())
				{
					mBack2CenterTime = 0;
					//在死区范围外
					if (!_IsPointInDeadZoonRect(targetPos))
					{
						transform()->SetPosition(camPos - mLastTargetPos + targetPos);
					}
					//更新上一次移动位置
					mLastTargetPos = targetPos;
				}
				else
				{
					//当前摄像机和目标的相对位置向量，从目标指向摄像机方向
					auto dirvector = camPos - targetPos;
					if (!dirvector.isZero())
					{
						mBack2CenterTime += deltatime;
						if (mBack2CenterTime > 1.0f)
						{
							transform()->SetPosition(targetPos + dirvector * mDeadZoonFollowParam);
						}
					}
				}

				if (mFollowTargetWithAngle)
				{
					float tgRot = mFollowTarget->rotation();
					float camRot = transform()->rotation();

					if (IS_FLOAT_ZERO(tgRot - camRot))
					{
						transform()->SetRotation(mFollowTarget->rotation());
					}
					else
					{
						camRot = camRot + (tgRot - camRot) * mDeadZoonFollowParam;
						transform()->SetRotation(camRot);
					}
				}
			}
			UpdateCamera();
		}
	}
	void Camera::SetSizeY(float sy)
	{
		mSizey = sy;
		mSizex = mSizey * mAspect;
		mMatrixProjection.SetScale(2 / mSizex, 2 / mSizey);

		_UpdateCornerPoints();
	}
	void Camera::SetAspect(float aspect)
	{
		mAspect = aspect;
		mSizex = mSizey * mAspect;
		mMatrixProjection.SetScale(2 / mSizex, 2 / mSizey);

		_UpdateCornerPoints();
	}

	const MathType::Mat3x3f& Camera::GetVPV() const
	{
		static MathType::Mat3x3f matVPV;
		matVPV = mMatrixView * mMatrixProjection * mMatrixViewport;
		return matVPV;
	}

	

	MathType::vec2f Camera::ToScreenPos(const MathType::vec2f& worldPos) const
	{
		// worldPos × (View × Projection × Viewport) = 屏幕像素坐标
		return worldPos * GetVPV();
	}

	MathType::vec2f Camera::ToScreenPos(GamePlay::GameObject& obj) const
	{
		return ToScreenPos(obj.transform().position());
	}

	void Camera::SetTarget(Transform* pTarget, bool withAngle)
	{
		mFollowTarget = pTarget;
		mFollowTargetWithAngle = withAngle;
	}
	CFM Camera::getFollowMode() const
	{
		return mFollowMode;
	}
	void Camera::SetFollowMode(CFM followmode)
	{
		mFollowMode = followmode;
		_UpdateDeadZoon();
	}
	void Camera::SetSmoothFollowParam(float speed, float maxdistance, float param)
	{
		if (IS_FLOAT_ZERO(maxdistance))
		{
			maxdistance = 10.0f;
		}
		mSmoothFollowSpeed = speed;
		mSmoothFollowMaxdistance = abs(maxdistance);
		mSmoothFollowParam = MathType::clamp(param, 0.0001f, 0.9999f);
	}
	void Camera::SetDeadZoonFollowParam(float deadzoonx, float deadzoony, float param)
	{
		mDeadZoneFollowSizeX = abs(deadzoonx);
		mDeadZoneFollowSizeY = abs(deadzoony);
		if (IS_FLOAT_ZERO(mDeadZoneFollowSizeX))
		{
			mDeadZoneFollowSizeX = 0.5f;
		}
		if (IS_FLOAT_ZERO(mDeadZoneFollowSizeY))
		{
			mDeadZoneFollowSizeY = 0.5f;
		}
		mDeadZoonFollowParam = MathType::clamp(param, 0.0001f, 0.9999f);

		_UpdateDeadZoon();
	}
	void Camera::FreeCamera()
	{
		mFollowTarget = nullptr;
		mFollowTargetWithAngle = false;
	}
	void Camera::Shake(float shaketimelong, float strength)
	{
		if (!mShaking)
		{
			mShaking = true;
			mShakingtime = 0;//已经震动的时长
			mShakeTimeLong = shaketimelong;//震动时长
			mShakeStrength = strength;//震动烈度
			mShakePosOrig = transform()->position();//震动的原点
		}
		else
		{
			//在震动的情况下再次调用该函数，等于重置时长和烈度
			mShakingtime = 0;//已经震动的时长
			mShakeTimeLong = shaketimelong;//震动时长
			mShakeStrength = strength;//震动烈度
		}

	}
	void Camera::ShakeCameraUpdate(float deltatimesecond)
	{
		if (mShaking)
		{
			mShakingtime += deltatimesecond;
			if (mShakingtime < mShakeTimeLong)
			{
				transform()->SetPosition(mShakePosOrig + MathType::vec2f::random() * mShakeStrength);
			}
			else
			{
				mShaking = false;
				transform()->SetPosition(mShakePosOrig);
			}
		}
	}

	const MathType::vec2f& Camera::GetPredictedPosition()
	{
		static MathType::vec2f predictedPosition;

		float cw = mSizex;
		float ch = mSizey;
		float camera_x = mFollowTarget->position().x;
		float camera_y = mFollowTarget->position().y;
		MathType::RectF& cameraBound = mCameraBound;
		if (camera_x < cameraBound.left + cw / 2)
		{
			camera_x = cameraBound.left + cw / 2;
		}
		else if (camera_x > cameraBound.right - cw / 2)
		{
			camera_x = cameraBound.right - cw / 2;
		}

		if (camera_y < cameraBound.top + ch / 2)
		{
			camera_y = cameraBound.top + ch / 2;
		}
		else if (camera_y > cameraBound.bottom - ch / 2)
		{
			camera_y = cameraBound.bottom - ch / 2;
		}
		predictedPosition.x = camera_x;
		predictedPosition.y = camera_y;
		return predictedPosition;
	}

	void Camera::_UpdateCornerPoints()
	{
		MathType::vec2f halfright = transform()->right() * mSizex;
		MathType::vec2f halfup = transform()->up() * mSizey;
		const MathType::vec2f& pos = transform()->position();

		mViewCornerPoint[0] = pos - halfright - halfup;
		mViewCornerPoint[1] = pos + halfright - halfup;
		mViewCornerPoint[2] = pos + halfright + halfup;
		mViewCornerPoint[3] = pos - halfright + halfup;

		float& minx = mAABBRect.left;
		float& maxx = mAABBRect.right;
		float& miny = mAABBRect.top;
		float& maxy = mAABBRect.bottom;

		minx = mViewCornerPoint[0].x;
		miny = mViewCornerPoint[0].y;
		maxx = mViewCornerPoint[0].x;
		maxy = mViewCornerPoint[0].y;

		for (int i = 1; i < 4; ++i)
		{
			if (minx > mViewCornerPoint[i].x) { minx = mViewCornerPoint[i].x; }
			if (maxx < mViewCornerPoint[i].x) { maxx = mViewCornerPoint[i].x; }
			if (miny > mViewCornerPoint[i].y) { miny = mViewCornerPoint[i].y; }
			if (miny < mViewCornerPoint[i].y) { maxy = mViewCornerPoint[i].y; }
		}
		_UpdateDeadZoon();
	}
	void Camera::_UpdateDeadZoon()
	{

		if (mFollowMode == CFM::Follow_DeadZone)
		{

			MathType::vec2f vx = transform()->right() * mDeadZoneFollowSizeX * mSizex;
			MathType::vec2f vy = transform()->up() * mDeadZoneFollowSizeY * mSizey;

			auto& campos = transform()->position();

			mDeadZoonRectCorner[0] = campos - vx - vy;
			mDeadZoonRectCorner[1] = campos + vx - vy;
			mDeadZoonRectCorner[2] = campos + vx + vy;
			mDeadZoonRectCorner[3] = campos - vx + vy;
		}
	}
	bool Camera::_IsPointInDeadZoonRect(MathType::vec2f p)
	{
		//判断点是否在非轴对称矩形范围内可以使用投影法,可以使用三角形分割判断,也可以使用面积法等等
		//我这里使用将矩形变为两个三角形的判断方法

		MathType::Triangle t1(mDeadZoonRectCorner[0], mDeadZoonRectCorner[1], mDeadZoonRectCorner[2]);
		MathType::Triangle t2(mDeadZoonRectCorner[0], mDeadZoonRectCorner[2], mDeadZoonRectCorner[3]);
		return t1.PointInTriangle(p) || t2.PointInTriangle(p);
	}
}
