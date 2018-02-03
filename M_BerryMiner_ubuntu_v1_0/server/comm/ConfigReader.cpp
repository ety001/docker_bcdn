#include <algorithm>
#include "ConfigReader.h"

using namespace std;

//IMPL_LOGGER(ConfigReader, logger);
//IMPL_LOGGER(ConfigWriter, logger);

int ConfigReader::compare_by_item_key(const void* key, const void* item)
{
	const char* cfg_key = (const char*)key;
	ConfigItem* cfg_item = (ConfigItem*)item;

	return strcasecmp(cfg_key, cfg_item->m_key.c_str());
}

ConfigReader::ConfigReader(const char* file_name)
{
	strcpy(m_file_name, file_name);
}

ConfigReader::ConfigReader()
{
	m_file_name[0] = '\0';
}

ConfigReader::~ConfigReader()
{
	m_items.clear();
}

// 读取一个文件的内容，并保存到m_items 中
bool ConfigReader::load(const char* file_name)
{
	if(file_name != NULL)
	{
		strcpy(m_file_name, file_name);
	}

	if(strlen(m_file_name) < 1)
	{
		//LOG4CPLUS_THIS_ERROR(logger, "Configuration file name not specified!");
		return false;
	}

	char buffer[max_line_length], *p_ret;
	FILE* fp = fopen(m_file_name, "r");

	if(!fp)
	{
		return false;
	}

	m_items.clear();
	while(true) 
	{
		p_ret = fgets(buffer, max_line_length, fp);
		if(!p_ret)
		{
			break;
		}
		loadLine(buffer);
	}
	fclose(fp);
	return true;
}

// 分析文件1 行的内容，分隔符为"="
bool ConfigReader::loadLine(char* buffer)
{
	if(buffer && buffer[0] == '#')
	{
		// this is comment line, ignore
		return false;
	}

	char *p_eq = strchr(buffer, '=');
	if(p_eq)
	{
		int pos = p_eq - buffer;
		char *p_LF = strrchr(buffer, '\n');
		string key = string(buffer, 0, pos);
		string value = string(buffer, pos + 1, p_LF - p_eq - 1);
		trim(key);
		trim(value);
		insertItem(ConfigItem(key, value));
	}
	else
	{
		return false;
	}
	return true;
}

// 去掉字符串头尾的空格
void ConfigReader::trim(string& s)
{
	int bpos = s.find_first_not_of(0x20);
	int epos = s.find_last_not_of(0x20);
	if(bpos < 0 || epos < 0)
		s.clear();
	else
		s = string(s, bpos, epos - bpos + 1);
}

void ConfigReader::insertItem(const ConfigItem& item)
{
	vector<ConfigItem>::iterator it_find 
		= lower_bound(m_items.begin(), m_items.end(), item);

	if(it_find == m_items.end() || it_find->m_key != item.m_key)
	{
		m_items.insert(it_find, item);
	}
}

string ConfigReader::get_string(const char* key, const char* default_value, bool log_warning)
{
	if(key == NULL || m_items.size() == 0)
		return default_value ? default_value : "";	

	ConfigItem* target = (ConfigItem*)bsearch(key, &m_items[0], m_items.size(), sizeof(ConfigItem), compare_by_item_key);
	if(target != NULL)
	{
//		//LOG4CPLUS_THIS_DEBUG(logger, "found config item: " << key << "=" << target->m_value)
		return target->m_value;
	}
	else
	{
		if(log_warning)
		{
//			//LOG4CPLUS_THIS_WARN(logger, "Config item '" << key << "' not found in file [" << m_file_name << "].");
		}
		return default_value ? default_value : "";	
	}

}

int ConfigReader::get_int(const char* key, int default_value, bool log_warning)
{
	if(key == NULL  || m_items.size() == 0)
		return default_value;	

	ConfigItem* target = (ConfigItem*)bsearch(key, &m_items[0], m_items.size(), sizeof(ConfigItem), compare_by_item_key);
	if(target != NULL)
	{
//		//LOG4CPLUS_THIS_DEBUG(logger, "found config item: " << key << "=" << target->m_value)
		return atoi(target->m_value.c_str());
	}
	else
	{
		if(log_warning)
		{
//			//LOG4CPLUS_THIS_WARN(logger, "Config item '" << key << "' not found in file [" << m_file_name << "].");	
		}
		return default_value;	
	}
}

ConfigWriter::ConfigWriter(const char* file_name) : ConfigReader(file_name)
{
	bool succ = load();
	if(!succ)
	{
		//LOG4CPLUS_THIS_ERROR(logger, "failed to load configuration file '" << file_name << "'!");
	}
}

void ConfigWriter::set_string(const char * key, const char * value)
{
	if(key == NULL || value == NULL)
		return;

	vector<ConfigReader::ConfigItem>& items = m_items;
	ConfigReader::ConfigItem* target = (ConfigReader::ConfigItem*)bsearch(key, &items[0], items.size(), 
		sizeof(ConfigReader::ConfigItem), ConfigReader::compare_by_item_key);
	if(target != NULL)
	{
		target->m_value = value;
	}
	else
	{
		ConfigReader::ConfigItem item((string)key, (string)value);
		insertItem(item);
	}	
}

void ConfigWriter::set_int(const char * key, int value)
{
	if(key == NULL)
		return;

	char value_text[32];
	snprintf(value_text, sizeof(value_text), "%d", value);

	vector<ConfigReader::ConfigItem>& items = m_items;
	ConfigReader::ConfigItem* target = (ConfigReader::ConfigItem*)bsearch(key, &items[0], items.size(), 
		sizeof(ConfigReader::ConfigItem), ConfigReader::compare_by_item_key);
	if(target != NULL)
	{
		target->m_value = value_text;
	}
	else
	{
		ConfigReader::ConfigItem item((string)key, (string)value_text);
		insertItem(item);
	}		
}

bool ConfigWriter::save(const char * file_name)
{
	const char* filepath = file_name;
	if(filepath == NULL)
		filepath = config_file();

	if(filepath == NULL)
	{
		//LOG4CPLUS_THIS_ERROR(logger, "config file name not specified when try to save it!");
		return false;
	}

	FILE* fp = fopen(filepath, "w"); 
	if(fp == NULL)
	{
		//LOG4CPLUS_THIS_ERROR(logger, "can not open config file '" << filepath << "' in write mode!");
		return false;
	}

	char line[256];
	snprintf(line, sizeof(line), "## DO NOT MODIFY!!\n");
	fputs(line, fp);
	
	vector<ConfigReader::ConfigItem>& items = m_items;	
	for(int i = 0, size = items.size(); i < size; i++)
	{
		ConfigReader::ConfigItem& item = items[i];
		snprintf(line, sizeof(line), "%s=%s\n", item.m_key.c_str(), item.m_value.c_str());
		fputs(line, fp);
	}
	fclose(fp);	

	return true;
}


