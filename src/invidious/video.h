//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_VIDEO_H
#define TUITUBE_VIDEO_H

namespace invidious {
    class c_video {
    public:
        std::string title;
        std::string channel;
        std::string channel_url;
        std::string url;
        uint64_t time;
    };
}

#endif //TUITUBE_VIDEO_H
