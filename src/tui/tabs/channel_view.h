//
// Created by dan on 05/05/2020.
//

#ifndef TUITUBE_CHANNEL_VIEW_H
#define TUITUBE_CHANNEL_VIEW_H

#include <mutex>
#include <thread>
#include <vector>

#include "../../invidious/video.h"

namespace tui::tabs {
    class c_channel_view {
    public:
        bool request_update = false;

        c_channel_view() {}

        c_channel_view(invidious::c_video video) {
            this->channel_url = video.channel_url;
        }

        void refresh_videos() {
            last_action = "refreshing...";
            videos.clear();
            request_update = true;

            videos = requests::extract_videos("/channel/" + channel_url);
            last_action = "";
            request_update = true;
            last_action = "";
        }

        void draw(const int &width, const int &height) {
            if (!videos.empty()) {
                while (selected > height + scroll - 2)
                    scroll++;
                while (selected < scroll)
                    scroll--;

                tui::utils::print_videos(videos, selected, width, height, scroll);
            }

            tui::utils::print_footer("[tab] change tab [b] back [a] subscribe [c] view channel", width);
        }

        bool handle_input(const char &input) {
            request_update = true;

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
            } else if (input == 'a' && !videos.empty()) { // a - subscribe
                last_action = "subscribed to " + videos[selected].channel_url;
                return true;
            } else if (input == 65 || input == 'j') { // up
                if (selected > 0)
                    selected--;
            } else if (input == 66 || input == 'k') { // down
                if (selected < videos.size() - 1)
                    selected++;
            } else
                request_update = false;

            return false;
        }

    private:
        std::string channel_url;
        int selected = 0;
        int scroll = 0;
        std::vector<invidious::c_video> videos;
        std::string last_action = "";
    };
}

#endif //TUITUBE_CHANNEL_VIEW_H
