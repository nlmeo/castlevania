# D2DGameFramework

Windows 原生 C++ 横版动作游戏，采用对象/组件模型，包含动画、碰撞、状态机、UI、背包、魂技能和存档。

## 构建与运行

- 使用 Visual Studio 2026，安装 **使用 C++ 的桌面开发**、MSVC **v145** 和 Windows 10/11 SDK。
- 打开 `D2DGameFramework.sln`，优先选择 `Debug | x64`。
- 四种配置统一使用 C++20；第三方头文件、导入库和 DLL 位于 `ThirdParty`。
- 解决方案中的 32 位平台名为 `x86`，映射到 C++ 项目的 `Win32`；命令行构建解决方案时使用 `/p:Platform=x86`。
- 从 Visual Studio 调试时，工作目录设为项目目录 `D2DGameFramework`。项目已设置该默认值。
- 从终端启动时，也必须使用项目目录作为工作目录，资源路径 `resource/` 和存档路径 `save/` 都以它为基准。

PowerShell 示例（从解决方案根目录执行）：

```powershell
$vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
$installation = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
$msbuild = Join-Path $installation 'MSBuild/Current/Bin/MSBuild.exe'
& $msbuild D2DGameFramework.sln /m /p:Configuration=Debug /p:Platform=x64
Push-Location D2DGameFramework
& ../x64/Debug/D2DGameFramework.exe
Pop-Location
```

构建后会自动复制 FFmpeg、FreeImage、BASS 的 DLL 到输出目录；资源文件不会自动复制。需要打包时，将 `resource/` 一并放到运行工作目录。

x86 额外运行依赖及许可记录见 `ThirdParty/FFmpeg/DEPENDENCIES.md`。MSVC 导入库重建说明见 `ThirdParty/FFmpeg/lib/msvc/README.md`。

## 项目归档

源码归档保留源码、游戏资源、第三方构建及运行依赖、现有回归测试源码和说明文档。使用 Git 已跟踪文件生成归档，排除构建输出、临时诊断文件、Visual Studio 本地缓存、个人运行存档和 AI 提示或配置文件。解压后按上面的步骤重新构建；本地个人存档不受归档操作影响。

2026-10-30：本轮修改已通过 `Debug | x64` 编译、链接。场景切换回归可使用 `tests/RunFadeTransitionRegression.ps1` 重新构建并运行。

2026-10-31：补充菜单文字、存档日期时间预览、直接保存及保存后返回标题；修复尸块方向与返回标题时的玩家清理。独立输出的 `Debug | x64` 构建及对应回归检查通过。本归档清除了生成文件，解压后需重新构建。

2026-10-28：清理测试引用后，已重新构建 `Release | x64` 并通过编译、链接。

## 统一开发约定

1. 对象由 `CreateGameObject` 创建并交给管理器；游戏逻辑通过 `Destroy()` 请求销毁，不直接 `delete` 管理器持有的对象。
2. `Destroy()` 幂等。销毁后不再参与逻辑、碰撞与绘制；销毁前尚未初始化的对象不执行初始化回调。
3. 更新期间的场景切换在帧逻辑结束后执行。同一帧采用第一个有效请求；切换后才能执行的工作使用 `SceneManager::SwitchTo` 的完成回调。
4. 场景通过 `IsWorldPaused()` 表达暂停。主循环冻结世界更新、碰撞与游戏特效，场景菜单及 UI 继续更新和绘制。
5. 帧更新与摄像机时间参数统一使用秒。角色代码中仍存在按帧计量的旧速度和动画参数，本次保留其玩法数值。
6. 普通批量清理同时处理活动与待加入对象。选档进入游戏是明确例外：保留本次创建、尚未合并的新玩家及技能对象；保留对象仍由管理器持有。
7. 读档先验证版本、场景和数值，失败留在选档页；保存先写同目录临时文件，再替换原文件。

具体实施和历史验证状态见 [优化清单](OPTIMIZATION_CHECKLIST.md)。UI 使用说明位于 `D2DGameFramework/GameUI/UI系统使用文档.md`。

体力消耗、自动恢复、默认数值及菜单浮动数字的处理规则见 [体力功能说明](STAMINA.md)。

子弹与伤害数字的切场景清理、下落高度规则、Soma 全部状态的校验结论见 [状态检查报告](SOMA_STATE_AUDIT.md)。
