#include "SoundConfigScene.h"
#include "./GameTool.h"
void SoundConfigScene::OnEnter()
{
	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;

	m_options.clear();
	m_options.push_back({ -337 + 38 + (16 * 6) + half_w, -257 + 174 - (16 * 3) });
	m_options.push_back({ -337 + 38 + (16 * 6) + half_w, -257 + 174 });
	m_options.push_back({ -337 + 38 + (16 * 6) + half_w, -257 + 174 + (16 * 3) });
	
	m_option_num.clear();
	m_option_num.push_back((GT::GetBGMVolume() * 32));
	m_option_num.push_back((GT::GetVoiceVolume() * 32));
	m_option_num.push_back((GT::GetSEVolume() * 32));
	
	
	UINT order = 0;
	//backgournd
	GT::DrawPicture(L"resource\\ui\\start\\frame020.bmp", { half_w,0 }, order++, { 0,0 }, 0x00FF00);
	//GT::DrawPicture(L"resource\\ui\\start\\frame016.bmp", { (cw / 2),0 }, order++, { 0,0 }, 0x00FF00);
	for (int i = 0; i < 32; i++)
	{
		m_bgm_objs[i] = GT::DrawPicture(L"resource\\ui\\start\\frame022.bmp", { 0, 0 }, order++, { 0,0 }, 0x00FF00);
		m_bgm_objs[i]->SetOrderInLayer(2+ i);
		m_bgm_objs[i]->gameobject()->transform().SetPosition({ 359.0f + i * 6 + half_w, 149.0f });
		if (i > (int)(GT::GetBGMVolume() * 32))
		{
			m_bgm_objs[i]->SetOpcity(0);
		}

		m_voice_objs[i] = GT::DrawPicture(L"resource\\ui\\start\\frame022.bmp", { 0, 0 }, order++,{ 0,0 }, 0x00FF00);
		m_voice_objs[i]->SetOrderInLayer(2 + 32+ i);
		m_voice_objs[i]->gameobject()->transform().SetPosition({ 359.0f + i * 6 + half_w, 197.0f });
		if (i > (int)(GT::GetVoiceVolume() * 32))
		{
			m_voice_objs[i]->SetOpcity(0);
		}

		m_se_objs[i] = GT::DrawPicture(L"resource\\ui\\start\\frame022.bmp", { 0, 0 }, order++, { 0,0 }, 0x00FF00);
		m_se_objs[i]->SetOrderInLayer(2 + 64+ i);
		m_se_objs[i]->gameobject()->transform().SetPosition({ 359.0f + i * 6 + half_w, 245.0f });
		if (i > (int)(GT::GetSEVolume() * 32))
		{
			m_se_objs[i]->SetOpcity(0);
		}
	}
	
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
		GT::PlayAnimation(clip, L"pointer_anim",order++, m_pointer);

	}
	


	m_a1 = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::AnimationClip clip;
	clip.AddFrames({
		{L"resource\\ui\\start\\frame021.bmp",          0, 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-1 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-2 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-3 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-4 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-5 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-4 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-3 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-2 * 3.0f), 0,4, 0x00FF00},
		{L"resource\\ui\\start\\frame021.bmp",(-1 * 3.0f), 0,4, 0x00FF00},
		});

	GT::PlayAnimation(clip, L"m_a1",order++, m_a1);
	m_a1->transform().SetPosition({half_w + m_options[0].first + 150, half_h + m_options[0].second - 3});


	m_a2 = GT::CreateGameObject<GamePlay::GameObject>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
			{L"resource\\ui\\start\\frame021.bmp",     0, 0,4,0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-5 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			});
		GT::PlayAnimation(clip, L"m_a2",order++, m_a2);
	}
	m_a2->transform().SetPosition({ half_w + m_options[1].first + 150, half_h + m_options[1].second - 3 });

	m_a3 = GT::CreateGameObject<GamePlay::GameObject>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
			{L"resource\\ui\\start\\frame021.bmp",     0, 0,4,0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-5 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			});
		GT::PlayAnimation(clip, L"m_a3",order++, m_a3);
	}
	m_a3->transform().SetPosition({ half_w + m_options[2].first + 150, half_h + m_options[2].second - 3 });

	m_a4 = GT::CreateGameObject<GamePlay::GameObject>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
			{L"resource\\ui\\start\\frame021.bmp",     0, 0,4,0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-5 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			});
		GT::PlayAnimation(clip, L"m_a4",order++, m_a4);
	}
	m_a4->transform().SetScale({ -1.0f, 1.0f });
	m_a4->transform().SetPosition({ half_w + m_options[0].first + 398, half_h + m_options[0].second - 3 });
	

	m_a5 = GT::CreateGameObject<GamePlay::GameObject>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
			{L"resource\\ui\\start\\frame021.bmp",     0, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-5 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			});
		GT::PlayAnimation(clip, L"m_a5",order++, m_a5);
	}
	m_a5->transform().SetScale({ -1.0f, 1.0f });
	m_a5->transform().SetPosition({ half_w + m_options[1].first + 398, half_h + m_options[1].second - 3 });



	m_a6 = GT::CreateGameObject<GamePlay::GameObject>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
			{L"resource\\ui\\start\\frame021.bmp",     0, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-5 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-4 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-3 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-2 * 3, 0,4, 0x00FF00},
			{L"resource\\ui\\start\\frame021.bmp",-1 * 3, 0,4, 0x00FF00},
			});
		GT::PlayAnimation(clip, L"m_a6",order++, m_a6);
	}
	m_a6->transform().SetScale({ -1.0f, 1.0f });
	m_a6->transform().SetPosition({ half_w + m_options[2].first + 398, half_h + m_options[2].second - 3 });

	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_camera->transform());
	pcam->SetCameraBound({ 0,0,cw,ch });
	
}

