#pragma once
#ifndef __MY_SINGLETON_TEMPLATE_H__
#define __MY_SINGLETON_TEMPLATE_H__

/**
 * @brief CRTP单例模板类
 *
 * 使用Curiously Recurring Template Pattern实现线程安全的单例模式
 *
 * @tparam T 派生类类型
 */
template<typename T>
class Singleton
{
protected:
	/** @brief 保护构造函数，防止外部实例化 */
	Singleton() = default;

public:
	/**
	 * @brief 获取单例实例
	 *
	 * 使用局部静态变量实现线程安全的懒汉模式
	 *
	 * @return 单例实例引用
	 */
	static T& Instance()
	{
		static T instance;
		return instance;
	}
};

/**
 * @brief 宏：开放私有构造函数给Singleton模板
 *
 * 在派生类中使用此宏，允许Singleton模板访问私有构造函数
 *
 * @param t 派生类类型
 */
#define PRIVATE_OPEN(t) friend class Singleton<t>;

/**
 * @brief 宏：私有化构造函数和拷贝操作
 *
 * 在派生类中使用此宏，禁用拷贝构造和赋值运算符，同时开放构造函数给Singleton
 *
 * @param t 派生类类型
 */
#define PRIVATE_CONSTRUCTOR_AND_COPY(t) \
private:\
	t(const t&) = delete;\
	t& operator = (const t&) = delete;\
	t() = default;\
	friend class Singleton<t>;

/**
 * @brief 宏：私有化拷贝操作（保留构造函数）
 *
 * 在派生类中使用此宏，禁用拷贝构造和赋值运算符，但保留构造函数为公有或保护
 *
 * @param t 派生类类型
 */
#define PRIVATE_COPY(t) \
private:\
	t(const t&) = delete;\
	t& operator = (const t&) = delete;\
	friend class Singleton<t>;

#endif