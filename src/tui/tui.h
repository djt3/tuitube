#ifndef TUITUBE_TUI_H
#define TUITUBE_TUI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>

#include "terminal.h"
#include "tabs/subscriptions.h"
#include "tabs/search.h"
#include "tabs/popular.h"
#include "tabs/generic_tab.h"
#include "tabs/settings.h"

namespace tui {
  static bool exit = false;

  namespace {
    static bool input_lock = false; // play is called in input loop, prevent refresh to show buffering
    static bool force_update = true;

    static int current_tab_idx = 0;
    static auto* subs_tab = new tabs::c_subscriptions_tab();
    static auto* popular_tab = new tabs::c_popular_tab();
    static auto* search_tab = new tabs::c_search_tab();
    static auto* settings_tab = new tabs::c_settings_tab();
    static tabs::c_generic_tab* current_tab = subs_tab;

    void input_loop() {
      static struct termios term_old = {0};
      if (tcgetattr(0, &term_old) < 0)
        perror("tcsetattr()");
      term_old.c_lflag &= ~ICANON;
      term_old.c_lflag &= ~ECHO;
      term_old.c_cc[VMIN] = 1;
      term_old.c_cc[VTIME] = 0;
      if (tcsetattr(0, TCSANOW, &term_old) < 0)
        perror("tcsetattr ICANON");

      while (!exit) {
        char input;
        read(0, &input, 1);

        if (input == 'q')
          exit = true;
        else if (input == 9 || input == '/') { // tab
          if (input == '/') {
            current_tab_idx = 2;
            current_tab = search_tab;
          } else {
            current_tab_idx = (current_tab_idx + 1) % 4;

            switch (current_tab_idx) {
            case 0:
              current_tab = subs_tab;
              break;
            case 1:
              current_tab = popular_tab;
              break;
            case 2:
              current_tab = search_tab;
              break;
            case 3:
              current_tab = settings_tab;
              break;
            }
          }

          terminal::clear(true);
          force_update = true;
          current_tab->force_update = true;
          current_tab->request_update = true;
        }
        else
          current_tab->handle_input(input);
      }

      term_old.c_lflag |= ICANON;
      term_old.c_lflag |= ECHO;
      if (tcsetattr(0, TCSADRAIN, &term_old) < 0)
        perror("tcsetattr ~ICANON");
    }
  }

  void run() {
    // old width and height used to check if a redraw is required
    int old_terminal_width = terminal::get_terminal_width();
    int old_terminal_height = terminal::get_terminal_height();

    std::thread input_thread(tui::input_loop);
    input_thread.detach();

    terminal::clear(true);

    while (!exit) {
      if(input_lock)
        continue;

      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      if (force_update)
        force_update = false;
      else if (!current_tab->is_update_required()) {
        int terminal_width = terminal::get_terminal_width();
        int terminal_height = terminal::get_terminal_height();

        if (terminal_width != old_terminal_width)
          old_terminal_width = terminal_width;
        else if (terminal_height != old_terminal_height)
          old_terminal_height = terminal_height;

        else
          continue;
      }

      current_tab->draw(old_terminal_width, old_terminal_height);

      fflush(stdout);
    }


    static struct termios term = {0};
    if (tcgetattr(0, &term) < 0)
      perror("tcsetattr()");

    term.c_lflag |= ICANON;
    term.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &term) < 0)
      perror("tcsetattr ~ICANON");
  }
}

#endif //TUITUBE_TUI_H
