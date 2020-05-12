//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_TERMINAL_H
#define TUITUBE_TERMINAL_H

#include <iostream>
#include <stdio.h>
#include <thread>

namespace terminal {
    enum class e_color : int {
        black = 30,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white
    };

    static inline void move_cursor(int x, int y) {
      printf("\033[%d;%dH", y, x);
    }

    static inline void clear(bool full_clear = false) {
        fflush(stdout);
        if (full_clear)
            system("clear");
        else
            move_cursor(0, 0);
        fflush(stdout);
    }

    static inline winsize get_terminal_size() {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size;
    }

    static inline int get_terminal_width() {
        return static_cast<int>(get_terminal_size().ws_col);
    }

    static inline int get_terminal_height() {
        return static_cast<int>(get_terminal_size().ws_row);
    }

    static inline void set_text_color(e_color color) {
        printf("\u001b[%d;1m", color);
    }

    static inline void set_background_color(e_color color) {
        printf("\u001b[%dm", static_cast<int>(color) + 10);
    }

    static inline void reset() {
        printf("\u001b[0m");
    }

    static inline void show_cursor() {
        system("setterm -cursor on");
    }

    static inline void hide_cursor() {
        system("setterm -cursor off");
    }
}

#endif //TUITUBE_TERMINAL_H
