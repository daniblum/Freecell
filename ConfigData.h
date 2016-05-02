#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

class ConfigData
{
public:
  ~ConfigData(void);
  ConfigData(const char* fn);
  bool getBool(const char* key, bool defValue = false);
  void setBool(const char* key, bool value);
  static const char* DELAYED_CLEANUP;
  static const char* STRICT_DELAYED_CLEANUP;
  static const char* SAVE_STATE;
private:
  std::string fileName;
  boost::property_tree::ptree tree ;
  void load(void);
  void save(void);
};
