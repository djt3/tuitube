//
// Created by dan on 29/04/2020.
//

#ifndef TUITUBE_UTILS_H
#define TUITUBE_UTILS_H

#include "../invidious/video.h"

namespace tui::utils {
  namespace {
    static int width = 0;
    static int last_width = -1;
    static int selected = 0;
    static int last_selected = -1;
    static std::vector<std::string> draw_queue = {""};
    static std::vector<std::string> last_draw_queue = {};
  }

  static void print_draw_queue(bool force_update) {
    for (int i = 0; i < draw_queue.size(); i++) {
      if (force_update || width != last_width || draw_queue[i] != last_draw_queue[i] || ((selected + 1 == i ||  last_selected + 1 == i) && selected != last_selected)) {

        if (i == 0 || i == draw_queue.size() - 1 || i - 1 == selected) {
          terminal::set_background_color(terminal::e_color::white);
          terminal::set_text_color(terminal::e_color::black);
        }

        terminal::move_cursor(0, i + 1);
        if (!draw_queue[i].empty()) {
          printf("%s", draw_queue[i].c_str());

          for (int j = 0; j < width - draw_queue[i].size(); j++)
            printf(" ");
          if (i == 0 || i == draw_queue.size() - 1 || i - 1 == selected)
            terminal::reset();
        }
        else {
          terminal::reset();
          for (int j = 0; j < width - draw_queue[i].size(); j++)
            printf(" ");
        }

        //if (1 != draw_queue.size() - 1)
        //printf("\n");
    }
    }

    last_draw_queue = draw_queue;
  }

  static void print_title(const std::string& title, int new_width, const std::string& extra = "") {
    last_width = width;
    width = new_width;
    std::string formatted_title = title;
    draw_queue.clear();

    if (!extra.empty())
      formatted_title = formatted_title + " - " + extra; // += doesn't work?

    if (formatted_title.size() > width)
      formatted_title = formatted_title.substr(0, width - 3) + "...";

    draw_queue.push_back(formatted_title);

  }

  static void print_videos(const std::vector<invidious::c_video>& videos,
                           int new_selected, int width, int height, int scroll) {
    draw_queue.resize(height);

    if (videos.empty())
      return;

    int max = std::min(height + scroll - 0, static_cast<int>(videos.size()));

    last_selected = selected;
    selected = new_selected - scroll;

    for (int i = scroll; i < max; i++) {
      const auto &video = videos[i];

      std::string text = video.channel_name + " - " + video.title + " - " + video.time_str + " - " + video.length;

      if (text.size() > width)
        text = text.substr(0, width - 3) + "...";

      draw_queue[i - scroll + 1] = text;
    }
  }

  static void print_footer(const std::string& binds, int width, bool force_update) {
    static bool footer_in_queue = false;

    std::string str;
    if (binds.size() > width)
      str = binds.substr(0, width - 3) + "...";
    else
      str = binds;

    draw_queue.back() = str;

    print_draw_queue(force_update);
  }

  static void play_video(const invidious::c_video& video) {
    std::string cmd = config::playcmd_start
      + requests::extract_video_link(video)
      + config::playcmd_end;

    system(cmd.c_str());
  }
}

#endif //TUITUBE_UTILS_H
