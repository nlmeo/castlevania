#pragma once
#ifndef __HELP_TOOL_RANDOM_MAKER_H__
#define __HELP_TOOL_RANDOM_MAKER_H__

#include <string>
#include <random>
#include <chrono>

namespace HelpTool
{
	inline std::string RandomNum(size_t bitCount) {
		if (bitCount == 0) {
			return {}; // 空字符串
		}

		// 线程安全的随机数引擎（使用静态确保只初始化一次）
		static thread_local std::mt19937 gen([]() {
			// 混合使用 random_device 和时间戳，增强随机性
			std::random_device rd;
			std::seed_seq seed{ rd(), static_cast<unsigned>(
				std::chrono::steady_clock::now().time_since_epoch().count()) };
			return std::mt19937(seed);
			}());

		std::string result;
		result.reserve(bitCount);

		// 生成最高位（1-9）
		std::uniform_int_distribution<int> firstDigitDist(1, 9);
		result.push_back(static_cast<char>('0' + firstDigitDist(gen)));

		// 生成其余位（0-9）
		std::uniform_int_distribution<int> otherDigitDist(0, 9);
		for (size_t i = 1; i < bitCount; ++i) {
			result.push_back(static_cast<char>('0' + otherDigitDist(gen)));
		}

		return result;
	}

	// 生成指定位数的随机大写字母或者小写字母
	inline std::string RandomLetters(size_t count, bool upperCase = true) {
		if (count == 0) {
			return {};
		}

		static thread_local std::mt19937 gen([]() {
			std::random_device rd;
			std::seed_seq seed{ rd(), static_cast<unsigned>(
				std::chrono::steady_clock::now().time_since_epoch().count()) };
			return std::mt19937(seed);
			}());

		std::uniform_int_distribution<int> dist(0, 25); // 0~25 对应 A~Z 或 a~z
		std::string result;
		result.reserve(count);

		char base = upperCase ? 'A' : 'a';
		for (size_t i = 0; i < count; ++i) {
			result.push_back(static_cast<char>(base + dist(gen)));
		}

		return result;
	}

	//生成指定位数的随机大小写混合字母
	inline std::string RandomLettersMix(size_t count) {
		if (count == 0) {
			return {};
		}

		static thread_local std::mt19937 gen([]() {
			std::random_device rd;
			std::seed_seq seed{ rd(), static_cast<unsigned>(
				std::chrono::steady_clock::now().time_since_epoch().count()) };
			return std::mt19937(seed);
			}());

		std::uniform_int_distribution<int> dist(0, 51);
		const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::string result;
		result.reserve(count);

		for (size_t i = 0; i < count; ++i) {
			result.push_back(charset[dist(gen)]);
		}

		return result;
	}

	inline float RandomFloat(float minf, float maxf) {
		// 确保 minf <= maxf，如果输入顺序颠倒则交换
		if (minf > maxf) {
			std::swap(minf, maxf);
		}

		// 使用静态变量确保随机数生成器只初始化一次
		static std::random_device rd;
		static std::mt19937 gen(rd());
		// 使用 uniform_real_distribution 生成 [minf, maxf] 内的浮点数
		std::uniform_real_distribution<float> dis(minf, maxf);

		return dis(gen);
	}

	inline int RandomInt(int minf, int maxf) {
		// 确保 minf <= maxf，如果输入顺序颠倒则交换
		if (minf > maxf) {
			std::swap(minf, maxf);
		}

		// 使用静态变量确保随机数生成器只初始化一次
		static std::random_device rd;
		static std::mt19937 gen(rd());
		// 使用 uniform_int_distribution 生成 [minf, maxf] 内的整数
		std::uniform_int_distribution<int> dis(minf, maxf);

		return dis(gen);
	}
}
#endif