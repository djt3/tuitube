//
// Created by dan on 29/04/2020.
//

#ifndef TUITUBE_UTILS_H
#define TUITUBE_UTILS_H

#include "../invidious/video.h"

namespace tui {
    namespace utils {
        static void print_title(const std::string& title, int width) {
            terminal::set_background_color(terminal::e_color::white);
            terminal::set_text_color(terminal::e_color::black);

            if (title.size() > width)
                printf(title.substr(0, width - 3).c_str(), "...");
            else {
                printf(title.c_str());

                for (int i = 0; i < width - title.size(); i++)
                    printf(" ");
            }

            terminal::reset();
        }

        static void print_videos(const std::vector<invidious::c_video>& videos, int selected, int width, int height, int scroll) {
            int max = std::min(height + scroll, static_cast<int>(videos.size()));
            for (int i = scroll; i < max; i++) {
                const auto &video = videos[i];

                if (i == selected) {
                    terminal::set_background_color(terminal::e_color::white);
                    terminal::set_text_color(terminal::e_color::black);
                }

                std::string text = video.channel + " - " + video.title;
                if (text.size() > width)
                    text = text.substr(0, width - 3) + "...";

                printf(text.c_str());
                if (i != max - 1)
                    printf("\n");

                if (i == selected)
                    terminal::reset();
            }
        }

        static void print_footer(const std::string& binds, int width) {
            terminal::set_background_color(terminal::e_color::white);
            terminal::set_text_color(terminal::e_color::black);

            printf("\n");

            if (binds.size() > width)
                printf(binds.substr(0, width - 3).c_str(), "...");
            else {
                printf(binds.c_str());

                for (int i = 0; i < width - binds.length(); i++)
                    printf(" ");
            }

            terminal::reset();
        }
    }
}

#endif //TUITUBE_UTILS_H
