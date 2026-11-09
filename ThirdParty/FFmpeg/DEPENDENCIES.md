# x86 运行依赖

本项目随附的 x86 `avcodec-62.dll` 依赖 `openh264-2.6.0-win32.dll`，原目录缺少该文件，导致程序启动返回 `0xC0000135`。本次在用户设备上从 Cisco 官方站点单独下载并解压匹配版本。

- 官方发布说明：<https://github.com/cisco/openh264/releases/tag/v2.6.0>
- 下载地址：<https://ciscobinary.openh264.org/openh264-2.6.0-win32.dll.bz2>
- 本地路径：`bin/x86/openh264-2.6.0-win32.dll`
- Authenticode 校验：`Valid`；签名者 `Cisco WebEx LLC`。
- 解压后 DLL 的 SHA-256：`B0098DB6ACBD290A1FE13997D61D461E7327E39B42BF868DB41FAF498B7621A2`。
- 许可全文：本目录的 `OPENH264_BINARY_LICENSE.txt`，来源 <https://www.openh264.org/BINARY_LICENSE.txt>。

OpenH264 Video Codec provided by Cisco Systems, Inc.

构建后的复制步骤会将该运行依赖放入本地 x86 输出目录。对外发布时请查阅随附许可，其中包含 Cisco 二进制的独立下载与用户控制条件。

`lib/msvc/x86/` 的导入库可使用配套 `.def` 文件和 Visual Studio `lib.exe` 重建，说明见 `lib/msvc/README.md`；它们只描述既有 DLL 的导出接口，没有替换 FFmpeg DLL，也没有关闭链接器安全检查。
