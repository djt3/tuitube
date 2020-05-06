//
// Created by dan on 29/04/2020.
//

#ifndef TUITUBE_SEARCH_H
#define TUITUBE_SEARCH_H

#include <fstream>

#include "../utils.h"
#include "channel_view.h"

namespace tui::tabs::search {
    namespace {
        static bool searched = false;
        static bool view_channel = false;
        static std::string search_text = "";
        static std::string last_action = "";
        static bool awaiting_search = false;
        static bool request_update = false;
        static int selected = 0;
        static int scroll = 0;
        static std::vector<invidious::c_video> videos;
        static c_channel_view channel_view;

        static void do_search() {
            videos.clear();

            awaiting_search = true;

            std::string search_url = "/search?q=" + search_text;
            std::replace(search_url.begin(), search_url.end(), ' ', '+');

            videos = requests::extract_videos(search_url);

            awaiting_search = false;
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

    static void draw(const int& width, const int& height) {
        if (!searched)
            tui::utils::print_title("search", width , last_action);
        else
            tui::utils::print_title("search - " + search_text, width, last_action);

        if (view_channel) {
            channel_view.draw(width, height);
            return;
        }

        if (awaiting_search)
            printf("loading...");
        else if (!searched) {
            int i;
            for (i = 0; i < height / 2; i++)
                printf("\n");

            std::string text = "          [";
            text += search_text;
            //text.append("", width - 20 - search_text.size());
            for (int j = 0; j < width - 20 - search_text.size(); j++)
                text += " ";
            text += "]";
            printf(text.c_str());

            for (; i < height - 1; i++)
                printf("\n");
        } else {
            while (selected > height + scroll - 2)
                scroll++;
            while (selected < scroll)
                scroll--;

            tui::utils::print_videos(videos, selected, width, height, scroll);
        }


        terminal::set_background_color(terminal::e_color::white);
        terminal::set_text_color(terminal::e_color::black);

        if (searched)
            tui::utils::print_footer("[tab] change tab [s] show searchbox [a] subscribe", width);
        else
            tui::utils::print_footer("[tab] change tab [enter] search", width);
    }

    // TODO: arrow keys for input
    static void handle_input(const char& input) {
        request_update = true;

        if (input == 'a' && !videos.empty()) {
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

        if (searched) {
            if (input == 10 && !videos.empty()) { // enter
                request_update = false;
                terminal::clear();

                printf("playing video...\n");
                last_action = "played " + videos[selected].title;
                request_update = true;
                std::string cmd = config::playcmd_start
                                  + requests::extract_video_link(videos[selected])
                                  + config::playcmd_end;
                system(cmd.c_str());
                request_update = true;
            } else if (input == 's') {
                awaiting_search = false;
                searched = false;
                search_text = ""; // maybe don't reset this
                last_action = "";
                scroll = 0;
            } else if (input == 'c' && !videos.empty()) { // c - view channel
                view_channel = true;
                last_action = "view channel " + videos[selected].channel_name;
                channel_view = c_channel_view(videos[selected]);
                std::thread refresh_thread([]{channel_view.refresh_videos();});
                refresh_thread.detach();
            } else if (input == 65) { // up
                if (selected > 0)
                    selected--;
            } else if (input == 66) { // down
                if (selected < videos.size() - 1)
                    selected++;
            } else
                request_update = false;
        } else {
            if (input == 10 && !search_text.empty()) {
                searched = true;
                awaiting_search = true;
                std::thread search_thread(do_search);
                search_thread.detach();
            } else if (input == 127) {
                if (search_text.size() > 0)
                    search_text = search_text.substr(0, search_text.size() - 1);
            } else
                search_text += input;
        }
    }
}

#endif //TUITUBE_SEARCH_H
