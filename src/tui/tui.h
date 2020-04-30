//
// Created by dan on 28/04/2020.
//

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
#include "subscriptions.h"
#include "search.h"

namespace tui {
    namespace {
        static bool exit = false;
        static bool search = false;
        static bool input_lock = false; // play is called in input loop, prevent refresh to show buffering
        static bool force_update = false;

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

                if (!search && input == 'q')
                    exit = true;
                else if (input == 9) { // tab
                    search = !search;
                    force_update = true;
                }
                else if (!search)
                    subscriptions::handle_input(input);
                else
                    search::handle_input(input);
            }

            term_old.c_lflag |= ICANON;
            term_old.c_lflag |= ECHO;
            if (tcsetattr(0, TCSADRAIN, &term_old) < 0)
                perror("tcsetattr ~ICANON");
        }
    }

    void run() {
        // old width and height used to check if a redraw is required
        int old_terminal_width = -1;
        int old_terminal_height = -1;

        std::thread input_thread(tui::input_loop);
        input_thread.detach();

        while (!exit) {
            if(input_lock)
                continue;

            bool redraw_required = false;

            int terminal_width = terminal::get_terminal_width();
            int terminal_height = terminal::get_terminal_height();

            if (force_update)
                force_update = false;
            else if(search && search::is_update_required());
            else if(!search && subscriptions::is_update_required());
            else if (terminal_width != old_terminal_width) {
                old_terminal_width = terminal_width;
                redraw_required = true;
            } else if (terminal_height != old_terminal_height) {
                old_terminal_height = terminal_height;
                redraw_required = true;
            }
            else
                continue;

            terminal::clear();

            if (!search)
                subscriptions::draw(terminal_width, terminal_height - 1);
            else
                search::draw(terminal_width, terminal_height - 1);

            fflush(stdout);
        }
    }
}

#endif //TUITUBE_TUI_H
