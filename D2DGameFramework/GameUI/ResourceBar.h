#pragma once
#include "UIImage.h"
#include <algorithm>
#include <cmath>

namespace GameUI
{
	inline void SetResourceBarValue(UIImage* bar, float current, float maximum, float fullWidth = 192.0f)
	{
		if (!bar) return;
		const float ratio = maximum > 0 && std::isfinite(maximum) && std::isfinite(current)
			? std::clamp(current / maximum, 0.0f, 1.0f) : 0.0f;
		bar->SetSize({fullWidth * ratio, bar->GetSize().y});
	}
}
