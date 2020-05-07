//
// Created by dan on 30/04/2020.
//

#ifndef TUITUBE_POPULAR_H
#define TUITUBE_POPULAR_H

#include "channel_view.h"

namespace tui::tabs::popular {
    namespace {
        static bool request_update = false;
        static bool view_channel = false;
        static int selected = 0;
        static int scroll = 0;
        static std::vector<invidious::c_video> videos;
        static std::string last_action = "";
        static c_channel_view channel_view;

        static void refresh_videos() {
            last_action = "refreshing...";
            videos.clear();
            request_update = true;

            videos = requests::extract_videos("/");
            last_action = "";
            request_update = true;
        }
    }

    static bool is_update_required() {
        if(view_channel)
            return channel_view.request_update;

        if (request_update) {
            request_update = false;
            return true;
        }
        return false;
    }

    static void draw(const int &width, const int &height) {
        static std::once_flag flag;
        std::call_once(flag, []() {
            std::thread refresh_thread(refresh_videos);
            refresh_thread.detach();
        });

        tui::utils::print_title("popular", width, last_action);

        if (view_channel) {
            channel_view.draw(width, height);
            return;
        }

        if (!videos.empty()) {
            while (selected > height + scroll - 2)
                scroll++;
            while (selected < scroll)
                scroll--;

            tui::utils::print_videos(videos, selected, width, height, scroll);
        }

        tui::utils::print_footer("[tab] change tab [q] quit [r] refresh [a] subscribe [c] view channel", width);
    }

    static void handle_input(const char &input) {
        request_update = true;

        if (input == 'a' && !videos.empty()) { // a - subscribe
            subscriptions::add_sub(videos[selected]);
            last_action = "subscribed to " + videos[selected].channel_url;
        }

        if (view_channel) {
            if (input == 'b')
                view_channel = false;
            else
                channel_view.handle_input(input);

            return;
        }

        if (input == 10 && !videos.empty()) { // enter - open video
            request_update = false;
            terminal::clear();

            printf("%s", "playing video...\n");
            last_action = "played " + videos[selected].title;
            request_update = true;
            std::string cmd = config::playcmd_start
                              + requests::extract_video_link(videos[selected])
                              + config::playcmd_end;

            system(cmd.c_str());
            request_update = true;
        } else if (input == 'r' && last_action != "refreshing...") { // r - refresh
            std::thread refresh_thread(refresh_videos);
            refresh_thread.detach();
            last_action = "";
        } else if (input == 'c' && !videos.empty()) { // c - view channel
            view_channel = true;
            last_action = "view channel " + videos[selected].channel_name;
            channel_view = c_channel_view(videos[selected]);
            std::thread refresh_thread([]{channel_view.refresh_videos();});
            refresh_thread.detach();
        } else if (input == 65 || input == 'j') { // up
            if (selected > 0)
                selected--;
        } else if (input == 66 || input == 'k') { // down
            if (selected < videos.size() - 1)
                selected++;
        } else if (input != 'a')
            request_update = false;
    }
}

#endif //TUITUBE_POPULAR_H
