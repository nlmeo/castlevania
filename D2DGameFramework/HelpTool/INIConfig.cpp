#define  _CRT_SECURE_NO_WARNINGS
#include "INIConfig.h"
#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <io.h>
#include <fstream>


namespace HelpTool
{

	bool INIConfig::ParseINI(const char* inifile)
	{
		if (!inifile) return false;
		std::ifstream input(inifile, std::ios::binary);
		if (!input) return false;
		INIData parsed;
		std::string line, section;
		auto trim = [](const std::string& value) {
			const auto first = value.find_first_not_of(" \t\r\n");
			if (first == std::string::npos) return std::string();
			return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
		};
		bool firstLine = true;
		while (std::getline(input, line))
		{
			if (firstLine && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
			firstLine = false;
			line = trim(line);
			if (line.empty() || line[0] == ';' || line[0] == '#') continue;
			if (line[0] == '[')
			{
				if (line.back() != ']') return false;
				section = trim(line.substr(1, line.size() - 2));
				if (section.empty()) return false;
				parsed[section];
				continue;
			}
			const auto equal = line.find('=');
			if (section.empty() || equal == std::string::npos) return false;
			const std::string key = trim(line.substr(0, equal));
			if (key.empty()) return false;
			parsed[section][key] = trim(line.substr(equal + 1));
		}
		if (input.bad() || parsed.empty()) return false;
		mData.swap(parsed);
		return true;
	}

	bool INIConfig::Save2File(const char* path)
	{
		if (path == nullptr) { return false; }
		FILE* pfile = nullptr;
		fopen_s(&pfile, path, "wb+");
		if (pfile == nullptr)
		{
			return false;
		}
		for (auto& s : mData)
		{
			fwrite("[", 1, 1, pfile);
			fwrite(s.first.c_str(), s.first.length(), 1, pfile);
			fwrite("]\r\n", 1, 3, pfile);

			for (auto& kv : s.second)
			{
				fwrite(kv.first.c_str(), kv.first.length(), 1, pfile);
				fwrite("=", 1, 1, pfile);
				fwrite(kv.second.CStr(), strlen(kv.second.CStr()), 1, pfile);
				fwrite("\r\n", 1, 2, pfile);
			}
		}
		const bool written = !ferror(pfile) && fflush(pfile) == 0;
		const bool closed = fclose(pfile) == 0;
		pfile = nullptr;
		return written && closed;
	}

	void INIConfig::PrintData()
	{
		for (auto& s : mData)
		{
			printf("[%s]\n", s.first.c_str());
			for (auto& kv : s.second)
			{
				printf("%s = %s\n", kv.first.c_str(), kv.second.CStr());
			}
		}
	}


	ConfigNode& INIConfig::Value(std::string sectionname, std::string keyname)
	{
		return mData[sectionname][keyname];
	}


	INIConfig::Section& INIConfig::operator[](std::string sectionname)
	{
		return mData[sectionname];
	}


	bool INIConfig::HasSection(std::string name)
	{
		return mData.count(name) > 0;
	}

	bool INIConfig::HasKey(std::string sname, std::string kname)
	{
		auto it = mData.find(sname);
		if (it != mData.end())
		{
			return it->second.count(kname) > 0;
		}
		return false;
	}

	bool INIConfig::DeleteSection(std::string sname)
	{
		auto it = mData.find(sname);
		if (it != mData.end())
		{
			mData.erase(it);
			return true;
		}
		return false;
	}

	bool INIConfig::DeleteKey(std::string sname, std::string kname)
	{
		auto it = mData.find(sname);
		if (it != mData.end())
		{
			auto itk = it->second.find(kname);
			if (itk != it->second.end())
			{
				it->second.erase(itk);
				return true;
			}
		}
		return false;
	}

	ConfigNode::ConfigNode()
	{
		mValue = "";
	}

	ConfigNode::ConfigNode(std::string v)
	{
		mValue = v;
	}

	ConfigNode::ConfigNode(const char* v)
	{
		mValue = v;
	}

	ConfigNode::ConfigNode(int v)
	{
		char buf[50] = {};
		sprintf(buf, "%d", v);
		mValue = buf;
	}

	ConfigNode::ConfigNode(float v)
	{
		char buf[50] = {};
		sprintf(buf, "%f", v);
		mValue = buf;
	}

	ConfigNode::ConfigNode(bool v)
	{
		mValue = v ? "true" : "false";
	}

	ConfigNode::operator std::string()
	{
		return mValue;
	}

	ConfigNode::operator int()
	{
		int a = atoi(mValue.c_str());
		return a;
	}

	ConfigNode::operator float()
	{
		float a = (float)atof(mValue.c_str());
		return a;
	}

	ConfigNode::operator bool()
	{
		if (mValue == "true")
		{
			return true;
		}
		return false;
	}


	const char* ConfigNode::CStr()
	{
		return mValue.c_str();
	}

}
