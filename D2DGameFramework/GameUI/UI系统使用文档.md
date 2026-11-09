# D2DGameFramework UI 系统使用文档

## 一、架构概述

UI 系统位于 `GameUI` 命名空间下，是一套独立于 `GamePlay::Component` 组件体系的轻量级 UI 框架。所有 UI 控件继承自 `UIElement` 基类，形成树形层级结构。

### 类继承关系

```
UIElement (基类)
├── UICanvas    (顶级画布，管理所有UI)
├── UIButton    (按钮，三状态 Normal/Hover/Pressed)
├── UICheckBox  (复选框，双状态 Unchecked/Checked)
├── UISliderBar (滑动条，值域 0.0~1.0)
├── UIWindow    (可拖动弹窗，含标题栏)
└── UIScrollView(滚动视图，D2D裁剪渲染)
```

### 核心特性

| 特性 | 说明 |
|------|------|
| 树形层级 | 父子嵌套，`AddChild`/`RemoveChild` 管理 |
| 锚点系统 | 每个 UIElement 可设置锚点，影响子控件相对偏移 |
| 世界矩形 | 自动计算屏幕空间矩形 `CalcWorldRect()`，支持 `HitsTest` |
| 事件路由 | 鼠标/键盘事件自顶向下分发至命中控件 |
| 焦点窗口 | UICanvas 管理模态窗口层级，自动置顶 |

---

## 二、UIElement 基类

所有 UI 控件的基类，提供位置、大小、锚点、父子关系、点击回调等通用功能。

### 构造与属性

```cpp
namespace GameUI
{
    class UIElement
    {
    public:
        UIElement();                         // 自动分配唯一UID
        virtual ~UIElement();

        // 位置与大小
        void SetPosition(const MathType::vec2f& pos);
        void SetPosition(float x, float y);
        const MathType::vec2f& GetPosition() const;

        void SetSize(const MathType::vec2f& size);
        void SetSize(float w, float h);
        const MathType::vec2f& GetSize() const;

        // 锚点（默认(0,0)左上角）
        void SetAnchor(const MathType::vec2f& anchor);

        // 世界矩形（自动计算含父级偏移）
        MathType::RectF GetWorldRect() const;
        MathType::RectF GetSelfRect() const;
        virtual void CalcWorldRect();

        // 可见性
        bool IsActive() const;
        void SetActive(bool active);
        bool IsVisible();

        // 层级管理
        virtual bool AddChild(UIElement* child);
        virtual bool RemoveChild(UIElement* child);
        virtual void RemoveAllChildren();
        UIElement* GetChild(unsigned int uid);

        // 命中测试
        bool HitsTest(int x, int y) const;

        // 点击回调
        void SetOnClick(std::function<void()> callback);

        // 事件响应（子类重写）
        virtual void OnMouseDown(int x, int y);
        virtual void OnMouseUp(int x, int y);
        virtual void OnMouseMove(int x, int y);
        virtual void OnMouseWheel(int delta);
        virtual void OnKeyDown(WPARAM key);
        virtual void OnKeyUp(WPARAM key);

        // 获取信息
        UIElementType GetType() const;
        unsigned int GetUID() const;
        UIElement* GetParent() const;
        const std::vector<std::unique_ptr<UIElement>>& GetChildren() const;
    };
}
```

### UIElementType 枚举

```cpp
typedef enum class emUIElementType {
    UI_Base,       // 基类
    UI_Button,     // 按钮
    UI_CheckBox,   // 复选框
    UI_SliderBar,  // 滑动条
    UI_Window,     // 窗口
    UI_Canvas,     // 画布
    UI_ScollView   // 滚动视图
} UIElementType;
```

---

## 三、UICanvas - 顶级画布

`UICanvas` 是 UI 系统的顶层容器，每个 `Scene` 自动持有一个 `UICanvas` 实例。负责管理所有子控件和子窗口，分发输入事件。

### 生命周期

```cpp
// 构造时调用 Initialize 设置画布尺寸
bool Initialize(int screenWidth, int screenHeight);

// 窗口缩放时调用 Resize
void Resize(int screenWidth, int screenHeight);
```

### 子窗口管理

```cpp
// 添加普通子控件
virtual bool AddChild(UIElement* child) override;

// 移除子控件
virtual bool RemoveChild(UIElement* child) override;

// 移除所有子控件（含子窗口）
virtual void RemoveAllChildren() override;
```

### 与 Scene 的集成（自动处理，无需手动调用）

