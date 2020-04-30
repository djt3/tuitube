//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_SUBSCRIPTIONS_H
#define TUITUBE_SUBSCRIPTIONS_H

#include <thread>
#include <filesystem>
#include <fstream>
#include <mutex>

#include "../invidious/video.h"
#include "../requests.h"
#include "../config.h"
#include "utils.h"

namespace subscriptions {
    namespace {
        static bool awaiting_refresh = false;
        static bool no_subs = false;
        static bool request_update = false;
        static int selected = 0;
        static int scroll = 0;
        static std::vector<invidious::c_video> videos;
        static std::vector<std::string> subs;

        static void write_subs(bool delete_current = false) {
            const static std::string path = std::string(getenv("HOME")) + "/.config/tuitube_subs";

            std::ofstream file;
            file.open(path.c_str());

            for (const auto& channel : subs) {
                if (!delete_current || channel != videos[selected].channel_url)
                    file << channel << std::endl;
            }

            file.close();
        }

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
            videos.clear();
            subs.clear();

            bool found_duplicate = false;
            std::string line;
            std::ifstream sub_file(path);
            while (std::getline(sub_file, line)) {
                if (line == "" || std::find(subs.begin(), subs.end(), line) != subs.end()) {
                    found_duplicate = true;
                    continue;
                }
                subs.push_back(line);
                auto channel_vids = requests::extract_videos("/channel/" + line);
                if (channel_vids.empty())
                    continue;

                // append the current channel's videos
                videos.insert(videos.end(), channel_vids.begin(), channel_vids.end());
            }

            // cleanup subs file
            if (found_duplicate)
                write_subs();

            auto sort_fn = [](const invidious::c_video &v1, const invidious::c_video &v2) {
                return v1.time < v2.time;
            };
            std::sort(videos.begin(), videos.end(), sort_fn);

            awaiting_refresh = false;
            request_update = true;
        }
    }

    static bool is_update_required() {
        if (request_update) {
            request_update = false;
            return true;
        }

        return false;
    }

    static void draw(const int& width, const int& height) {
        static std::once_flag flag;
        std::call_once(flag, []() {
            std::thread refresh_thread(refresh_subs);
            refresh_thread.detach();
        });

        tui::utils::print_title("subscriptions", width);

        if (awaiting_refresh)
            printf("loading...");
        else if (no_subs)
            printf("sub list empty");
        else if (videos.empty())
            printf("no videos found");
        else {
            while (selected > height + scroll - 2)
                scroll++;
            while (selected < scroll)
                scroll--;

            tui::utils::print_videos(videos, selected, width, height, scroll);
        }

        tui::utils::print_footer("[tab] search [q] quit [r] refresh [d] delete sub", width);
    }

    static void handle_input(const char& input) {
        request_update = true;

        if (input == 10 && !videos.empty()) { // enter - open video
            request_update = false;
            terminal::clear();

            printf("playing video...\n");
            std::string cmd = config::playcmd_start + requests::extract_video_link(videos[selected]) + config::playcmd_end;
            system(cmd.c_str());
        } else if (input == 'r' && !awaiting_refresh) { // r - refresh
            if (!awaiting_refresh) {
                std::thread refresh_thread(refresh_subs);
                refresh_thread.detach();
            }
        } else if (input == 'd' && !awaiting_refresh && !videos.empty()) {
            write_subs(true);
        } else if (input == 65) { // up
            if (selected > 0)
                selected--;
        } else if (input == 66) { // down
            if (selected < videos.size() - 1)
                selected++;
        } else
            request_update = false;
    }
}

#endif //TUITUBE_SUBSCRIPTIONS_H
