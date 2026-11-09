#include "Scene.h"
#include "GameLoop.h"
#include "../GameCore/Application.h"
#include "../GamePlay/Camera2d.h"
#include "../D2DCore/D2DRender.h"

namespace GameCore
{
	Scene::Scene():m_isActive(false), mUICanvas(nullptr)
	{
		InitializeUICanvas(App.GetWidth(), App.GetHeight());
	}

	float Scene::deltaTime() const
	{
		return static_cast<float>(GameLooper.frameDeltaTime());
	}

	void Scene::RenderUICanvasInWorld(float dt, float originX, float originY, int width, int height)
	{
		auto* target = D2DRd.GetD2DRenderTarget();
		auto* camera = GamePlay::Camera::main();
		if (!mUICanvas || !target || !camera) return;

		// WM_SIZE resizes canvases in screen pixels; these menus keep logical layout units.
		mUICanvas->Resize(width, height);
		mUICanvas->Update(dt);
		MathType::Mat3x3f origin;
		origin.SetTranslate(originX, originY);
		auto transform = origin * camera->GetVPV();
		D2D1_MATRIX_3X2_F previous;
		target->GetTransform(&previous);
		target->SetTransform(reinterpret_cast<const D2D1_MATRIX_3X2_F*>(transform.GetMat3x2()));
		mUICanvas->Render();
		target->SetTransform(previous);
	}
}
