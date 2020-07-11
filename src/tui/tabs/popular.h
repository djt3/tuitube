#ifndef TUITUBE_POPULAR_H
#define TUITUBE_POPULAR_H

#include <fstream>

#include "../utils.h"
#include "channel_view.h"
#include "generic_tab.h"

namespace tui::tabs {
  class c_popular_tab : public c_generic_tab {
  public:
    c_popular_tab() {
      title = "popular";
      generate_footer();
    }

    void refresh_videos() {
      last_action = "refreshing...";
      request_update = true;
      videos = requests::extract_videos("/", "", true);
      c_generic_tab::refresh_videos();
    }

    void draw(const int& width, const int& height) {
        c_generic_tab::draw(width, height);
    }

    // TODO: arrow keys for input
    void handle_input(const char& input) {
        c_generic_tab::handle_input(input);
    }
  };
}

#endif //TUITUBE_POPULAR_H
