#ifndef TUITUBE_CONFIG_H
#define TUITUBE_CONFIG_H
#include "invidious/instances.h"
#include <string>
#include <unordered_map>

namespace config {
const static std::string config_file_path =
    std::string(getenv("HOME")) + "/.config/tuitube/config";
static std::unordered_map<std::string, std::string> config;

static void init() {
    std::ifstream config_file(config_file_path);
    std::string key;
    while (std::getline(config_file, key)) {
        std::string value;
        std::getline(config_file, value);

        config[key] = value;
    }

    // default values
    if (config.find(std::string("Invidious Instance")) == config.end())
        config[std::string("Invidious Instance")] = "invidious.snopyta.org";
    if (config.find(std::string("Instance For Popular Videos")) == config.end())
        config[std::string("Instance For Popular Videos")] =
            "invidious.snopyta.org";
    if (config.find(std::string("Video Source")) == config.end())
        config[std::string("Video Source")] = "Invidious";
}

static std::string get_value(const std::string &key) {
    if (config[key].empty())
        config[key] = "0";

    if (config[key] == "fastest") {
        return invidious::instances::get_fastest_instance();
    }

    return config[key];
}

static void set_value(const std::string &key, const std::string &value) {
    config[key] = value;
}

static void write_config() {
    std::ofstream file;
    file.open(config_file_path.c_str());

    for (const auto &setting : config) {
        file << setting.first << std::endl << setting.second << std::endl;
    }

    file.close();
}

// use these if your mpv cant play web videos
// const static std::string playcmd_start = "youtube-dl -o - \"";
// const static std::string playcmd_end = "\" | mpv -";

const static std::string max_resolution = "1080"; // set maximum resolution here
const static std::string playcmd_start =
    "mpv --ytdl-format=\"bestvideo[height<=?" + max_resolution +
    "]+bestaudio/best\"  \""; // mpv runs youtube-dl with right arguments

// old   const static std::string playcmd_start = "mpv \"";
// url is placed in middle
const static std::string playcmd_end = "\"";
} // namespace config

#endif // TUITUBE_CONFIG_H
