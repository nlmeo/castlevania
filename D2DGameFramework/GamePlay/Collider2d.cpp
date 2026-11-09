#include "GameObject.h"
#include <cmath>
#include "../TypeDefine/Universal.h"
namespace GamePlay
{
	namespace
	{
		/** @brief OBB数据：中心点、两条单位轴、两条轴的半长 */
		struct OBBData
		{
			MathType::vec2f center;     ///< 中心点
			MathType::vec2f axis[2];    ///< 两条单位轴（已旋转）
			float halfSize[2];          ///< 两条轴的半长
		};

		/**
		 * @brief 构造OBB数据
		 *
		 * @param center 中心点
		 * @param rotation 旋转角度（弧度）
		 * @param halfWidth 半宽
		 * @param halfHeight 半高
		 * @param out 输出的OBB数据
		 */
		void BuildOBB(const MathType::vec2f& center, float rotation, float halfWidth, float halfHeight, OBBData& out)
		{
			float c = cos(D2R(rotation));
			float s = sin(D2R(rotation));
			out.center = center;
			out.axis[0] = MathType::vec2f(c, s);   // 右方向单位轴
			out.axis[1] = MathType::vec2f(-s, c);  // 上方向单位轴
			out.halfSize[0] = halfWidth;
			out.halfSize[1] = halfHeight;
		}

		/**
		 * @brief 分离轴定理(SAT)：检测两个OBB是否相交
		 *
		 * 将两个盒子分别投影到四条候选轴（各盒的两条单位轴）上，
		 * 只要存在一条分离轴则两盒不相交。
		 *
		 * @param a 盒子A
		 * @param b 盒子B
		 * @return 是否相交
		 */
		bool SATOverlap(const OBBData& a, const OBBData& b)
		{
			const OBBData* boxes[2] = { &a, &b };
			for (int k = 0; k < 2; ++k)
			{
				const OBBData& box = *boxes[k];
				for (int i = 0; i < 2; ++i)
				{
					const MathType::vec2f& axis = box.axis[i];
					//两盒在axis上的投影半径之和
					float ra = a.halfSize[0] * fabs(a.axis[0].Dot(axis)) + a.halfSize[1] * fabs(a.axis[1].Dot(axis));
					float rb = b.halfSize[0] * fabs(b.axis[0].Dot(axis)) + b.halfSize[1] * fabs(b.axis[1].Dot(axis));
					float dist = fabs((a.center - b.center).Dot(axis));
					if (dist > ra + rb)
					{
						return false;   //存在分离轴，两盒不相交
					}
				}
			}
			return true;
		}

		/**
		 * @brief OBB与圆相交检测
		 *
		 * 将圆心变换到OBB局部坐标系后做AABB-圆检测。
		 *
		 * @param box OBB盒子
		 * @param center 圆心（世界坐标）
		 * @param radius 半径
		 * @return 是否相交
		 */
		bool OBBOverlapSphere(const OBBData& box, const MathType::vec2f& center, float radius)
		{
			MathType::vec2f rel = center - box.center;
			//变换到OBB局部坐标
			float lx = rel.Dot(box.axis[0]);
			float ly = rel.Dot(box.axis[1]);
			//将局部坐标夹取到盒子范围内的最近点
			float cx = MathType::clamp(lx, -box.halfSize[0], box.halfSize[0]);
			float cy = MathType::clamp(ly, -box.halfSize[1], box.halfSize[1]);
			float dx = lx - cx;
			float dy = ly - cy;
			return (dx * dx + dy * dy) <= (radius * radius);
		}
	}

	Collider::Collider(GameObject* pobj):Component(pobj)
	{
		mType = COMTYPE::ComType_Collider;
		mInCollitionCallBack = nullptr;
		mInTriggerCallBack = nullptr;
		mIsTrigger = false;
		GameObjectMgr._AddColliderObject(this);
	}
	Collider::~Collider()
	{
		GameObjectMgr._RemoveColliderObject(this); 
	}
	BoxCollider::BoxCollider(GameObject* pobj):Collider(pobj)
	{
		mType = COMTYPE::ComType_BoxCollider;
		mWidth = 0;
		mHeight = 0;
	}
	void BoxCollider::SetColliderSize(float width, float height)
	{
		mWidth = width;
		mHeight = height;
		UpdateCollider();
	}
	void BoxCollider::UpdateCollider()
	{
		auto& pos = transform()->position();
		float halfw = mWidth * 0.5f;
		float halfh = mHeight * 0.5f;
		mCollitonRect.left = pos.x - halfw;
		mCollitonRect.right = pos.x + halfw;
		mCollitonRect.top = pos.y - halfh;
		mCollitonRect.bottom = pos.y + halfh;
	}

	bool BoxCollider::Overlap(const Collider* other)
	{
		auto ot = other->Type();
		if (ot == COMTYPE::ComType_BoxCollider)
		{
			//盒子和盒子相交
			return mCollitonRect.Overlap(other->CollitionAABBRect());
		}
		else if (ot == COMTYPE::ComType_RotationBoxCollider)
		{
			//普通矩形（旋转角为0）和旋转矩形做SAT投影检测
			OBBData a, b;
			BuildOBB(transform()->position(), 0, mWidth * 0.5f, mHeight * 0.5f, a);
			const RotationBoxCollider* p = dynamic_cast<const RotationBoxCollider*>(other);
			BuildOBB(other->transform()->position(), other->transform()->rotation(), p->Width() * 0.5f, p->Height() * 0.5f, b);
			return SATOverlap(a, b);
		}
		else if(ot == COMTYPE::ComType_SphereCollider)
		{
			//盒子和球相交检测
			auto& pos = other->transform()->position();
			const SphereCollider* p = dynamic_cast<const SphereCollider*>(other);
			return mCollitonRect.OverlapSphere(pos.x, pos.y, p->Radius());
		}
		return false;
	}

