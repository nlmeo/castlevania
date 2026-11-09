#include "SelectDataScene.h"
#include "./GameTool.h"
#include "Player.h"
#include "Soma.h"
#include "BackPack.h"
#include "SaveManager.h"
#include "GamePlay/Debug.h"
void SelectDataScene::OnEnter()
{
	mStartingGame = false;
	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	int order = 0;

	m_cur_opt_index = 0;
	m_copy_src = -1;
	if (mDescriptionFont < 0)
		mDescriptionFont = FontMgr.CreateFontFromFile(L"resource\\font\\Chinese.ttf", 28.0f);
	if (mPreviewFont < 0)
		mPreviewFont = FontMgr.CreateFontFromFile(L"resource\\font\\Chinese.ttf", 28.0f);
	if (mNewGameFont < 0)
		mNewGameFont = FontMgr.CreateFontFromFile(L"resource\\font\\English.ttf", 56.0f);
	auto* canvas = GetUICanvas();
	mDescription = new GameUI::UITextArea();
	mDescription->Create(L"", mDescriptionFont, ColorType::COLORF::White(), 80, 435, 600, 60);
	canvas->AddChild(mDescription);
	mSavePreview = new GameUI::UITextArea();
	mSavePreview->Create(L"", mPreviewFont, ColorType::COLORF::White(), 440, 145, 290, 180);
	canvas->AddChild(mSavePreview);
	RefreshSlotText();

	m_options.clear();
	m_options.push_back({ half_w -337 + 38,-257 + 188 });
	m_options.push_back({ half_w -337 + 38 + (16 * 6),-257 + 188 });
	m_options.push_back({ half_w -337 + 38 + (16 * 6 * 2),-257 + 188 });
	
	
	//相机
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_camera->transform());
	pcam->SetCameraBound({ 0,0,cw,ch });


	//backgournd
	GT::DrawPicture(L"resource\\ui\\top\\frame078.bmp", { half_w,0 }, order++, { 0,0 }, 0x00FF00);
	//GT::DrawPicture(L"resource\\ui\\start\\frame016.bmp", { (cw / 2),0 }, order++, { 0,0 }, 0x00FF00);

	{
		m_pointer = GT::CreateGameObject<GamePlay::GameObject>();
		GamePlay::AnimationClip clip;
		clip.AddFrames({
			{L"resource\\ui\\top\\frame048.bmp",337.0f,257.0f,31,0x00FF00},
			{L"resource\\ui\\top\\frame049.bmp",337.0f,257.0f,4, 0x00FF00},
			{L"resource\\ui\\top\\frame04A.bmp",337.0f,257.0f,4, 0x00FF00},
			{L"resource\\ui\\top\\frame04B.bmp",337.0f,257.0f,4, 0x00FF00},
			{L"resource\\ui\\top\\frame04C.bmp",337.0f,257.0f,4, 0x00FF00},
			{L"resource\\ui\\top\\frame04D.bmp",337.0f,257.0f,4, 0x00FF00},
			});
		clip.m_isLoop = true;
		GT::PlayAnimation(clip, L"pointer_anim", order++, m_pointer);
		m_pointer->transform().SetPosition({ half_w + m_options[0].first, half_h + m_options[0].second });
	}

	//GO::PlayAnimation
	{
		m_title = GT::CreateGameObject<GamePlay::GameObject>();
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{L"resource\\ui\\top\\frame002.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame003.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame004.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame005.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame006.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame007.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame008.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame009.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame00A.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame00B.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame00C.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame00D.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame00E.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame00F.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame010.bmp",-108.0f, -113.0f, 3, 0x00FF00},
				{L"resource\\ui\\top\\frame011.bmp",-108.0f, -113.0f, 3, 0x00FF00},
			});
		clip.m_isLoop = true;
		GT::PlayAnimation(clip, L"title_anim",order++, m_title);
		m_title->transform().SetPosition({half_w*2, half_h});
	}
}

void SelectDataScene::OnExit()
{
	auto* canvas = GetUICanvas();
	if (mDescription) { canvas->RemoveChild(mDescription); mDescription = nullptr; }
	if (mSavePreview) { canvas->RemoveChild(mSavePreview); mSavePreview = nullptr; }
	// 新玩家、碰撞子对象和魂技能在待加入队列中，进入游戏时保留。
	GameObjectMgr.ClearAllObjects({}, !mStartingGame);
	mStartingGame = false;
}

