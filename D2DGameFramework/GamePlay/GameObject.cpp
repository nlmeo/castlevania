#include "GameObject.h"
#include <algorithm>
namespace GamePlay
{
	bool GameObjectManager::CompareGameObjectLayer::operator()(class RenderComponent* a, class RenderComponent* b)
	{
		auto layer1 = a->gameobject()->layer();
		auto layer2 = b->gameobject()->layer();
		//先比较层级
		if (layer1 != layer2)
		{
			return layer1 > layer2;
		}

		//层级相同的情况下比较层内渲染顺序
		return a->OrderInLayer() > b->OrderInLayer();
	}
	GameObjectManager::~GameObjectManager()
	{
		ClearAllObjects({});
	}
	void GameObjectManager::_UpdateNewObjects()
	{
		_MergeNewObjects();
	}
	void GameObjectManager::_UpdateAllObjects(float deltaTime)
	{
		VideoPlayer* pvp = nullptr;
		Animator* pani = nullptr;
		AfterImageTrail* ptl = nullptr;
		HitFlash* ph = nullptr;
		const unsigned int cullingMask = MainCamera ? MainCamera->CullingMask() : 0;

		mActiveObjectCount = 0;
		mCollitionObjectCount = 0;
		mRenderObjectCount = 0;
		//1.删除销毁的物体
		//2.对激活物体进行更新，非激活物体不更新
		//3.将要处理碰撞的物体和要进行渲染的物体进行分类
		for (auto it = mObjects.begin(); it != mObjects.end();)
		{
			//销毁的物体删除
			if ((*it)->mIsDestroy)
			{
				delete (*it);
				it = mObjects.erase(it);
			}
			else
			{
				//逻辑分类处理
				//只对激活的对象进行逻辑更新
				if ((*it)->Active())
				{
					++mActiveObjectCount;
					
					//更新组件数据
					(*it)->_ComponentUpdate(deltaTime);
					//更新逻辑数据
					(*it)->Update(deltaTime);
					if ((*it)->IsDestroyed() || !(*it)->Active()) { ++it; continue; }

					//如果该物体需要绘制(有绘制组件并且图层允许被绘制)，
					//那么加入到渲染列表
					//通过位运算快速知道哪些层是可以被摄像机绘制的
					
					ph = (*it)->GetComponent<HitFlash>();
					if (ph != nullptr && (unsigned int((*it)->layer()) & cullingMask))
					{
						ph->Update(deltaTime);
					}

					pani = (*it)->GetComponent<Animator>();
					if (pani != nullptr && (unsigned int((*it)->layer()) & cullingMask))
					{
						pani->Update(deltaTime);
					}

					pvp = (*it)->GetComponent<VideoPlayer>();
					if (pvp != nullptr && (unsigned int((*it)->layer()) & cullingMask))
					{
						pvp->Update();
					}

					ptl = (*it)->GetComponent<AfterImageTrail>();
					if (ptl != nullptr && (unsigned int((*it)->layer()) & cullingMask))
					{
						ptl->Update(deltaTime);
					}

					
				}
				++it;
			}
		}
	
	}
	

