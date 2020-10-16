#ifndef TUITUBE_SEARCH_H
#define TUITUBE_SEARCH_H

#include <fstream>

#include "../utils.h"
#include "channel_view.h"
#include "generic_tab.h"

namespace tui::tabs {
  class c_search_tab : public c_generic_tab {
  private:
    bool searched = false;
    std::string search_text = "";
  public:

    c_search_tab() {
      title = "search";
      custom_actions.push_back(action("back", 'b', nullptr));
      generate_footer();
    }

    void refresh_videos() {
      last_action = "searching...";
      std::string search_url = "/search?q=" + search_text;
      std::replace(search_url.begin(), search_url.end(), ' ', '+');
      videos = requests::extract_videos(search_url);
      c_generic_tab::refresh_videos();
      last_action = search_text;
    }

    void draw(const int& width, const int& height) {
      // draw the search box
      if (!searched) {
        tui::utils::print_title("search", width , last_action);
        tui::utils::print_generic(0, width, height, scroll);
        tui::utils::print_footer("[tab] change tab [enter] search", width, force_update);

        int i;
        terminal::move_cursor(0, height / 2);
        std::string text = "          [";
        text += search_text;
        //text.append("", width - 20 - search_text.size());
        for (int j = 0; j < width - 20 - search_text.size(); j++)
          text += " ";
        text += "]";
        printf("%s", text.c_str());
      } else {
        c_generic_tab::draw(width, height);
      }
    }

    // TODO: arrow keys for input
    void handle_input(const char& input) {
      request_update = true;

      if (searched) {
        if (input == 'b') { // b - back
          searched = false;
          videos.clear();
          terminal::clear(true);
          search_text = ""; // maybe don't reset this
          last_action = "";
          scroll = 0;
        }
        c_generic_tab::handle_input(input);
      }
      else {
        if (input == 10 && !search_text.empty()) { // enter - search
          searched = true;
          terminal::clear(true);
          force_update = true;
          std::thread search_thread(&c_search_tab::refresh_videos, this);
          search_thread.detach();
          force_update = true;
        } else if (input == 127) { // backspace - delete character
          if (search_text.size() > 0)
            search_text = search_text.substr(0, search_text.size() - 1);
        } else if ((input >= 48 && input <= 57) || (input >= 65 && input <= 90) || (input >= 97 && input <= 122) || input == ' ')
          search_text += input;
      }
    }
  };
}

#endif //TUITUBE_SEARCH_H
