#pragma once
#ifndef __MY_ANIMATION_H__
#define __MY_ANIMATION_H__

#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include "GameLayer.h"
#include "Component.h"
#include "../TypeDefine/Matrix3x3.h"
#include "../TypeDefine/Vector2.h"
#include "../D2DCore/TextureManager.h"
namespace GamePlay
{

	typedef struct AnimationFrame
	{
		D2DCore::Texture* texture;		
		//const wchar_t* textureKey;	
		MathType::vec2f anchor;		    
		bool isUseSpiritRegion;			
		MathType::vec2f start;			
		MathType::vec2f size;			
	} AF, * LPAF;



	typedef struct AnimationClip
	{
		std::vector<AF> m_frames;
		float m_frameRate = 60.0f;		
		bool m_isLoop = false;				
	public:
		bool AddFrames(const std::vector<std::tuple<std::wstring, MathType::vec2f, UINT, UINT>> tasks);
		bool AddFrames(const std::vector<std::tuple<std::wstring, float, float, UINT, UINT>> tasks);

		int GetFrameIndex(float time) const;
		float GetTotalDuration() const; 				
	}AC, * LPAC;


	typedef enum class AnimationStatus
	{
		AS_DEFAULT = 0,
		AS_PLAYING = 1,
		AS_PAUSED = 2,
		AS_STOPPED = 3
	}AS;


	typedef class Animator : public RenderComponent
	{
		std::unordered_map<std::wstring, AnimationClip> m_clips;
		AnimationClip* m_currentClip = nullptr;
		std::wstring m_currentAnikey = L"";

		float m_currentTime = 0.0f;

		GamePlay::AS m_status = GamePlay::AS::AS_DEFAULT;
		
		class SpriteRender* m_render = nullptr;
	public:
		Animator(class GameObject* pObj);
		virtual ~Animator();
		void AddClip(const wchar_t* clipName, const AnimationClip& clip);
		void AddClipV(const std::vector<std::pair<const wchar_t*, AnimationClip>>& clips);
		void Play(const wchar_t* clipName);
		void Continue(const wchar_t* clipName); 
		void Stop();
		void Pause();
		void RePlay();
		void Update(float delta);
		GamePlay::AS GetState();
		virtual const int& OrderInLayer() const override;
		virtual void Render(const MathType::Mat3x3f& vpv)override;
		virtual const MathType::RectF& GetRenderRect() const override;
		
		virtual void CaclRenderMatrix();
		void SetOrderInLayer(int layerorder);
		void SetOpcity(float op);
		float Opacity() const;
		/**
		 * @brief 设置着色颜色（分通道）
		 *
		 * 转发到内部SpriteRender组件
		 * @param r 红色分量（0~1）
		 * @param g 绿色分量（0~1）
		 * @param b 蓝色分量（0~1）
		 * @param a Alpha分量/着色强度（0~1），0表示不着色
		 */
		void SetTintColor(float r, float g, float b, float a);

		void OnDeBugPlay();

		const GamePlay::AC& GetCurrentClip() const;
		const GamePlay::AF& GetCurrentFrame() const;
		const std::wstring& GetCurrentAniKey() const;
		int GetCurrentFrameIndex() const;
	} ANI, * LPANI;
}


#endif 
