#pragma once
#ifndef __GAME_UI_CHECKBOX_H__
#define __GAME_UI_CHECKBOX_H__

#include "UIElement.h"
#include "../D2DCore/Texture.h"
#include <functional>

namespace GameUI
{
	/**
	 * @brief ???????
	 *
	 * ?????????????2????????????(Unchecked/Checked)??
	 */
	class UICheckBox : public UIElement
	{
	public:
		UICheckBox();

		virtual ~UICheckBox();

		bool Create(float x, float y, float width, float height,
			D2DCore::Texture* pTexture, bool checked = false);

		bool IsChecked() const;

		void SetChecked(bool checked);

		void SetOnCheckedChanged(std::function<void(bool)> callback);

		virtual void Render() override;

		virtual void Update(float deltaTime) override;

		virtual void OnMouseDown(int x, int y) override;

	protected:
		D2DCore::Texture* mPTexture;                 
		bool mChecked;                               
		std::function<void(bool)> mOnCheckedChanged; 
	};
}

#endif
