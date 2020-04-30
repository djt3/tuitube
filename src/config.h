//
// Created by dan on 30/04/2020.
//

#ifndef TUITUBE_CONFIG_H
#define TUITUBE_CONFIG_H

namespace config {
    const static std::string invidious_instance = "https://invidio.us";

    // use these if your mpv cant play web videos
    // const static std::string playcmd_start = "youtube-dl -o - \"";
    // const static std::string playcmd_end = "\" | mpv -";

    const static std::string playcmd_start = "mpv \"";
    // url is placed in middle
    const static std::string playcmd_end = "\"";
}

#endif //TUITUBE_CONFIG_H
