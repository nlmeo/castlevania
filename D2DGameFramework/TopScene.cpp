#include "TopScene.h"
#include "GameTool.h"
#include "SaveManager.h"
namespace
{
	constexpr const wchar_t* MenuDescriptions[] = {
		L"选择存档继续游戏，或在空存档槽开始新游戏。",
		L"将已有存档复制到另一个空存档槽。",
		L"删除选中的存档，删除后无法恢复。"
	};
}

void TopScene::OnEnter()
{
	if (mDescriptionFont < 0)
		mDescriptionFont = FontMgr.CreateFontFromFile(L"resource\\font\\Chinese.ttf", 28.0f);
	if (mHintFont < 0)
		mHintFont = FontMgr.CreateFontFromFile(L"resource\\font\\Chinese.ttf", 18.0f);

	auto* canvas = GetUICanvas();
	mDescription = new GameUI::UITextArea();
	mDescription->Create(MenuDescriptions[m_cur_opt_index], mDescriptionFont,
		ColorType::COLORF::White(), 80, 430, 600, 54);
	canvas->AddChild(mDescription);
	mKeyHint = new GameUI::UIText();
	mKeyHint->Create(L"W / S  选择     J  确认     K  返回", mHintFont,
		ColorType::COLORF::White(), 80, 490);
	canvas->AddChild(mKeyHint);

	UINT order = 0;
	float cw = 768 * 2;
	float ch = 576;
	float half_w = 384;
	//backgournd
	GT::DrawPicture(L"resource\\ui\\top\\frame077.bmp", { half_w,0 }, order++, { 0,0 }, 0x00FF00);
	//GT::DrawPicture(L"resource\\ui\\start\\frame016.bmp", { (cw / 2),0 }, order++, { 0,0 }, 0x00FF00);
	//相机
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_camera->transform());
	pcam->SetCameraBound({ 0,0,cw,ch });

	m_options.clear();
	// SELECT DATA
	{
		GamePlay::GameObject* obj = GT::CreateGameObject<GamePlay::GameObject>();
		GT::DrawPicture(L"resource\\ui\\top\\frame001.bmp", { half_w,0 }, order++, { 0.5f - (64/792.0f),0.5f- (135/567.0f) }, 0x00FF00, obj);
		
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{L"resource\\ui\\top\\frame002.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame003.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame004.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame005.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame006.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame007.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame008.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame009.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame00A.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame00B.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame00C.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame00D.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame00E.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame00F.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame010.bmp",64, 135,3,0x00FF00},
				{L"resource\\ui\\top\\frame011.bmp",64, 135,3,0x00FF00},
			});
		clip.m_isLoop = true;
		GT::PlayAnimation(clip, L"option_anim", order++, obj);
		obj->transform().SetPosition({ half_w + (256 * 3 / 2) - 64 - 12, (192 * 3 / 2) - 135 - 29 });

		m_options.push_back(obj);
	}

	// COPY DATA
	{
		GamePlay::GameObject* obj = GT::CreateGameObject<GamePlay::GameObject>();
		GT::DrawPicture(L"resource\\ui\\top\\frame012.bmp", { half_w,0 }, order++, { 0.5f-(-8 / 792.0f),0.5f-(39 / 567.0f) }, 0x00FF00, obj);

		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{L"resource\\ui\\top\\frame013.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame014.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame015.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame016.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame017.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame018.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame019.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame01A.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame01B.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame01C.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame01D.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame01E.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame01F.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame020.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame021.bmp",-8, 39,3,0x00FF00},
				{L"resource\\ui\\top\\frame022.bmp",-8, 39,3,0x00FF00},
			});
		clip.m_isLoop = true;
		GT::PlayAnimation(clip, L"option_anim", order++, obj);
		obj->transform().SetPosition({ half_w+(256 * 3 / 2) + 8 - 12, (192 * 3 / 2) - 39 - 29 });

		m_options.push_back(obj);
	}

	// DELETE DATA
	{
		GamePlay::GameObject* obj = GT::CreateGameObject<GamePlay::GameObject>();
		GT::DrawPicture(L"resource\\ui\\top\\frame023.bmp", { half_w,0 }, order++, { 0.5f-(-80 / 792.0f),0.5f-(-57 / 567.0f) }, 0x00FF00, obj);

		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{L"resource\\ui\\top\\frame024.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame025.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame026.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame027.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame028.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame029.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame02A.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame02B.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame02C.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame02D.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame02E.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame02F.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame030.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame031.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame032.bmp",-80, -57,3,0x00FF00},
				{L"resource\\ui\\top\\frame033.bmp",-80, -57,3,0x00FF00},
			});
		clip.m_isLoop = true;
		GT::PlayAnimation(clip, L"option_anim", order++, obj);
		obj->transform().SetPosition({ half_w +(256 * 3 / 2) + 80 - 12, (192 * 3 / 2) + 57 - 29 });

		m_options.push_back(obj);
	}




}

void TopScene::OnExit()
{
	auto* canvas = GetUICanvas();
	if (mDescription) { canvas->RemoveChild(mDescription); mDescription = nullptr; }
	if (mKeyHint) { canvas->RemoveChild(mKeyHint); mKeyHint = nullptr; }
	GT::ClearAllGameObject();
}

void TopScene::Update(float deltaTimeSecond)
{
	int i1, count;

	count = m_options.size();
	i1 = m_cur_opt_index;

	if (GT::KeyJustDown('W'))
	{
		i1 -= 1;
		GT::PlaySE(SE_MENU_CHANGE);
	}
	else if (GT::KeyJustDown('S'))
	{
		i1 += 1;
		GT::PlaySE(SE_MENU_CHANGE);
	}

	// 让索引永远合法（万能环形公式）
	i1 = (i1 % count + count) % count; //负数取余 + count 再取余，保证结果在0到count-1之间，无论i1是正数还是负数都能正确处理。
	m_cur_opt_index = i1;
	if (mDescription) mDescription->SetText(MenuDescriptions[m_cur_opt_index]);


	for (int i = 0; i < count; i++)
	{
		if (i == m_cur_opt_index)
		{
			m_options[i]->GetComponent<GamePlay::SpriteRender>()->SetOpcity(0);			
			m_options[i]->GetComponent<GamePlay::Animator>()->SetOpcity(1);
		}
		else
		{
			m_options[i]->GetComponent<GamePlay::SpriteRender>()->SetOpcity(1);
			m_options[i]->GetComponent<GamePlay::Animator>()->SetOpcity(0);
		}
	}


	if (GT::KeyJustDown('J'))
	{
		GT::PlaySE(SE_MENU_CHOOSE);
		// 0=SELECT DATA 1=COPY DATA 2=DELETE DATA，与 SaveManager::Mode 枚举一一对应
		SaveMgr.SetMode((SaveManager::Mode)m_cur_opt_index);
		GT::SwitchTo("select_data");
	}
	else if (GT::KeyJustDown('K'))
	{
		GT::PlaySE(SE_MENU_BACK);
		GT::SwitchTo("start");
	}
}

void TopScene::Render(float deltaTimeSecond)
{
}

void TopScene::RenderUICanvas(float dt)
{
	// The 768x576 menu background starts at world position (384, 0).
	RenderUICanvasInWorld(dt, 384.0f, 0.0f, 768, 576);
}

void TopScene::OnKeyDown(WPARAM key)
{
}
