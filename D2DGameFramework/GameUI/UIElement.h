#pragma once
#ifndef __GAME_UI_ELEMENT_H__
#define __GAME_UI_ELEMENT_H__

#include "../TypeDefine/Vector2.h"
#include "../TypeDefine/Geometric.h"
#include "../TypeDefine/Color.h"
#include <vector>
#include <memory>
#include <functional>

namespace GameUI
{
	typedef enum class emUIElementType
	{
		UI_Base,
		UI_Button,
		UI_CheckBox,
		UI_SliderBar,
		UI_Window,
		UI_Canvas,
		UI_ScrollView,
		UI_Image,
		UI_Number,
		UI_Text,
		UI_TextArea,
		UI_Animator
	}UIElementType;

	class UIElement
	{
	public:
		UIElement();

		virtual ~UIElement();

		virtual void Render() = 0;

		virtual void Update(float deltaTime) = 0;

		virtual void CalcWorldRect();

		MathType::RectF GetWorldRect() const;

		MathType::RectF GetSelfRect() const;

		bool IsActive() const;

		void SetActive(bool active);

		bool IsVisible();

		UIElementType GetType() const;

		unsigned int GetUID() const;

		void SetPosition(const MathType::vec2f& pos);

		void SetPosition(float x, float y);

		void SetSize(const MathType::vec2f& size);

		void SetSize(float w, float h);

		void SetAnchor(const MathType::vec2f& anchor);

		const MathType::vec2f& GetPosition() const;

		const MathType::vec2f& GetSize() const;

		virtual bool AddChild(UIElement* child);

		virtual bool RemoveChild(UIElement* child);

		virtual void RemoveAllChildren();

		UIElement* GetChild(unsigned int uid);

		virtual void OnMouseDown(int x, int y);

		virtual void OnMouseUp(int x, int y);

		virtual void OnMouseMove(int x, int y);

		virtual void OnMouseWheel(int delta);

		virtual void OnKeyDown(WPARAM key);

		virtual void OnKeyUp(WPARAM key);

		void SetParent(UIElement* parent);

		UIElement* GetParent() const;

		const std::vector<std::unique_ptr<UIElement>>& GetChildren() const;

		bool HitsTest(int x, int y) const;

		void SetOnClick(std::function<void()> callback);

	protected:
		MathType::vec2f mPosition;
		MathType::vec2f mSize;
		MathType::vec2f mAnchor;
		MathType::RectF mWorldRect;
		UIElement* mParent;
		std::vector<std::unique_ptr<UIElement>> mChildren;
		bool mIsActive;
		UIElementType mType;
		unsigned int mUID;

		std::function<void()> mOnClick;

		static unsigned int msNextUID;
		static unsigned int mUIElementCount;

		bool mFlipX = false;				   ///< 是否水平翻转
	};
}

#endif
