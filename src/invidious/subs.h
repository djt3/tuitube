#ifndef TUITUBE_SUBS_H
#define TUITUBE_SUBS_H

namespace invidious::subs {
  static std::vector<std::string> channels;
  const static std::string subs_file_path = std::string(getenv("HOME")) + "/.config/tuitube_subs";

  // returns true if the subs file needs cleaning
  static bool refresh_subs_file() {
    channels.clear();

    bool needs_cleaning = false;
    std::string channel;
    std::ifstream sub_file(subs_file_path);
    while (std::getline(sub_file, channel)) {
      if (channel == "" || std::find(channels.begin(), channels.end(), channel) != channels.end()) {
        needs_cleaning = true;
        continue;
      }

      channels.push_back(channel);
    }

    return needs_cleaning;
  }

  static void write_subs(const std::string& url_for_deletion = "") {
    std::ofstream file;
    file.open(subs_file_path.c_str());

    for (const auto &channel : channels) {
      if (channel != url_for_deletion)
        file << channel << std::endl;
    }

    file.close();
    refresh_subs_file();
  }

  static void add_sub(const invidious::c_video& video) {
    std::ofstream file;
    file.open(subs_file_path.c_str(), std::ios_base::app);
    file << std::endl << video.channel_url;
    file.close();
  }
}

#endif
