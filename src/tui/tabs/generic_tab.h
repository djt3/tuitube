#ifndef TUITUBE_GENERIC_TAB_H
#define TUITUBE_GENERIC_TAB_H

#include "channel_view.h"
#include <vector>

namespace tui::tabs {
  struct action {
    action(std::string name, char key, void(*fn)()) : name(name), key(key), fn(fn) {
    }

    std::string name;
    char key;
    void (*fn)();
  };


  class c_generic_tab {
    public:
    bool request_update = false;
    bool force_update = true;
    bool view_channel = false;
    bool init = false;
    int selected = 0;
    int scroll = 0;
    std::vector<invidious::c_video> videos;
    std::string last_action = "";
    std::string title;
    std::string footer = "[tab] change tab [q] quit [enter] play [a] audio only [c] view channel";
    c_channel_view channel_view;

    bool can_subscribe = true;
    bool can_refresh = true;
    std::vector<action> custom_actions;

    void generate_footer() {
      if (can_subscribe)
        footer += " [s] subscribe";
      if (can_refresh)
        footer += " [r] refresh";

      if (custom_actions.empty())
        return;

      for (auto& action : custom_actions) {
        footer += " [";
        footer.push_back(action.key);
        footer += "] " + action.name;
      }
    }

  public:
    bool is_update_required() {
      if(view_channel)
        return channel_view.request_update;

      if (request_update) {
        request_update = false;
        return true;
      }
      return false;
    }

    virtual void refresh_videos() {
      request_update = true;
      last_action = "";
    }

    virtual void draw(const int &width, const int &height) {
      if (!init) {
        init = true;
        std::thread refresh_thread(&c_generic_tab::refresh_videos, this);
        refresh_thread.detach();
      }

      tui::utils::print_title(title, width, last_action);

      if (view_channel) {
        channel_view.draw(width, height);
        return;
      }

      while (selected > height + scroll - 3)
        scroll++;
      while (selected < scroll)
        scroll--;

      tui::utils::print_videos(videos, selected, width, height, scroll);

      tui::utils::print_footer(footer, width, force_update);
      force_update = false;
    }

    virtual void handle_input(const char &input) {
      request_update = true;

      if (input == 's' && can_subscribe && !videos.empty()) { // a - subscribe
        invidious::subs::add_sub(videos[selected]);
        last_action = "subscribed to " + videos[selected].channel_name;
        return;
      }

      if (view_channel) {
        if (input == 'b')
          view_channel = false;
        else
          channel_view.handle_input(input);

        return;
      }

      if (input == 10 || input == 'a' && !videos.empty()) { // enter - open video
        request_update = false;
        terminal::clear();

        last_action = "played " + videos[selected].title;
        request_update = true;
        if (input == 10)
          utils::play_video(videos[selected]);
        else
          utils::play_audio(videos[selected]);
        request_update = true;
        force_update = true;
      } else if (input == 'r' && can_refresh && last_action != "refreshing...") { // r - refresh
        std::thread refresh_thread(&c_generic_tab::refresh_videos, this);
        refresh_thread.detach();
        last_action = "";
      } else if (input == 'c' && !videos.empty()) { // c - view channel
        view_channel = true;
        last_action = "view channel " + videos[selected].channel_name;
        channel_view = c_channel_view(videos[selected]);
        std::thread refresh_thread([&]{channel_view.refresh_videos();});
        refresh_thread.detach();
      } else if (input == 65 || input == 'k') { // up
        if (selected > 0)
          selected--;
      } else if (input == 66 || input == 'j') { // down
        if (selected < videos.size() - 1)
          selected++;
      } else {
        for (auto& action : custom_actions) {
          if (input == action.key) {
            if (action.fn)
              action.fn();
            return;
          }
        }

        request_update = false;
      }
    }
  };
}

#endif
