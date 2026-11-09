#pragma once
#ifndef __INI_CONFIG_FILE_H__
#define __INI_CONFIG_FILE_H__

#include <string>
#include <map>

namespace HelpTool
{
	/**
	 * @brief 配置节点类
	 *
	 * 支持与int、float、bool、string之间的自由转换，用于存储INI配置项的值。
	 */
	class ConfigNode
	{
	public:
		/** @brief 默认构造函数 */
		ConfigNode();

		/** @brief 使用字符串初始化 */
		ConfigNode(std::string v);

		/** @brief 使用C字符串初始化 */
		ConfigNode(const char* v);

		/** @brief 使用整数初始化 */
		ConfigNode(int v);

		/** @brief 使用浮点数初始化 */
		ConfigNode(float v);

		/** @brief 使用布尔值初始化 */
		ConfigNode(bool v);

		/** @brief 转换为字符串 */
		operator std::string();

		/** @brief 转换为整数 */
		operator int();

		/** @brief 转换为浮点数 */
		operator float();

		/** @brief 转换为布尔值 */
		operator bool();

		/** @brief 获取C风格字符串 */
		const char* CStr();

	private:
		std::string mValue;  ///< 存储的值
	};

	/**
	 * @brief INI配置文件解析类
	 *
	 * 支持解析标准INI格式的配置文件，提供读写操作。
	 */
	class INIConfig
	{
	public:
		typedef std::map<std::string, ConfigNode> Section;   ///< 配置节类型
		typedef std::map<std::string, Section> INIData;      ///< INI数据类型

		/**
		 * @brief 解析INI配置文件
		 *
		 * @param inifile INI文件路径
		 * @return 解析成功返回true，失败返回false
		 */
		bool ParseINI(const char* inifile);

		/**
		 * @brief 将配置保存到文件
		 *
		 * @param path 目标文件路径
		 * @return 保存成功返回true，失败返回false
		 */
		bool Save2File(const char* path);

		/** @brief 打印所有配置数据 */
		void PrintData();

		/**
		 * @brief 获取指定节和键的值
		 *
		 * @param sectionname 节名称
		 * @param keyname 键名称
		 * @return 配置节点引用
		 */
		ConfigNode& Value(std::string sectionname, std::string keyname);

		/**
		 * @brief 获取指定节
		 *
		 * @param sectionname 节名称
		 * @return 节引用
		 */
		Section& operator [] (std::string sectionname);

		/**
		 * @brief 检查是否存在指定节
		 *
		 * @param name 节名称
		 * @return 存在返回true，否则返回false
		 */
		bool HasSection(std::string name);

		/**
		 * @brief 检查指定节中是否存在指定键
		 *
		 * @param sname 节名称
		 * @param kname 键名称
		 * @return 存在返回true，否则返回false
		 */
		bool HasKey(std::string sname, std::string kname);

		/**
		 * @brief 删除指定节
		 *
		 * @param name 节名称
		 * @return 删除成功返回true，失败返回false
		 */
		bool DeleteSection(std::string name);

		/**
		 * @brief 删除指定节中的键
		 *
		 * @param sname 节名称
		 * @param kname 键名称
		 * @return 删除成功返回true，失败返回false
		 */
		bool DeleteKey(std::string sname, std::string kname);

	protected:
	private:
		INIData mData;  ///< 存储完整INI文件内容
	};
}

#endif