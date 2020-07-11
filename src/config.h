#ifndef TUITUBE_CONFIG_H
#define TUITUBE_CONFIG_H
#include <unordered_map>
#include <string>

namespace config {
  extern std::unordered_map<std::string, std::string> config;
  extern void init();
  extern std::string get_value(const std::string& key);
  extern void write_config();

    // use these if your mpv cant play web videos
    // const static std::string playcmd_start = "youtube-dl -o - \"";
    // const static std::string playcmd_end = "\" | mpv -";

    const static std::string playcmd_start = "mpv \"";
    // url is placed in middle
    const static std::string playcmd_end = "\"";
}

#endif //TUITUBE_CONFIG_H