void SoundConfigScene::OnExit()
{
	GT::ClearAllGameObject();
}

void SoundConfigScene::Update(float deltaTimeSecond)
{
	int i1, i2, i3, count;
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
	i1 = (i1 % count + count) % count; 
	m_cur_opt_index = i1;
	
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	m_pointer->transform().SetPosition({ half_w + m_options[i1].first, half_h + m_options[i1].second });



	if (GT::KeyDown('A'))
	{
		if (m_option_num[i1] == 0)
		{
			GT::PlaySE(SE_MENU_WRONG);
		}
		else
		{
			m_option_num[i1]--;
			GT::PlaySE(SE_MENU_CHANGE);
		}

	}
	else if (GT::KeyDown('D'))
	{
		if (m_option_num[i1] == 32)
		{
			GT::PlaySE(SE_MENU_WRONG);
		}
		else
		{
			m_option_num[i1]++;
			GT::PlaySE(SE_MENU_CHANGE);
		}

	}

	for (int i = 0; i < 32; i++)
	{
		m_bgm_objs[i]->SetOpcity((i >= m_option_num[0]) ? 0 : 1);
		m_voice_objs[i]->SetOpcity((i >= m_option_num[1]) ? 0 : 1);
		m_se_objs[i]->SetOpcity((i >= m_option_num[2]) ? 0 : 1);
	}

	GT::SetBGMVolume(m_option_num[0] / 32.0f);
	GT::SetVoiceVolume(m_option_num[1] / 32.0f);
	GT::SetSEVolume(m_option_num[2] / 32.0f);

	

	if (GT::KeyJustDown('K'))
	{
		GT::PlaySE(SE_MENU_BACK);
		GT::SwitchTo("start");
	}


	//if (GO::KeyDown(VK_UP))
	//{
	//	m_camera->transform().translate(MathType::vec2f::down() * deltaTimeSecond * 100);
	//}
	//else if (GO::KeyDown(VK_DOWN))
	//{
	//	m_camera->transform().translate(MathType::vec2f::up() * deltaTimeSecond * 100);
	//}

	//if (GO::KeyDown(VK_LEFT))
	//{
	//	m_camera->transform().translate(MathType::vec2f::left() * deltaTimeSecond * 100);
	//}
	//else if (GO::KeyDown(VK_RIGHT))
	//{
	//	m_camera->transform().translate(MathType::vec2f::right() * deltaTimeSecond * 100);
	//}

	//if (GO::KeyDown(VK_PRIOR))
	//{
	//	float sizey = MainCamera->SizeY();
	//	MainCamera->SetSizeY(sizey += deltaTimeSecond * 100);
	//}
	//else if (GO::KeyDown(VK_NEXT))
	//{
	//	float sizey = MainCamera->SizeY();
	//	MainCamera->SetSizeY(sizey -= deltaTimeSecond * 100);
	//}


}

void SoundConfigScene::Render(float deltaTimeSecond)
{
}

void SoundConfigScene::OnKeyDown(WPARAM key)
{
}