```cpp
// Scene 构造时自动创建 UICanvas
Scene::Scene() : m_isActive(false), mUICanvas(nullptr)
{
    InitializeUICanvas(App.GetWidth(), App.GetHeight());
}

// 获取当前场景的 UI 画布
UICanvas* canvas = scene->GetUICanvas();

// 渲染 UI（GameLoop 每帧自动调用）
scene->RenderUICanvas();  // 等价于 canvas->Update() + canvas->Render()
```

---

## 四、UIButton - 按钮

支持纹理模式和纯色模式的三状态按钮（Normal / Hover / Pressed）。

### 纹理模式

纹理贴图为一张 3 倍宽度横向拼接的图片，从左到右依次为 Normal、Hover、Pressed 状态。

```cpp
// 加载纹理
auto* texture = TextureMgr.Load(L"res\\ui\\button.bmp");

// 创建按钮（纹理模式）
auto* button = new GameUI::UIButton();
button->CreateWithTexture(x, y, width, height, texture);
button->SetOnClick([&]() {
    // 点击处理
});
canvas->AddChild(button);
```

### 纯色模式

```cpp
auto* button = new GameUI::UIButton();
button->CreateWithColor(
    x, y, width, height,
    ColorType::COLORF(0.2f, 0.6f, 1.0f, 1.0f),  // Normal 颜色
    ColorType::COLORF(0.3f, 0.7f, 1.0f, 1.0f),  // Hover 颜色
    ColorType::COLORF(0.1f, 0.4f, 0.8f, 1.0f),  // Pressed 颜色
    4.0f  // 圆角半径
);
```

### 设置文本

```cpp
// 参数：文本内容，字体名称，字号
button->SetText("开始游戏", L"Arial", 24.0f);
```

### API 摘要

| 方法 | 说明 |
|------|------|
| `CreateWithTexture(x, y, w, h, tex)` | 纹理模式创建 |
| `CreateWithColor(x, y, w, h, normal, hover, pressed, radius)` | 纯色模式创建 |
| `SetText(text, fontName, fontSize)` | 设置按钮文字 |
| `SetOnClick(callback)` | 设置点击回调（继承自 UIElement） |

---

## 五、UICheckBox - 复选框

双状态勾选框，纹理为 2 倍宽度横向拼接（左=未选中，右=已选中）。

```cpp
// 加载复选框纹理（2倍宽度横向拼接）
auto* texture = TextureMgr.Load(L"res\\ui\\checkbox.bmp");

auto* checkBox = new GameUI::UICheckBox();
checkBox->Create(x, y, width, height, texture, false);  // false=初始未选中
checkBox->SetOnCheckedChanged([&](bool checked) {
    if (checked) {
        // 已选中
    } else {
        // 未选中
    }
});
canvas->AddChild(checkBox);
```

### API 摘要

| 方法 | 说明 |
|------|------|
| `Create(x, y, w, h, texture, checked)` | 创建复选框，checked 为初始状态 |
| `IsChecked()` | 获取当前选中状态 |
| `SetChecked(bool)` | 设置选中状态 |
| `SetOnCheckedChanged(callback)` | 状态改变回调，参数为新的选中状态 |

---

## 六、UISliderBar - 滑动条

支持纹理模式和纯色模式，值域 0.0~1.0，由背景条、填充条和滑块三部分组成。

### 纹理模式

```cpp
auto* backTex = TextureMgr.Load(L"res\\ui\\slider_bg.bmp");
auto* coverTex = TextureMgr.Load(L"res\\ui\\slider_fill.bmp");
auto* handleTex = TextureMgr.Load(L"res\\ui\\slider_handle.bmp");

auto* slider = new GameUI::UISliderBar();
slider->CreateWithTexture(x, y, width, height, backTex, coverTex, handleTex, 0.5f);
slider->SetOnValueChanged([&](float value) {
    // value 范围 0.0~1.0
});
canvas->AddChild(slider);
```

### 纯色模式

```cpp
auto* slider = new GameUI::UISliderBar();
slider->CreateWithColor(
    x, y, width, height,
    16.0f,  // handleWidth 滑块宽度
    ColorType::COLORF(0.3f, 0.3f, 0.3f, 1.0f),  // 背景色
    ColorType::COLORF(0.2f, 0.6f, 1.0f, 1.0f),  // 填充色
    ColorType::COLORF(1.0f, 1.0f, 1.0f, 1.0f),  // 滑块色
    0.0f   // 初始值
);
```

