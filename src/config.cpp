#include "config.h"
#include <fstream>

namespace config {
  const static std::string config_file_path = std::string(getenv("HOME")) + "/.config/tuitube/config";
  std::unordered_map<std::string, std::string> config;

  void init() {
    std::ifstream config_file(config_file_path);
    std::string key;
    while (std::getline(config_file, key)) {
      std::string value;
      std::getline(config_file, value);

      config[key] = value;
    }

    if (config.find(std::string("Invidious Instance")) == config.end())
      config[std::string("Invidious Instance")] = "invidious.snopyta.org";
    if (config.find(std::string("Video Source")) == config.end())
      config[std::string("Video Source")] = "Invidious";
  }

  std::string get_value(const std::string& key) {
    if (config[key].empty())
      config[key] = "0";
    return config[key];
  }

  void write_config() {
    std::ofstream file;
    file.open(config_file_path.c_str());

    for (const auto& setting : config) {
        file << setting.first << std::endl << setting.second << std::endl;
    }

    file.close();
  }
}