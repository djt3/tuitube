//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_VIDEO_H
#define TUITUBE_VIDEO_H

namespace invidious {
    class c_video {
    public:
        std::string title;
        std::string channel_name;
        std::string channel_url;
        std::string url;
        std::string time_str;
        std::string length;
        uint64_t time;

        void cleanup_text() {
          cleanup_str(title);
          cleanup_str(channel_name);
        }

        void cleanup_str(std::string& str) {
            auto pos = str.find("&quot;");
            while (pos != std::string::npos) {
                str.replace(pos, 6, "\"");
                pos = str.find("&quot;");
            }

            pos = str.find("&#");
            while (pos != std::string::npos) {
                if (str[pos + 4] == ';') {
                    char character = std::stoi(str.substr(pos + 2, 2));
                    str.replace(pos, 5, std::string(character, 1));
                } else
                    break;

                pos = str.find("&#");
            }

            pos = str.find("&amp;");
            while (pos != std::string::npos) {
                str.replace(pos, 5, "&");
                pos = str.find("&amp;");
            }

            pos = str.find("–");
            while (pos != std::string::npos) {
              str.replace(pos, 3, "-");
              pos = str.find("–");
            }

            pos = str.find("…");
            while (pos != std::string::npos) {
              str.replace(pos, 3, "...");
              pos = str.find("…");
            }

            pos = str.find("’");
            while (pos != std::string::npos) {
              str.replace(pos, 3, "'");
              pos = str.find("’");
            }
        }
    };
}

#endif //TUITUBE_VIDEO_H