### API 摘要

| 方法 | 说明 |
|------|------|
| `CreateWithTexture(x, y, w, h, bg, cover, handle, value)` | 纹理模式创建 |
| `CreateWithColor(x, y, w, h, handleW, backColor, fillColor, handleColor, value)` | 纯色模式创建 |
| `GetValue()` | 获取当前值 (0.0~1.0) |
| `SetValue(float)` | 设置当前值 |
| `SetOnValueChanged(callback)` | 值改变回调，参数为当前浮点值 |

---

## 七、UIWindow - 弹窗容器

可拖动、可设置模态的弹窗窗口，支持标题栏、背景纹理/纯色。

```cpp
auto* window = new GameUI::UIWindow();

// 创建窗口：位置、大小、标题栏高度、是否可拖动、背景纹理
window->Create(100.0f, 80.0f, 400.0f, 300.0f, 30.0f, true, nullptr);

// 设置为模态窗口（阻止事件穿透到下层）
window->SetModal(true);

// 设置背景色
window->SetBackColor(ColorType::COLORF(0.1f, 0.1f, 0.15f, 0.9f));

// 设置标题栏颜色
window->SetTitleBarColor(ColorType::COLORF(0.2f, 0.3f, 0.5f, 1.0f));

// 设置边框颜色
window->SetBorderColor(ColorType::COLORF(0.4f, 0.5f, 0.8f, 1.0f));

// 向窗口中添加子控件（如按钮）
auto* closeBtn = new GameUI::UIButton();
closeBtn->CreateWithColor(window->GetSize().x - 60.0f, 2.0f, 50.0f, 24.0f,
    ColorType::COLORF(0.6f, 0.2f, 0.2f, 1.0f),
    ColorType::COLORF(0.8f, 0.3f, 0.3f, 1.0f),
    ColorType::COLORF(0.4f, 0.1f, 0.1f, 1.0f));
closeBtn->SetText("X", L"Arial", 16.0f);
closeBtn->SetOnClick([window]() {
    window->SetActive(false);
});
window->AddChild(closeBtn);

// 添加到画布
canvas->AddChild(window);
```

### API 摘要

| 方法 | 说明 |
|------|------|
| `Create(x, y, w, h, titleH, canMove, bgTex)` | 创建窗口 |
| `SetCanMove(bool)` | 设置是否可拖动 |
| `SetModal(bool)` | 设置模态（点击事件不穿透） |
| `IsModal()` | 是否模态 |
| `SetBackColor(COLORF)` | 设置背景色 |
| `SetTitleBarColor(COLORF)` | 设置标题栏颜色 |
| `SetBorderColor(COLORF)` | 设置边框颜色 |
| `SetTitleBarHeight(float)` | 设置标题栏高度 |
| `AddChild(UIElement*)` | 添加子控件（相对于窗口坐标） |

---

## 八、UIScrollView - 滚动视图

支持鼠标滚轮滚动的内容容器，使用 D2D `PushAxisAlignedClip` 实现裁剪渲染。

```cpp
auto* scrollView = new GameUI::UIScrollView();

// 创建：位置、视口大小、内容大小、滚动速度
scrollView->Create(50.0f, 50.0f, 300.0f, 200.0f, 300.0f, 500.0f, 20.0f);

// 添加子控件到滚动视图中
for (int i = 0; i < 20; i++) {
    auto* item = new GameUI::UIButton();
    item->CreateWithColor(0, i * 40.0f, 280.0f, 30.0f,
        ColorType::COLORF(0.2f, 0.2f, 0.3f, 1.0f),
        ColorType::COLORF(0.3f, 0.3f, 0.5f, 1.0f),
        ColorType::COLORF(0.1f, 0.1f, 0.2f, 1.0f));
    scrollView->AddChild(item);
}

canvas->AddChild(scrollView);
```

### API 摘要

| 方法 | 说明 |
|------|------|
| `Create(x, y, viewW, viewH, contentW, contentH, speed)` | 创建滚动视图 |
| `GetScrollOffset()` | 获取当前 Y 方向滚动偏移 |
| `SetScrollOffset(float)` | 设置 Y 方向滚动偏移 |

---

## 九、Scene 集成机制

### 自动化事件转发

`Scene` 基类已内置 UICanvas 支持，鼠标/键盘事件自动转发：