	void GameObjectManager::_CollitionAndTriggerEvent()
	{
		mCollitionObjectCount = (unsigned int)mColliderObjects.size();
		//处理碰撞和触发事件
		if (!mColliderObjects.empty())
		{
			//按照碰撞区域的矩形AABB区域的left值从小到大进行排序
			std::sort(mColliderObjects.begin(), mColliderObjects.end(),
				[](Collider* a, Collider* b) {
					auto& rc1 = a->CollitionAABBRect();
					auto& rc2 = b->CollitionAABBRect();
					return rc1.left < rc2.left; }
			);

			size_t curIndex = 0;
			size_t colliderCount = mColliderObjects.size();

			while (curIndex < colliderCount)
			{
				const MathType::RectF& curRc = mColliderObjects[curIndex]->CollitionAABBRect();
				for (size_t i = curIndex + 1; i < colliderCount; ++i)
				{
				
					//totest
					//优先进行层级筛选，如果筛选成功则继续，否则直接跳过
					if (!CanCollide(mColliderObjects[curIndex]->gameobject()->layer(), 
						mColliderObjects[i]->gameobject()->layer()))
					{
						continue;
					}
					if (!mColliderObjects[curIndex]->gameobject()->Active() ||
						!mColliderObjects[i]->gameobject()->Active() ||
						mColliderObjects[curIndex]->gameobject()->IsDestroyed() ||
						mColliderObjects[i]->gameobject()->IsDestroyed())
					{
						continue;
					}
					const MathType::RectF& otherRc = mColliderObjects[i]->CollitionAABBRect();

					if (otherRc.left < curRc.right)
					{
						//在x方向上可能相撞,现在检验y方向
						if (!(otherRc.top > curRc.bottom || otherRc.bottom < curRc.top))
						{
							//在y方向也可能相撞，那么这两个碰撞器可能发生碰撞
							//不过考虑到碰撞器有盒子碰撞器也有矩形碰撞器，所以在这样的情况下需要再判断一下
							if (mColliderObjects[curIndex]->Overlap(mColliderObjects[i]))
							{
								//在相交的情况下判断类型
								bool curIsTrigger = mColliderObjects[curIndex]->IsTrigger();
								bool otherIsTrigger = mColliderObjects[i]->IsTrigger();

								if (curIsTrigger && (!otherIsTrigger))
								{
									//如果对方不是触发器，自己是触发器，由对方触发自己的Trigger事件
									mColliderObjects[curIndex]->OnTrigger(mColliderObjects[i]);
								}
								else if (otherIsTrigger && (!curIsTrigger))
								{
									//如果自己不是触发器，对方是触发器，由自己触发对方的Trigger事件
									mColliderObjects[i]->OnTrigger(mColliderObjects[curIndex]);
								}
								else if ((!otherIsTrigger) && (!curIsTrigger))
								{
									//两个都不是触发器，相互触发碰撞事件
									mColliderObjects[curIndex]->OnCollition(mColliderObjects[i]);
									mColliderObjects[i]->OnCollition(mColliderObjects[curIndex]);
								}
								else
								{
									//两个触发器相互之间是不产生事件的
								}
							}
						}
					}
					else
					{
						//如果当前的碰撞器在x方向没有相交的可能性的话，
						//后面的碰撞器就更不可能和当前的碰撞器相交了所以后面就不比较了
						break;
					}
				}
				++curIndex;
			}
			//mColliderObjects.clear();
		}
		
	}
	void GameObjectManager::_LaterUpdateAllObjects(float deltaTime)
	{
		for (auto& p : mObjects)
		{
			if (p->Active() && !p->IsDestroyed())
				p->LaterUpdate(deltaTime);
		}
	}
	void GameObjectManager::_PrepareRenderObjects()
	{
		while (!mRenderList.empty()) mRenderList.pop();
		const unsigned int mask = MainCamera ? MainCamera->CullingMask() : 0;
		for (auto* object : mObjects)
		{
			if (!object->Active() || object->IsDestroyed() || !(unsigned(object->layer()) & mask)) continue;
			// 渲染准备独立于世界逻辑：黑屏和暂停期间也更新首帧与包围盒。
			if (auto* sprite = object->GetComponent<SpriteRender>())
			{
				sprite->CaclRenderMatrix();
				mRenderList.push(sprite);
			}
			if (auto* animator = object->GetComponent<Animator>())
			{
				animator->Update(0);
				animator->CaclRenderMatrix();
				mRenderList.push(animator);
			}
			if (auto* video = object->GetComponent<VideoPlayer>())
			{
				video->CaclRenderMatrix();
				mRenderList.push(video);
			}
		}
		mRenderObjectCount = static_cast<unsigned int>(mRenderList.size());
	}

