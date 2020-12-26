#include <string>
#include <map>
#include <vector>

class ModelParameter
{
public:
	static bool Open(std::string path);
	static bool Save();
	static bool IsOpen();
	static void Clear() { map_parameter_.clear(); }

	static std::string get_string_value(std::string root, std::string key);
	static int get_int_value(std::string root, std::string key);
	static float get_float_value(std::string root, std::string key);
	static bool set_string_value(std::string root, std::string key,std::string value);
	static bool set_int_value(std::string root, std::string key,int value);
	static bool set_float_value(std::string root, std::string key,float value);
private:
	ModelParameter() = default;
	~ModelParameter() = default;
private:
	static std::string model_file_path_;
	static bool is_open_;
	static std::map<std::string, std::vector<std::pair<std::string,std::string>>> map_parameter_;
};
