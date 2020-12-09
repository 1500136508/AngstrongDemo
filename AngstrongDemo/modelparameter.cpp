#include "ModelParameter.h"
#include <fstream>

std::string &IgnoreSpace(std::string &str)
{
	std::string::size_type pos = 0;
	while (str.npos != (pos = str.find(" ")))
	{
		str = str.replace(pos, pos + 1, "");
	}
	return str;
}

std::string ModelParameter::model_file_path_ = "";
bool ModelParameter::is_open_ = false;
std::map<std::string, std::vector<std::pair<std::string, std::string>>> ModelParameter::map_parameter_;
bool ModelParameter::Open(std::string path)
{
	bool bReturn = false;
	do 
	{
		model_file_path_ = path;
		std::ifstream out_config_file(path.c_str());
		if (!out_config_file)
		{
			std::string err_msg = "failed to open the model file!";
			break;
		}

		std::string str_line = "";
		std::string str_root = "";
		while (getline(out_config_file, str_line))
		{
			std::string::size_type left_pos = 0;
			std::string::size_type right_pos = 0;
			std::string::size_type equal_div_pos = 0;
			std::string str_key = "";
			std::string str_value = "";
			if ((str_line.npos != (left_pos = str_line.find("["))) && (str_line.npos != (right_pos = str_line.find("]"))))
			{
				str_root = str_line.substr(left_pos + 1, right_pos - 1);
			}

			if (str_line.npos != (equal_div_pos = str_line.find("=")))
			{
				str_key = str_line.substr(0, equal_div_pos);
				str_value = str_line.substr(equal_div_pos + 1, str_line.size() - 1);
				str_key = IgnoreSpace(str_key);
				str_value = IgnoreSpace(str_value);
			}

			if ((!str_root.empty()) && (!str_key.empty()) && (!str_value.empty()))
			{
				std::vector<std::pair<std::string, std::string>> temp_vec_for_key_value;
				temp_vec_for_key_value.clear();
				temp_vec_for_key_value.push_back({ str_key,str_value });
				if (map_parameter_.empty())
				{
					map_parameter_.insert({ str_root, temp_vec_for_key_value });
				}
				else
				{
					auto iter = map_parameter_.find(str_root);
					if (iter != map_parameter_.end())
					{
						iter->second.push_back({ str_key, str_value });
					}
					else
					{
						map_parameter_.insert({ str_root, temp_vec_for_key_value });
					}
				}
			}
		}
		out_config_file.close();
		is_open_ = true;
		bReturn = true;
	} while (false);
	return bReturn;
}

bool ModelParameter::IsOpen()
{
	return is_open_;
}

std::string ModelParameter::get_string_value(std::string root, std::string key)
{
	std::string value("");
	if (!is_open_ || map_parameter_.size() <= 0)
	{
		return value;
	}

	auto iter = map_parameter_.find(root);
	if (iter == map_parameter_.end())
	{
		std::string output_msg = "failed to find the parameter!";
		return value;
	}
	
	if (!(iter->second).empty())
	{
		for (auto &v : iter->second)
		{
			if (v.first == key)
			{
				value = v.second;
			}
		}
	}
	return value;
}

int ModelParameter::get_int_value(std::string root, std::string key)
{
	int value = -1;
	if (get_string_value(root, key) == "")
	{
		return value;
	}
	value = std::stoi(get_string_value(root, key));
	return value;
}

float ModelParameter::get_float_value(std::string root, std::string key)
{
	float value = -1.;
	if (get_string_value(root, key) == "")
	{
		return value;
	}
	value = std::stof(get_string_value(root, key));
	return value;
}

bool ModelParameter::Save()
{
	bool bReturn = false;
	do 
	{
		std::ofstream out_config_file(model_file_path_.c_str());
		if (!out_config_file)
		{
			std::string err_msg = "failed to terminate model!";
			break;
		}
		if (map_parameter_.empty())
		{
			break;
		}
		for (auto iter = map_parameter_.cbegin(); iter != map_parameter_.cend(); ++iter)
		{
			out_config_file << "[" << iter->first << "]" << std::endl;
			if (!iter->second.empty())
			{
				for (auto &v : iter->second)
				{
					out_config_file << v.first << "=" << v.second << std::endl;
				}
			}
		}
		out_config_file.close();
		bReturn = true;
	} while (false);
	
	return 1;
}

bool ModelParameter::set_string_value(std::string root, std::string key, std::string value)
{
	bool bReture = false;
	do 
	{
		auto iter = map_parameter_.find(root);
		if (map_parameter_.end() != iter)
		{
			for (auto &v : iter->second)
			{
				if (key == v.first)
				{
					v.second = value;
				}
			}
		}
		else
		{
			std::vector<std::pair<std::string, std::string>> temp_vec_key_value;
			temp_vec_key_value.clear();
			temp_vec_key_value.push_back({ key,value });
			map_parameter_.insert({ root,temp_vec_key_value });
		}
		bReture = true;
	} while (false);
	return bReture;
}

bool ModelParameter::set_int_value(std::string root, std::string key, int value)
{
	return set_string_value(root,key,std::to_string(value));
}

bool ModelParameter::set_float_value(std::string root, std::string key, float value)
{
	return set_string_value(root,key,std::to_string(value));
}