| Scene 事件方法 | 转发目标 |
|---------------|---------|
| `OnMouseDown(MouseButton, int, int)` | `mUICanvas->OnMouseDown(x, y)` (仅左键) |
| `OnMouseUp(MouseButton, int, int)` | `mUICanvas->OnMouseUp(x, y)` (仅左键) |
| `OnMouseMove(int, int)` | `mUICanvas->OnMouseMove(x, y)` |
| `OnMouseWheel(int)` | `mUICanvas->OnMouseWheel(delta)` |
| `OnKeyDown(WPARAM)` | `mUICanvas->OnKeyDown(key)` |
| `OnKeyUp(WPARAM)` | `mUICanvas->OnKeyUp(key)` |

### 渲染流程

在 `GameLoop.cpp` 中每帧自动调用：

```cpp
// 每帧渲染循环中
SceneMgr.Render(m_TimeDelta);                       // 游戏场景渲染
SceneMgr.GetCurrentScene()->RenderUICanvas();       // UI层渲染（Update + Render）
```

### 窗口缩放响应

`Application::_OnSizeChange()` 中自动通知：

```cpp
auto* curScene = SceneMgr.GetCurrentScene();
if (curScene && curScene->GetUICanvas()) {
    curScene->GetUICanvas()->Resize(newWidth, newHeight);
}
```

### 子类场景中使用 UI

```cpp
void S0::OnEnter()
{
    // 获取画布
    auto* canvas = GetUICanvas();
    if (!canvas) return;

    // 创建按钮
    auto* startBtn = new GameUI::UIButton();
    startBtn->CreateWithColor(300, 200, 200, 50,
        ColorType::COLORF(0.2f, 0.6f, 0.2f, 1.0f),
        ColorType::COLORF(0.3f, 0.8f, 0.3f, 1.0f),
        ColorType::COLORF(0.1f, 0.4f, 0.1f, 1.0f));
    startBtn->SetText("开始游戏", L"SimSun", 24.0f);
    startBtn->SetOnClick([&]() {
        SceneMgr.SwitchScene("S1");
    });
    canvas->AddChild(startBtn);
}
```

---

## 十、Lua 脚本绑定

UI 系统提供了完整的 Lua C API 导出层（位于 `LuaExport/` 目录），可在 Lua 脚本中直接操作 UI。

### UICanvas Lua API

```lua
local canvas = UICanvas:new()
canvas:Initialize(800, 600)
canvas:AddChild(button)
canvas:RemoveChild(button)
canvas:SetActive(true)
```

### UIButton Lua API

```lua
local btn = UIButton:new()
-- 纹理模式
btn:CreateWithTexture(x, y, w, h, texture)
-- 纯色模式
btn:CreateWithColor(x, y, w, h, normalColor, hoverColor, pressedColor, radius)
btn:SetText("按钮文字", "Arial", 24)
btn:SetOnClick(function() print("clicked!") end)
```

### UICheckBox Lua API

```lua
local cb = UICheckBox:new()
cb:Create(x, y, w, h, texture, false)
cb:SetOnCheckedChanged(function(checked) print(checked) end)
local checked = cb:IsChecked()
cb:SetChecked(true)
```

### UISliderBar Lua API

```lua
local slider = UISliderBar:new()
-- 纹理模式
slider:CreateWithTexture(x, y, w, h, bgTex, coverTex, handleTex, 0.5)
-- 纯色模式
slider:CreateWithColor(x, y, w, h, handleW, backColor, fillColor, handleColor, 0.5)
slider:SetOnValueChanged(function(value) print(value) end)
local val = slider:GetValue()
slider:SetValue(0.75)
```

### UIWindow Lua API

```lua
local win = UIWindow:new()
win:Create(x, y, w, h, titleH, true, nil)
win:SetModal(true)
win:SetCanMove(true)
win:AddChild(childElem)
win:SetBackColor(color)
win:SetTitleBarColor(color)
win:SetBorderColor(color)
```

### UIScrollView Lua API

```lua
local sv = UIScrollView:new()
sv:Create(x, y, viewW, viewH, contentW, contentH, 20)
local offset = sv:GetScrollOffset()
sv:SetScrollOffset(50.0)
```

---

## 十一、完整使用示例

