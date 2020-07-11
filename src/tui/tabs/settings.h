#ifndef TUITUBE_SETTINGS_H
#define TUITUBE_SETTINGS_H

#include <fstream>

#include "../utils.h"
#include "channel_view.h"
#include "generic_tab.h"

namespace tui::tabs {
  class c_dropdown {
  public:
    std::string name;
    std::vector<std::string> options;

    c_dropdown(const std::string& name, const std::vector<std::string>& options) :
      name(name), options(options) {}

    std::pair<std::string, std::string> to_string() {
      return std::make_pair(name, config::get_value(name));
    }

    void set_value(int selected_value) {
      config::config[name] = options[selected_value];
    }
  };

  class c_settings_tab : public c_generic_tab {
  private:
    bool editing_value = false;
    int editing_selected = 0;
    std::vector<c_dropdown> options;
  public:
    c_settings_tab() {
      options.push_back(c_dropdown("Invidious Instance", {"invidious.snopyta.org", "invidio.us"}));
      options.push_back(c_dropdown("Video Source", {"Invidious", "YouTube"}));
    }

    void refresh_videos() { }

    void draw(const int& width, const int& height) {
      while (selected > height + scroll - 3)
        scroll++;
      while (selected < scroll)
        scroll--;


      if (!editing_value) {
        std::vector<std::pair<std::string, std::string>> strings;
        for (auto& option : options) {
          strings.push_back(option.to_string());
        }

        tui::utils::print_title("settings", width);
        tui::utils::print_generic(selected, width, height, scroll);
        tui::utils::print_strings(strings, scroll);
        tui::utils::print_footer("[tab] change tab [enter] edit value", width, force_update);
      }

      else {
        std::vector<std::pair<std::string, std::string>> strings;
        for (auto& option : options[selected].options) {
          strings.push_back(std::make_pair(option, ""));
        }

        tui::utils::print_title("settings - " + options[selected].name, width);
        tui::utils::print_generic(editing_selected, width, height, 0);
        tui::utils::print_strings(strings, 0);
        tui::utils::print_footer("[tab] change tab [enter] set value", width, force_update);
      }
    }

    void handle_input(const char& input) {
      request_update = true;

      if (input == 10) { // enter
        if (editing_value) {
          options[selected].set_value(editing_selected);
        }

        editing_value = !editing_value;
        editing_selected = 0;
      } else if (input == 65 || input == 'k') { // up
        if (!editing_value && selected > 0)
          selected--;
        else if (editing_value && editing_selected > 0)
          editing_selected--;
        else
          request_update = false;
      } else if (input == 66 || input == 'j') { // down
        if (!editing_value && selected < options.size() - 1)
          selected++;
        else if (editing_value && editing_selected < options[selected].options.size() - 1)
          editing_selected++;
        else
          request_update = false;
      } else {
        request_update = false;
      }
    }
  };
}

#endif //TUITUBE_SEARCH_H
