/*
	very simple ini file manager.
	by "github.com/giffi-dev/" (me)
*/

#pragma once
#include <fstream>
#include <string>
#include <map>
#include <variant>

class FileIni
{
public:
	FileIni() {}
	FileIni(const std::string& path)
	{
		Parse(path);
	}
	
	~FileIni() {}

	template<typename T>
	T GetValue(const std::string& section, const std::string& value)
	{
		return std::get<T>(m_Sections[section][value].m_Value);
	}

	template<typename T>
	void SetValue(const std::string& section, const std::string& value_name, const T& value)
	{
		m_Sections[section][value_name].Set(value);
	}

	bool HasSection(const std::string& section)
	{
		return m_Sections.count(section);
	}

	bool HasValue(const std::string& section, const std::string& value_name)
	{
		return m_Sections.count(section) && m_Sections[section].count(value_name);
	}

	void SaveFile(const std::string& path)
	{
		std::ofstream file(path);

		auto i = m_Sections.begin();
		for (const auto[section_name, value_map] : m_Sections)
		{
			auto j = value_map.begin();
			file << "[" << section_name << "]\n";
			for (const auto[val_name, value] : value_map)
			{
				switch(value.m_Type)
				{
					case VarType::INT:
						file << val_name << "=" << std::to_string( std::get<int>(value.m_Value) ) << "\n";
						break;
					case VarType::REAL:
						file << val_name << "=" << std::to_string( std::get<float>(value.m_Value) ) << "\n";
						break;
					case VarType::STRING:
						file << val_name << "=" << std::get<std::string>(value.m_Value) << "\n";
						break;
					default: 
						static_assert("INVALID TYPE");
						break;
				}
				j++;
			}
			i++;
		}

		file.close();
	}
	
private:
	bool IsStrInt(const std::string& str)
	{
		for (const char& c : str) {
			if (std::isdigit(c) == 0) return false;
		}
		return true;
	}

	void Parse(const std::string& path)
	{
		std::fstream file(path);

		std::string current_section = "";
		std::string current_value   = "";
		std::string current_parsing = "";

		char c;
		while (file.get(c))
		{
			switch (c)
			{
				case '\n':
					if (!current_value.empty() && !current_section.empty())
					{
						if (IsStrInt(current_parsing))
						{
							m_Sections[current_section][current_value].Set(atoi(current_parsing.c_str()));
						}
						else if (atof(current_parsing.c_str()))
						{
							m_Sections[current_section][current_value].Set((float)atof(current_parsing.c_str()));
						}
						else
						{
							m_Sections[current_section][current_value].Set(current_parsing);
						}
						
						current_parsing.clear();
						current_value.clear();
					}
					break;
				case '=':
					current_value = current_parsing;
					current_parsing.clear();
					break;
				case '[':
					current_section.clear();
					current_value.clear();
					current_parsing.clear();
					break;
				case ']':
					current_section = current_parsing;
					current_value.clear();
					current_parsing.clear();
					break;
				default:
					current_parsing += c;
					break;
			}
		} 

		file.close();
	}

private:
	enum class VarType
	{
		INT,
		REAL,
		STRING,
	};

	struct Variant
	{
	public:
		void Set(int value)
		{
			m_Type = VarType::INT;
			m_Value = value;
		}

		void Set(float value)
		{
			m_Type = VarType::REAL;
			m_Value = value;
		}

		void Set(const std::string& value)
		{
			m_Type = VarType::STRING;
			m_Value = value;
		}

	private:
		VarType m_Type;
		std::variant<int, float, std::string> m_Value;
		friend FileIni;
	};

	std::map<std::string, std::map<std::string, Variant> > m_Sections;
};
 
