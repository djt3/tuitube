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
        uint64_t time;

        void cleanup_title() {
            auto pos = title.find("&quot;");
            while (pos != std::string::npos) {
                title.replace(pos, 6, "\"");
                pos = title.find("&quot;");
            }

            pos = title.find("&#");
            while (pos != std::string::npos) {
                if (title[pos + 4] == ';') {
                    char character = std::stoi(title.substr(pos + 2, 2));
                    title.replace(pos, 5, std::string(character, 1));
                } else
                    break;
                
                pos = title.find("&#");
            }

            pos = title.find("&amp;");
            while (pos != std::string::npos) {
                title.replace(pos, 5, "&");

                pos = title.find(" &amp;");
            }
        }
    };
}

#endif //TUITUBE_VIDEO_H
