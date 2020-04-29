//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_SUBSCRIPTIONS_H
#define TUITUBE_SUBSCRIPTIONS_H

#include <thread>
#include <filesystem>
#include <fstream>

#include "invidious/video.h"
#include "requests.h"

namespace subscriptions {
    namespace {
        static bool awaiting_refresh = false;
        static bool no_subs = false;
        static int selected = 0;
        static int scroll = 0;
        static std::vector<invidious::c_video> videos;

        // TODO: implement threading
        static void refresh_subs() {
            videos.clear();

            const static std::string path = std::string(getenv("HOME")) + "/.config/tuitube_subs";
            if (!std::filesystem::exists(path)) {
                awaiting_refresh = false;
                no_subs = true;
                return;
            }

            awaiting_refresh = true;
            no_subs = false;

            std::string line;
            std::ifstream sub_file(path);
            while (std::getline(sub_file, line)) {
                auto channel_vids = requests::extract_videos(line);
                if (channel_vids.empty())
                    continue;

                // append the current channel's videos
                videos.insert(videos.end(), channel_vids.begin(), channel_vids.end());
            }

            auto sort_fn = [](const invidious::c_video &v1, const invidious::c_video &v2) {
                return v1.time < v2.time;
            };
            std::sort(videos.begin(), videos.end(), sort_fn);

            awaiting_refresh = false;
        }
    }

    static void draw(const int& width, const int& height) {
        if (awaiting_refresh)
            printf("loading...");
        else if (no_subs)
            printf("sub list empty");
        else if (videos.empty())
            printf("no videos found");
        else {
            while (selected > height + scroll - 1)
                scroll++;
            while (selected < 0 + scroll)
                scroll--;

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

        terminal::set_background_color(terminal::e_color::white);
        terminal::set_text_color(terminal::e_color::black);

        std::string binds = "[q] quit [r] refresh";
        if (binds.size() > width)
            binds = binds.substr(0, width - 3) + "...";

        printf("\n");
        printf(binds.c_str());
        for (int i = 0; i < width - binds.length(); i++)
            printf(" ");

        terminal::reset();
    }

    // returns true if a refresh is required
    static bool handle_input(const char& input) {
        if (input == 10) { // enter - open video
            std::string cmd = "mpv \"" + requests::extract_video_link(videos[selected]) + "\"";
            system(cmd.c_str());
            return true;
        }
        if (input == 'r') { // r - refresh TODO: locks up i/o, thread will redraw at wrong time
            if (!awaiting_refresh) {
                //std::thread refresh_thread(refresh_subs);
                //refresh_thread.detach();
                refresh_subs();
            }

            return true;
        } else if (input == 65) { // up
            if (selected > 0)
                selected--;
            return true;
        } else if (input == 66) { // down
            if (selected < videos.size())
                selected++;
            return true;
        }

        return false;
    }
}

#endif //TUITUBE_SUBSCRIPTIONS_H
