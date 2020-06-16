//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_SUBSCRIPTIONS_H
#define TUITUBE_SUBSCRIPTIONS_H

#include <thread>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <algorithm>

#include "../../invidious/video.h"
#include "../../invidious/subs.h"
#include "../../requests.h"
#include "../../config.h"
#include "../utils.h"
#include "channel_view.h"
#include "generic_tab.h"

namespace tui::tabs {
  class c_subscriptions_tab : public c_generic_tab {
  private:
    bool viewing_sublist = false;
    std::vector<invidious::c_video> channel_videos;

  public:
    c_subscriptions_tab() {
      title = "subscriptions";
      can_subscribe = false;
      custom_actions.push_back(action("unsubscribe", 'u', nullptr));
      custom_actions.push_back(action("view sublist", 'l', nullptr));
      generate_footer();
    }

    void refresh_videos() {
      videos.clear();
      invidious::subs::refresh_subs_file();

      if (!std::filesystem::exists(invidious::subs::subs_file_path)) {
        last_action = "subs file empty";
        request_update = true;
        return;
      }

      last_action = "refreshing...";
      request_update = true;

      // cleans up duplicated and empty lines in the subs file
      if (invidious::subs::refresh_subs_file())
        invidious::subs::write_subs();

      std::vector<std::thread> threads{};

      if (invidious::subs::channels.empty()) {
        last_action = "subs file empty";
        request_update = true;
        return;
      }

      for (const auto &channel : invidious::subs::channels) {
        auto scrape_fn = [&]() {
                           auto channel_vids = requests::extract_videos("/channel/" + channel, channel);
                           if (channel_vids.empty())
                             return;

                           videos.insert(videos.end(), channel_vids.begin(), channel_vids.end());
                           request_update = true;
                         };

        threads.push_back(std::thread(scrape_fn));
      }

      for (auto &thread : threads)
        thread.join();

      if (videos.empty()) {
        last_action = "no videos found";
        request_update = true;
        return;
      }

      auto sort_fn = [](const invidious::c_video &v1, const invidious::c_video &v2) -> bool {
                       return v1.time < v2.time;
                     };
      std::sort(videos.begin(), videos.end(), sort_fn);

      c_generic_tab::refresh_videos();
    }

    void draw(const int &width, const int &height) {
      if (viewing_sublist) {
        tui::utils::print_title("sublist", width, last_action);

        if (view_channel) {
          channel_view.draw(width, height);
          return;
        }

        while (selected > height + scroll - 3)
          scroll++;
        while (selected < scroll)
          scroll--;

        tui::utils::print_videos(channel_videos, selected, width, height, scroll, true);

        tui::utils::print_footer(footer, width, force_update);
        force_update = false;
      }

      else
        c_generic_tab::draw(width, height);
    }

    void handle_input(const char &input) {
      request_update = true;
      if (input == 'u' && !videos.empty()) {
        if (!viewing_sublist) {
          invidious::subs::write_subs(videos[selected].channel_url);
          last_action = "deleted " + videos[selected].channel_name;
        } else {
          invidious::subs::write_subs(channel_videos[selected].channel_url);
          last_action = "deleted " + channel_videos[selected].channel_name;
        }
      } else if (input == 'l' && !videos.empty()) {
        viewing_sublist = !viewing_sublist;
        request_update = true;
        selected = 0;
        channel_videos.clear();

        std::vector<std::string> channels_added;

        for (int i = 0; i < videos.size(); i++) {
          std::string text = videos[i].channel_name;

          if (std::find(channels_added.begin(), channels_added.end(), text) == channels_added.end()) {
            channel_videos.push_back(videos[i]);
            channels_added.push_back(text);
          }

          auto sort_fn = [](const invidious::c_video &v1, const invidious::c_video &v2) -> bool {
                           return v1.channel_name < v2.channel_name;
                         };
          std::sort(channel_videos.begin(), channel_videos.end(), sort_fn);
        }

      }
      else if (input == 10 && viewing_sublist) {
        view_channel = true;
        last_action = "view channel " + videos[selected].channel_name;
        channel_view = c_channel_view(channel_videos[selected]);
        std::thread refresh_thread([&]{channel_view.refresh_videos();});
        refresh_thread.detach();
      }
      else
        c_generic_tab::handle_input(input);

      if (input == 'r' && viewing_sublist)
        viewing_sublist = false;

      if (viewing_sublist && selected >= channel_videos.size())
          selected = channel_videos.size() - 1;
    }
  };
}

#endif //TUITUBE_SUBSCRIPTIONS_H