	SphereCollider::SphereCollider(GameObject* pobj):Collider(pobj)
	{
		mType = COMTYPE::ComType_SphereCollider;
		mRadius = 0;
	}
	void SphereCollider::SetColliderRadius(float radius)
	{
		mRadius = radius;
		UpdateCollider();
	}
	void SphereCollider::UpdateCollider()
	{
		auto& pos = transform()->position();
		mCollitonRect.left = pos.x - mRadius;
		mCollitonRect.right = pos.x + mRadius;
		mCollitonRect.top = pos.y - mRadius;
		mCollitonRect.bottom = pos.y + mRadius;
	}
	bool SphereCollider::Overlap(const Collider* other)
	{
		auto ot = other->Type();
		if (ot == COMTYPE::ComType_BoxCollider)
		{
			const auto& pos = transform()->position();
			//盒子和球相交
			return other->CollitionAABBRect().OverlapSphere(pos.x,pos.y,mRadius);
		}
		else if (ot == COMTYPE::ComType_RotationBoxCollider)
		{
			//圆和旋转矩形相交：把旋转矩形当作OBB做检测
			OBBData b;
			const RotationBoxCollider* p = dynamic_cast<const RotationBoxCollider*>(other);
			BuildOBB(other->transform()->position(), other->transform()->rotation(), p->Width() * 0.5f, p->Height() * 0.5f, b);
			return OBBOverlapSphere(b, transform()->position(), mRadius);
		}
		else if (ot == COMTYPE::ComType_SphereCollider)
		{
			//球和球相交检测
			const auto& opos = other->transform()->position();
			const auto& tpos = transform()->position();
			const SphereCollider* p = dynamic_cast<const SphereCollider*>(other);

			float R = mRadius + p->Radius();
			return (opos - tpos).sqrMagnitude() < (R * R);	
		}
		return false;
	}
	
	RotationBoxCollider::RotationBoxCollider(GameObject* pobj) : Collider(pobj)
	{
		mType = COMTYPE::ComType_RotationBoxCollider;
		mWidth = 0;
		mHeight = 0;
	}
	void RotationBoxCollider::SetColliderSize(float width, float height)
	{
		mWidth = width;
		mHeight = height;
		UpdateCollider();
	}
	void RotationBoxCollider::UpdateCollider()
	{
		MathType::vec2f corners[4];
		GetWorldCorners(corners);

		//计算旋转后四个顶点的AABB包围盒，用于Sweep-and-Prune粗检测
		float minX = corners[0].x, maxX = corners[0].x;
		float minY = corners[0].y, maxY = corners[0].y;
		for (int i = 1; i < 4; ++i)
		{
			minX = MathType::_min(minX, corners[i].x);
			maxX = MathType::_max(maxX, corners[i].x);
			minY = MathType::_min(minY, corners[i].y);
			maxY = MathType::_max(maxY, corners[i].y);
		}
		mCollitonRect.left = minX;
		mCollitonRect.right = maxX;
		mCollitonRect.top = minY;
		mCollitonRect.bottom = maxY;
	}

	float RotationBoxCollider::Rotation() const
	{
		return transform()->rotation();
	}



	void RotationBoxCollider::GetWorldCorners(MathType::vec2f corners[4]) const
	{
		const auto& pos = transform()->position();
		float rot = transform()->rotation();
		float c = cos(D2R(rot));
		float s = sin(D2R(rot));

		MathType::vec2f right(c, s);    //右方向单位轴
		MathType::vec2f up(-s, c);      //上方向单位轴
		float hw = mWidth * 0.5f;
		float hh = mHeight * 0.5f;

		corners[0] = pos + right * hw + up * hh;
		corners[1] = pos + right * hw - up * hh;
		corners[2] = pos - right * hw - up * hh;
		corners[3] = pos - right * hw + up * hh;
	}
	bool RotationBoxCollider::Overlap(const Collider* other)
	{
		auto ot = other->Type();
		if (ot == COMTYPE::ComType_BoxCollider)
		{
			//旋转矩形和普通矩形（旋转角为0的矩形）做SAT投影检测
			OBBData a, b;
			BuildOBB(transform()->position(), transform()->rotation(), mWidth * 0.5f, mHeight * 0.5f, a);
			const BoxCollider* p = dynamic_cast<const BoxCollider*>(other);
			BuildOBB(other->transform()->position(), 0, p->Width() * 0.5f, p->Height() * 0.5f, b);
			return SATOverlap(a, b);
		}
		else if (ot == COMTYPE::ComType_RotationBoxCollider)
		{
			//旋转矩形和旋转矩形做SAT投影检测
			OBBData a, b;
			BuildOBB(transform()->position(), transform()->rotation(), mWidth * 0.5f, mHeight * 0.5f, a);
			const RotationBoxCollider* p = dynamic_cast<const RotationBoxCollider*>(other);
			BuildOBB(other->transform()->position(), other->transform()->rotation(), p->Width() * 0.5f, p->Height() * 0.5f, b);
			return SATOverlap(a, b);
		}
		else if (ot == COMTYPE::ComType_SphereCollider)
		{
			//旋转矩形和圆相交检测
			OBBData a;
			BuildOBB(transform()->position(), transform()->rotation(), mWidth * 0.5f, mHeight * 0.5f, a);
			const SphereCollider* p = dynamic_cast<const SphereCollider*>(other);
			return OBBOverlapSphere(a, p->transform()->position(), p->Radius());
		}
		return false;
	}
	
}