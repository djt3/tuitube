//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_SUBSCRIPTIONS_H
#define TUITUBE_SUBSCRIPTIONS_H

#include <thread>
#include <filesystem>
#include <fstream>
#include <mutex>

#include "../../invidious/video.h"
#include "../../requests.h"
#include "../../config.h"
#include "../utils.h"

namespace tui::tabs::subscriptions {
    namespace {
        static bool request_update = false;
        static int selected = 0;
        static int scroll = 0;
        static std::vector<invidious::c_video> videos;
        static std::vector<std::string> channels;
        const static std::string subs_file_path = std::string(getenv("HOME")) + "/.config/tuitube_subs";
        static std::string last_action = "refreshing...";


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

        static void write_subs(bool delete_current = false) {
            std::ofstream file;
            file.open(subs_file_path.c_str());

            for (const auto &channel : channels) {
                if (!delete_current || channel != videos[selected].channel_url)
                    file << channel << std::endl;
                else
                    last_action = "deleted " + videos[selected].channel_url;
            }

            file.close();
            refresh_subs_file();
        }

        static void refresh_videos() {
            videos.clear();

            if (!std::filesystem::exists(subs_file_path)) {
                last_action = "subs file empty";
                return;
            }

            last_action = "refreshing...";
            request_update = true;

            // cleans up duplicated and empty lines in the subs file
            if (refresh_subs_file())
                write_subs();

						std::vector<std::thread> threads { };

            for (const auto& channel : channels) {
								auto scrape_fn = [=]() {
										auto channel_vids = requests::extract_videos("/channel/" + channel, channel);
										if (channel_vids.empty())
												return;

										videos.insert(videos.end(), channel_vids.begin(), channel_vids.end());
										request_update = true;
								};
								
								threads.push_back(std::thread(scrape_fn));
            }

						for (auto& thread : threads)
							thread.join();

            if (videos.empty()) {
                last_action = "no videos found";
                return;
            }

            auto sort_fn = [](const invidious::c_video &v1, const invidious::c_video &v2) {
                return v1.time < v2.time;
            };
            std::sort(videos.begin(), videos.end(), sort_fn);
            request_update = true;

            last_action = "";
        }
    }

    static bool is_update_required() {
        if (request_update) {
            request_update = false;
            return true;
        }
        return false;
    }

    static void add_sub(const invidious::c_video& video) {
        std::ofstream file;
        file.open(subs_file_path.c_str(), std::ios_base::app);
        file << std::endl << video.channel_url;
        file.close();
    }

    static void draw(const int &width, const int &height) {
        static std::once_flag flag;
        std::call_once(flag, []() {
            std::thread refresh_thread(refresh_videos);
            refresh_thread.detach();
        });

        tui::utils::print_title("subscriptions", width, last_action);

        if (!videos.empty()) {
            while (selected > height + scroll - 2)
                scroll++;
            while (selected < scroll)
                scroll--;

            tui::utils::print_videos(videos, selected, width, height, scroll);
        }

        tui::utils::print_footer("[tab] change tab [q] quit [r] refresh [d] unsubscribe", width);
    }

    static void handle_input(const char &input) {
        request_update = true;

        if (input == 10 && !videos.empty()) { // enter - open video
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
        } else if (input == 'r' && last_action != "refreshing...") { // r - refresh
                std::thread refresh_thread(refresh_videos);
                refresh_thread.detach();
            last_action = "";
        } else if (input == 'd' && last_action != "refreshing..." && !videos.empty()) {
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
