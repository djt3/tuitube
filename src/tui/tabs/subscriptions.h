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
#include "../../invidious/subs.h"
#include "../../requests.h"
#include "../../config.h"
#include "../utils.h"
#include "channel_view.h"
#include "generic_tab.h"

namespace tui::tabs {
  class c_subscriptions_tab : public c_generic_tab {
  public:
    c_subscriptions_tab() {
      title = "subscriptions";
      can_subscribe = false;
      custom_actions.push_back(action("unsubscribe", 'u', nullptr));
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

      auto sort_fn = [](const invidious::c_video &v1, const invidious::c_video &v2) {
                       return v1.time < v2.time;
                     };
      std::sort(videos.begin(), videos.end(), sort_fn);

      c_generic_tab::refresh_videos();
    }

    void draw(const int &width, const int &height) {
      c_generic_tab::draw(width, height);
    }

    void handle_input(const char &input) {
      request_update = true;
      if (input == 'u' && !videos.empty()) {
        invidious::subs::write_subs(videos[selected].channel_url);
        last_action = "deleted " + videos[selected].channel_name;
        return;
      }

      c_generic_tab::handle_input(input);
    }
  };
}

#endif //TUITUBE_SUBSCRIPTIONS_H
