//
// Created by dan on 30/04/2020.
//

#ifndef TUITUBE_CONFIG_H
#define TUITUBE_CONFIG_H

namespace config{
    const static std::string invidious_instance = "https://invidio.us";

    // most systems are fine calling mpv {url} but yt-dl for max compatibility
    const static std::string playcmd_start = "youtube-dl -o - \"";
    // url is placed in middle
    const static std::string playcmd_end = "\" | mpv -";
}

#endif //TUITUBE_CONFIG_H
