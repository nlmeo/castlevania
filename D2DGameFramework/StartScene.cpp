#include "StartScene.h"
#include "GameTool.h"
#include "./GameCore/InputMgr.h"
#include "./GameCore/AudioLib.h"
#include "BackPack.h"
void StartScene::OnEnter()
{
	// 关卡之间保留玩家；返回标题意味着结束本次会话，先清掉旧世界再创建标题对象。
	GT::ClearAllGameObject();
	BackPack.Clear();
	m_options.clear();
	// GAME START
	{
		std::pair<std::wstring, std::wstring> p(L"resource\\ui\\start\\frame001.bmp", L"resource\\ui\\start\\frame017.bmp");
		m_options.push_back(p);
	}

	// BOSS RUSH MODE
	{
		std::pair<std::wstring, std::wstring> p(L"resource\\ui\\start\\frame004.bmp", L"resource\\ui\\start\\frame019.bmp");
		m_options.push_back(p);
	}

	// SOUND CONFIG
	{
		std::pair<std::wstring, std::wstring> p(L"resource\\ui\\start\\frame005.bmp", L"resource\\ui\\start\\frame018.bmp");
		m_options.push_back(p);
	}

	float cw = 768 * 2;
	float ch = 576;
	UINT order = 0;
	//GT::DrawPicture(L"resource\\ui\\start\\frame014.bmp", { (cw / 2 - ch) / 2 ,0 },						 order++);
	//GT::DrawPicture(L"resource\\ui\\start\\frame013.bmp", { (cw / 2 / 2) - 363 / 2, (ch / 2) - 84 / 2 }, order++);
	//GT::DrawPicture(L"resource\\ui\\start\\frame012.bmp", { (cw / 2 / 2) - 768 / 2, 36 },				 order++);
	//GT::DrawPicture(L"resource\\ui\\start\\frame016.bmp", { (cw / 2), 0 },							     order++);


	GT::DrawPicture(L"resource\\ui\\start\\frame014.bmp", { (cw - ch) / 2 ,0 }, order++);
	GT::DrawPicture(L"resource\\ui\\start\\frame013.bmp", { (cw / 2) - 363 / 2, (ch / 2) - 84 / 2 }, order++);
	GT::DrawPicture(L"resource\\ui\\start\\frame012.bmp", { (cw / 2) - 768 / 2, 36 }, order++);
	//GT::DrawPicture(L"resource\\ui\\start\\frame016.bmp", { 0, 0 }, order++);


	pCamera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = pCamera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f/3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&pCamera->transform());
	pcam->SetCameraBound({ 0,0,cw,ch });


	pOption1 = GT::DrawPicture(m_options[2].second.c_str(), { (cw / 2) - 576 / 2, 110 }, order++);
	pOption2 = GT::DrawPicture(m_options[0].first.c_str(),  { (cw / 2) - 576 / 2, 160 }, order++);
	pOption3 = GT::DrawPicture(m_options[1].second.c_str(), { (cw / 2) - 576 / 2, 210 }, order++);

	GT::DrawPicture(L"resource\\ui\\start\\frame011.bmp", { (cw / 2) - 576 / 2, 160 }, order++);
	GT::DrawPicture(L"resource\\ui\\start\\frame009.bmp", { (cw / 2) - 576 / 2, 90 }, order++);
	GT::DrawPicture(L"resource\\ui\\start\\frame010.bmp", { (cw / 2) - 576 / 2, 230 }, order++);
	
	AudioLib.StopAllMusic();
	AudioLib.PlaySE(SE_MENU_INTRO);
}

void StartScene::OnExit()
{
	GT::ClearAllGameObject();
}

void StartScene::Update(float deltaTimeSecond)
{
	int i1, i2, i3, count;

	count = m_options.size();
	i1 = m_cur_opt_index;

	if (GameInput.KeyJustDown('W'))
	{
		i1 -= 1;
		AudioLib.PlaySE(SE_MENU_CHANGE);
	}

	else if (GameInput.KeyJustDown('S'))
	{
		i1 += 1;
		AudioLib.PlaySE(SE_MENU_CHANGE);
	}

	// 让索引永远合法（万能环形公式）
	i1 = (i1 % count + count) % count; //负数取余 + count 再取余，保证结果在0到count-1之间，无论i1是正数还是负数都能正确处理。
	i2 = (i1 + 1) % count;			   //下一个索引，直接对count取余即可，因为i1已经是合法索引了。
	i3 = (i1 - 1 + count) % count;     //上一个索引，先减1可能会得到负数，所以加上count再取余，保证结果在0到count-1之间。

	m_cur_opt_index = i1; //更新当前选项索引



	pOption1->SetTexture(TexMgr.LoadTextureWithoutColor(m_options[i3].second.c_str(), RGB(0, 255, 0)));
	pOption2->SetTexture(TexMgr.LoadTextureWithoutColor(m_options[i1].first.c_str(), RGB(0, 255, 0)));
	pOption3->SetTexture(TexMgr.LoadTextureWithoutColor(m_options[i2].second.c_str(), RGB(0, 255, 0)));


	if (GT::KeyJustDown('J'))
	{
		GT::PlaySE(SE_MENU_CHOOSE);
		//进入场景
		switch (m_cur_opt_index)
		{
			case 0:
			{
				GT::SwitchTo("top");
				//选项值
				GT::PlayBGM(BGM_GLOOMY_MEMORIES,true);// BGM_GLOOMY_MEMORIES);
				break;
			}
			//case 1:
			//	GT::SwitchTo("s0");
			//	break;
			case 2:
			{
				GT::SwitchTo("sound_config");
				GT::PlayBGM(BGM_GLOOMY_MEMORIES,true);
				break;
			}
		}

	}

}

void StartScene::Render(float deltaTimeSecond)
{
}

void StartScene::OnKeyDown(WPARAM key)
{

}