	void GameObjectManager::_RenderObjects()
	{
		if (!MainCamera) return;
		mRenderObjectCount = (unsigned int)mRenderList.size();
		//只绘制在摄像机AABB包围盒范围内的的物体
		auto& camRect = MainCamera->AABBRect();
		auto& camMatVPV = MainCamera->GetVPV();
		mVisibleObjectCount = 0;

		while (!mRenderList.empty())
		{
			//检查绘制物体的绘制区域的AABB包围盒是否与摄像机的AABB包围盒相交
			if (mRenderList.top()->GetRenderRect().Overlap(camRect))
			{
				++mVisibleObjectCount;
				mRenderList.top()->CaclRenderMatrix();
				mRenderList.top()->Render(camMatVPV);
			}
			mRenderList.pop();
		}
	}
	void GameObjectManager::DestroySceneTransientObjects()
	{
		std::vector<GameObject*> snapshot = mObjects;
		snapshot.insert(snapshot.end(), mNewObjects.begin(), mNewObjects.end());
		for (auto* object : snapshot)
			if (object->mDestroyOnSceneExit) object->Destroy();
	}

	void GameObjectManager::ClearAllObjects(std::vector<GameObject*> ignores, bool clearPending)
	{
		while (!mRenderList.empty()) mRenderList.pop();
		auto clear = [&ignores](std::vector<GameObject*>& objects)
		{
			const auto end = std::remove_if(objects.begin(), objects.end(), [&ignores](GameObject* object) {
				if (std::find(ignores.begin(), ignores.end(), object) != ignores.end()) return false;
				delete object;
				return true;
			});
			objects.erase(end, objects.end());
		};
		clear(mObjects);
		if (clearPending) clear(mNewObjects);
		mActiveObjectCount = mRenderObjectCount = mVisibleObjectCount = 0;
		mCollitionObjectCount = static_cast<unsigned int>(mColliderObjects.size());
	}

	GameObject* GameObject::FindGameObjectByName(std::string _name, bool isActive, bool absolute)
	{
		if (absolute)
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if ((p->name() == _name))
						{
							return p;
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if ((p->name() == _name))
					{
						return p;
					}
				}
			}
		}
		else
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if (p->name().find(_name) != std::string::npos)
						{
							return p;
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->name().find(_name) != std::string::npos)
					{
						return p;
					}
				}
			}
		}
		return nullptr;
	}
	std::vector<GameObject*> GameObject::FindGameObjectsByName(std::string _name, bool isActive, bool absolute)
	{
		std::vector<GameObject*> ret;
		if (absolute)
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if ((p->name() == _name))
						{
							ret.push_back(p);
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if ((p->name() == _name))
					{
						ret.push_back(p);
					}
				}
			}
		}
		else
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if (p->name().find(_name) != std::string::npos)
						{
							ret.push_back(p);
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->name().find(_name) != std::string::npos)
					{
						ret.push_back(p);
					}
				}
			}
		}
		return ret;
	}
	GameObject* GameObject::FindGameObjectByTag(std::string _tag, bool isActive, bool absolute)
	{
		if (absolute)
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if ((p->tag() == _tag))
						{
							return p;
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if ((p->tag() == _tag))
					{
						return p;
					}
				}
			}
		}
		else
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if (p->tag().find(_tag) != std::string::npos)
						{
							return p;
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->tag().find(_tag) != std::string::npos)
					{
						return p;
					}
				}
			}
		}
		return nullptr;
	}
	std::vector<GameObject*> GameObject::FindGameObjectsByTag(std::string _tag, bool isActive, bool absolute)
	{
		std::vector<GameObject*> ret;
		if (absolute)
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if ((p->tag() == _tag))
						{
							ret.push_back(p);
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if ((p->tag() == _tag))
					{
						ret.push_back(p);
					}
				}
			}
		}
		else
		{
			if (isActive)
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->Active())
					{
						if (p->tag().find(_tag) != std::string::npos)
						{
							ret.push_back(p);
						}
					}
				}
			}
			else
			{
				for (auto& p : GameObjectMgr.mObjects)
				{
					if (p->tag().find(_tag) != std::string::npos)
					{
						ret.push_back(p);
					}
				}
			}
		}
		return ret;
	}
}

