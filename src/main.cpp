#include "tui/tui.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    signal(SIGINT, [](int sig){terminal::clear(true); terminal::show_cursor(); tui::exit = true;});

    std::filesystem::create_directory(std::string(getenv("HOME")) + "/.config/tuitube");
    system("mv ~/.config/tuitube_subs ~/.config/tuitube/subs > /dev/null 2>&1");

    terminal::hide_cursor();
    config::init();
    tui::run();
    config::write_config();
    terminal::show_cursor();

    return 0;
}
