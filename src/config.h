#ifndef TUITUBE_CONFIG_H
#define TUITUBE_CONFIG_H

#include <unordered_map>
#include <string>

namespace config {
  const static std::string config_file_path = std::string(getenv("HOME")) + "/.config/tuitube/config";
  static std::unordered_map<std::string, std::string> config;

  static void init() {
    std::ifstream config_file(config_file_path);
    std::string key;
    while (std::getline(config_file, key)) {
      std::string value;
      std::getline(config_file, value);

      config[key] = value;
    }
  }

  static std::string get_value(const std::string& key) {
    if (config[key].empty())
      config[key] = "0";

    return config[key];
  }

  static void set_value(const std::string& key, const std::string& value) {
    config[key] = value;
  }

  static void write_config() {
    std::ofstream file;
    file.open(config_file_path.c_str());

    for (const auto& setting : config) {
        file << setting.first << std::endl << setting.second << std::endl;
    }

    file.close();
  }

    // use these if your mpv cant play web videos
    // const static std::string playcmd_start = "youtube-dl -o - \"";
    // const static std::string playcmd_end = "\" | mpv -";

    const static std::string playcmd_start = "mpv \"";
    // url is placed in middle
    const static std::string playcmd_end = "\"";
}

#endif //TUITUBE_CONFIG_H