void SelectDataScene::RefreshSlotText()
{
	const bool hasSave = SaveMgr.HasSave(m_cur_opt_index);
	if (mSavePreview)
	{
		if (hasSave)
		{
			const auto time = SaveMgr.GetSaveTime(m_cur_opt_index);
			mSavePreview->Create(L"地点：地图上消失的村庄\n\nTIME：\n" +
				std::wstring(time.begin(), time.end()), mPreviewFont,
				ColorType::COLORF::White(), 420, 120, 310, 220);
			mSavePreview->SetAlignment(DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
		else
		{
			mSavePreview->Create(L"New Game", mNewGameFont,
				ColorType::COLORF::White(), 430, 80, 310, 270);
			mSavePreview->SetAlignment(DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}
	if (!mDescription) return;
	switch (SaveMgr.GetMode())
	{
	case SaveManager::Mode::Copy:
		mDescription->SetText(m_copy_src < 0 ? L"请选择要复制的存档。" : L"请选择一个空存档槽作为复制目标。");
		break;
	case SaveManager::Mode::Delete:
		mDescription->SetText(hasSave ? L"删除此存档，删除后无法恢复。" : L"此存档槽为空。");
		break;
	default:
		mDescription->SetText(hasSave ? L"读取此存档继续游戏" : L"开始新的游戏");
		break;
	}
}

void SelectDataScene::Update(float deltaTimeSecond)
{

	int i1, i2, i3, count;
	count = m_options.size();
	i1 = m_cur_opt_index;

	if (GT::KeyJustDown('A'))
	{
		i1 -= 1;
		GT::PlaySE(SE_MENU_CHANGE);
	}
	else if (GT::KeyJustDown('D'))
	{
		i1 += 1;
		GT::PlaySE(SE_MENU_CHANGE);
	}
	// 让索引永远合法（万能环形公式）
	i1 = (i1 % count + count) % count; //负数取余 + count 再取余，保证结果在0到count-1之间，无论i1是正数还是负数都能正确处理。
	m_cur_opt_index = i1;
	if (GT::KeyJustDown('A') || GT::KeyJustDown('D')) RefreshSlotText();


	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	m_pointer->transform().SetPosition({ half_w + m_options[i1].first, half_h + m_options[i1].second });


	if (GT::KeyJustDown('J'))
	{
		GT::PlaySE(SE_MENU_CHOOSE);

		SaveManager::Mode mode = SaveMgr.GetMode();

		if (mode == SaveManager::Mode::Delete)
		{
			// 删除模式：清除当前选中槽位的存档
			if (SaveMgr.DeleteSave(m_cur_opt_index))
				Debug.Log2Screen("已清除存档槽 %d", 3.0f, ColorType::COLORF::Yellow(), m_cur_opt_index + 1);
			else
				Debug.Log2Screen("存档槽 %d 为空", 3.0f, ColorType::COLORF::Gray(), m_cur_opt_index + 1);
		}
		else if (mode == SaveManager::Mode::Copy)
		{
			// 复制模式：第一步选源槽，第二步选目标槽
			if (m_copy_src < 0)
			{
				if (SaveMgr.HasSave(m_cur_opt_index))
				{
					m_copy_src = m_cur_opt_index;
					Debug.Log2Screen("已选源存档槽 %d，请选择目标槽", 3.0f, ColorType::COLORF::Yellow(), m_cur_opt_index + 1);
				}
				else
				{
					Debug.Log2Screen("存档槽 %d 为空", 3.0f, ColorType::COLORF::Gray(), m_cur_opt_index + 1);
				}
			}
			else
			{
				if (m_copy_src == m_cur_opt_index)
				{
					Debug.Log2Screen("目标槽与源槽相同", 3.0f, ColorType::COLORF::Gray());
				}
				else if (SaveMgr.CopySave(m_copy_src, m_cur_opt_index))
				{
					Debug.Log2Screen("已复制存档槽 %d 到槽 %d", 3.0f, ColorType::COLORF::Yellow(), m_copy_src + 1, m_cur_opt_index + 1);
					m_copy_src = -1;
				}
				else
				{
					Debug.Log2Screen("复制失败（目标槽已有存档）", 3.0f, ColorType::COLORF::Red());
				}
			}
		}
		else
		{
			// 正常模式：记录槽位并读档/新建
			SaveMgr.SetCurrentSlot(m_cur_opt_index);

			Soma* m_player = GT::CreateGameObject<Soma>();
			m_player->layer() = GamePlay::GameLayer::Layer_4;
			Player::SetMainPlayer(m_player);

			if (SaveMgr.HasSave(m_cur_opt_index))
			{
				// 读档：恢复属性/装备/背包并进入存档记录的场景
				if (!SaveMgr.LoadGame(m_cur_opt_index))
				{
					m_player->Destroy();
					Player::SetMainPlayer(nullptr);
					Debug.Log2Screen("存档无效或读取失败，请选择其他槽位", 3.0f, ColorType::COLORF::Red());
					return;
				}
				mStartingGame = true;
				SceneMgr.SwitchTo(SaveMgr.LoadedScene(), [bgm = SaveMgr.LoadedBGM()] {
					GT::StopAllMusic();
					if (!bgm.empty()) GT::PlayBGM(bgm.c_str(), true);
				});
			}
			else
			{
				mStartingGame = true;
				// 新游戏：默认位置 + 默认背包，进入初始关卡
				m_player->transform().SetPosition({ 600,350 });
				BackPack.Clear();
				BackPack.InitTestData();
				GT::SwitchTo("s0");
			}
		}
		if (!mStartingGame) RefreshSlotText();
	}
	else if (GT::KeyJustDown('K'))
	{
		GT::PlaySE(SE_MENU_BACK);
		m_copy_src = -1;
		GT::SwitchTo("top");
	}
}

void SelectDataScene::Render(float deltaTimeSecond)
{
}

void SelectDataScene::RenderUICanvas(float dt)
{
	RenderUICanvasInWorld(dt, 384.0f, 0.0f, 768, 576);
}

void SelectDataScene::OnKeyDown(WPARAM key)
{
}
