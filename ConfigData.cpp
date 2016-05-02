#include "ConfigData.h"

const char* ConfigData::DELAYED_CLEANUP = "menu.settings.delayed_cleanup";
const char* ConfigData::STRICT_DELAYED_CLEANUP = "menu.settings.strict_delayed_cleanup";
const char* ConfigData::SAVE_STATE = "menu.settings.save_state";

ConfigData::ConfigData(const char* fn): fileName(fn)
{
  load();
}

ConfigData::~ConfigData(void)
{
	boost::property_tree::write_json(fileName, tree);
}

void ConfigData::load(void)
{
  // see if tree is empty - if not, throw data?
  if (!tree.empty()) {
    // sound alarm?
    tree.clear();
  }
  try {
    boost::property_tree::read_json(fileName, tree);
  } catch(...) {
  }

}

bool ConfigData::getBool(const char* key, bool defValue)
{
  bool result = defValue;
  try {
    result = tree.get<bool>(key);
  } catch(...) {
    // In case key is not present, create it
    setBool( key, result );
  }
  return result;
}

void ConfigData::setBool(const char* key, bool value)
{
  try {
    tree.put<bool>(key, value);
  } catch(...) {
  }
}
