/********************************************************************
	created:		2005/08/27
	created:		27:8:2005   9:49
	filename: 		d:\work\newcvs\chub\impl\src\common\sdconfiguration.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdconfiguration
	file ext:		h
	author:			lifeng
	description:	a simple configuartion (only read now)
					key=value
*********************************************************************/

#ifndef SANDAI_C_SDCONFIGURATION_H_200508270949
#define SANDAI_C_SDCONFIGURATION_H_200508270949

#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class ConfigWriter;

class ConfigReader
{
	class ConfigItem
	{
	public:
		ConfigItem(const std::string& s1, const std::string& s2)
		{
			m_key = s1;
			m_value = s2;
		}

		std::string m_key;
		std::string m_value;

		friend bool operator< (const ConfigItem& s1, const ConfigItem& s2)
		{
			return strcasecmp(s1.m_key.c_str(), s2.m_key.c_str()) < 0;
		}
	};

	static int compare_by_item_key(const void* key, const void* item);	


	static const int max_path = 256;
	static const int max_line_length = 256;
public:
	ConfigReader(const char* file_name);
	ConfigReader();
	~ConfigReader();

	bool load(const char* file_name = NULL);
	std::string get_string(const char* key, const char* default_value = NULL, bool log_warning = true);
	int get_int(const char* key, int default_value = 0, bool log_warning = true);

	const char* config_file() const { return m_file_name; }
	
protected:
	char m_file_name[max_path];
	std::vector<ConfigItem> m_items;

	bool loadLine(char* buffer);
	void trim(std::string&);
	void insertItem(const ConfigItem&);

	friend class ConfigWriter;

private:
	//DECL_LOGGER(logger);
};

class ConfigWriter : public ConfigReader
{
public:
	ConfigWriter(const char* file_name);
	~ConfigWriter() { }

	bool save(const char* file_name = NULL);
	
	void set_string(const char* key, const char* value);
	void set_int(const char* key, int value);

private:
//	ConfigReader m_cfg_reader;

private:
	//DECL_LOGGER(logger);
};


#endif
