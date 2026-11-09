#pragma once
#ifndef __GAME_CONTENT_DEMO_SCENE_H__
#define __GAME_CONTENT_DEMO_SCENE_H__
#include "./D2DCore/Texture.h"
#include "./GameCore/Scene.h"
//using GameCore::Scene;//单独从命名空间中解放Scene
//using namespace GameCore;//无脑全部解放GameCore下的所有内容
#include "./GamePlay/SpriteRender.h"

/**
 * @class DemoScene
 * @brief 示例场景类
 * 
 * 继承自Scene类，提供游戏演示功能。展示了D2D渲染的基本用法，
 * 包括形状绘制、文本渲染、纹理显示等核心功能。
 */
class DemoScene:public GameCore::Scene
{
public:
    /**
     * @brief 析构函数
     */
	virtual ~DemoScene();

    /**
     * @brief 进入场景时调用
     */
	virtual void OnEnter() override;

    /**
     * @brief 退出场景时调用
     */
	virtual void OnExit() override;

    /**
     * @brief 每帧更新调用
     * @param deltaTimeSecond 距离上一帧的时间间隔（秒）
     */
	virtual void Update(float deltaTimeSecond) override;

    /**
     * @brief 每帧渲染调用
     * @param deltaTimeSecond 距离上一帧的时间间隔（秒）
     */
	virtual void Render(float deltaTimeSecond) override;

    /**
     * @brief 键盘按下事件处理
     * @param key 按下的键值
     */
	virtual void OnKeyDown(WPARAM key) override;
protected:
    /**
     * @brief 绘制标题
     * @param w 渲染区域宽度
     * @param h 渲染区域高度
     */
	void DrawTitle(float w, float h);

    /**
     * @brief 绘制基础形状演示
     * @param w 渲染区域宽度
     * @param h 渲染区域高度
     */
	void DrawBasicShapesDemo(float w, float h);

    /**
     * @brief 绘制文本演示
     * @param w 渲染区域宽度
     * @param h 渲染区域高度
     */
	void DrawTextDemo(float w, float h);

    /**
     * @brief 绘制纹理演示
     * @param w 渲染区域宽度
     * @param h 渲染区域高度
     */
	void DrawTextureDemo(float w, float h);

    /**
     * @brief 绘制操作说明
     * @param w 渲染区域宽度
     * @param h 渲染区域高度
     */
	void DrawInstructions(float w, float h);

private:
    /** @brief 普通字体ID */
	int m_normalFontId;

    /** @brief 粗体字体ID */
	int m_boldFontId;

    /** @brief 标题字体ID */
	int m_titleFontId;

    /** @brief 小号字体ID */
	int m_smallFontId;

    /** @brief 中文字体ID */
	int m_ChineseFontId;

    /** @brief 旋转角度（弧度） */
	float m_rotatin;

    /** @brief 时间累计（秒） */
	float m_time;

    /** @brief 测试用纹理指针 */
	class D2DCore::Texture* m_testTexture;

    /** @brief 位图纹理指针 */
	class D2DCore::Texture* m_bmpTexture;
};



#endif