```cpp
#include "GameCore/Scene.h"
#include "GameUI/UICanvas.h"
#include "GameUI/UIButton.h"
#include "GameUI/UICheckBox.h"
#include "GameUI/UISliderBar.h"
#include "GameUI/UIWindow.h"
#include "GameUI/UIScrollView.h"
#include "D2DCore/TextureManager.h"

class SettingsScene : public GameCore::Scene
{
public:
    void OnEnter() override
    {
        auto* canvas = GetUICanvas();
        if (!canvas) return;

        // 1. 创建标题文本按钮
        mTitleBtn = new GameUI::UIButton();
        mTitleBtn->CreateWithColor(250, 20, 300, 40,
            ColorType::COLORF(0.1f, 0.1f, 0.2f, 1.0f),
            ColorType::COLORF(0.1f, 0.1f, 0.2f, 1.0f),
            ColorType::COLORF(0.1f, 0.1f, 0.2f, 1.0f));
        mTitleBtn->SetText("游戏设置", L"SimHei", 28.0f);
        canvas->AddChild(mTitleBtn);

        // 2. 音乐音量滑动条
        mMusicSlider = new GameUI::UISliderBar();
        mMusicSlider->CreateWithColor(250, 100, 300, 20, 16.0f,
            ColorType::COLORF(0.2f, 0.2f, 0.2f, 1.0f),
            ColorType::COLORF(0.2f, 0.6f, 0.2f, 1.0f),
            ColorType::COLORF(1.0f, 1.0f, 1.0f, 1.0f),
            0.8f);
        mMusicSlider->SetOnValueChanged([&](float val) {
            AudioLib.SetVolume("BGM", val);
        });
        canvas->AddChild(mMusicSlider);

        // 3. 全屏复选框
        mFullScreenCB = new GameUI::UICheckBox();
        // 需要先加载纹理...
        // mFullScreenCB->Create(250, 180, 24, 24, cbTexture, false);
        // mFullScreenCB->SetOnCheckedChanged([&](bool checked) {
        //     App.SetFullScreen(checked);
        // });

        // 4. 返回按钮
        mBackBtn = new GameUI::UIButton();
        mBackBtn->CreateWithColor(300, 500, 200, 45,
            ColorType::COLORF(0.6f, 0.2f, 0.2f, 1.0f),
            ColorType::COLORF(0.8f, 0.3f, 0.3f, 1.0f),
            ColorType::COLORF(0.4f, 0.1f, 0.1f, 1.0f));
        mBackBtn->SetText("返回主菜单", L"SimSun", 20.0f);
        mBackBtn->SetOnClick([&]() {
            SceneMgr.SwitchScene("StartScene");
        });
        canvas->AddChild(mBackBtn);

        // 5. 弹窗示例（帮助窗口）
        mHelpWindow = new GameUI::UIWindow();
        mHelpWindow->Create(150, 100, 500, 350, 30, true, nullptr);
        mHelpWindow->SetModal(true);
        mHelpWindow->SetBackColor(ColorType::COLORF(0.1f, 0.1f, 0.15f, 0.95f));
        mHelpWindow->SetTitleBarColor(ColorType::COLORF(0.2f, 0.4f, 0.6f, 1.0f));
        mHelpWindow->SetActive(false);  // 默认隐藏
        canvas->AddChild(mHelpWindow);
    }

private:
    GameUI::UIButton* mTitleBtn = nullptr;
    GameUI::UISliderBar* mMusicSlider = nullptr;
    GameUI::UICheckBox* mFullScreenCB = nullptr;
    GameUI::UIButton* mBackBtn = nullptr;
    GameUI::UIWindow* mHelpWindow = nullptr;
};
```

---

## 十二、注意事项

1. **内存管理**：`UIElement::AddChild` 接收裸指针后转由 `std::unique_ptr` 托管，不要手动 delete 已添加的子控件。

2. **渲染顺序**：UI 在游戏场景渲染之后执行（`GameLoop.cpp` 中 `RenderUICanvas()` 在 `SceneMgr.Render()` 之后），确保 UI 显示在最上层。

3. **事件优先级**：模态窗口（`SetModal(true)`）会拦截鼠标事件，阻止穿透到下层控件。

4. **纹理加载**：使用 `TextureManager::Load()` 加载纹理，纹理对象由 TextureManager 管理生命周期。

5. **坐标系统**：UI 坐标系统原点为屏幕左上角，X 轴向右，Y 轴向下。子控件的 `SetPosition` 相对于父控件位置 + 锚点偏移。

6. **场景切换**：切换到新场景时，旧场景的 UICanvas 会被自动销毁，子控件也会被级联释放。

7. **Lua 绑定**：所有 Lua 导出函数通过 `LuaExport::RegGameFramework2Lua(L)` 统一注册，需要在初始化 Lua 状态机后调用。
