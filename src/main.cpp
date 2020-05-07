#include "tui/tui.h"
#include "requests.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    signal(SIGINT, [](int sig){terminal::clear(true); terminal::show_cursor(); tui::exit = true;});

    terminal::hide_cursor();
    tui::run();
    terminal::show_cursor();

    return 0;
}
