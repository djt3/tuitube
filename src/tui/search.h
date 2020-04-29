//
// Created by dan on 29/04/2020.
//

#ifndef TUITUBE_SEARCH_H
#define TUITUBE_SEARCH_H

namespace search {
    namespace {
        static bool searched = false;
        static std::string search_text = "";
        static bool awaiting_refresh = false;
        static int selected = 0;
        static int scroll = 0;
        static std::vector<invidious::c_video> videos;

        static void do_search() {
            videos.clear();

            awaiting_refresh = true;

            std::string search_url = "/search?q=" + search_text;
            std::replace(search_url.begin(), search_url.end(), ' ', '+');

            videos = requests::extract_videos(search_url);

            awaiting_refresh = false;
        }
    }

    static void draw(const int& width, const int& height) {
        if (awaiting_refresh)
            printf("loading...");
        else if (!searched) {
            int i;
            for (i = 0; i < height / 2; i++)
                printf("\n");

            std::string text = "          [";
            text += search_text;
            //text.append("", width - 20 - search_text.size());
            for (int j = 0; j < width - 20 - search_text.size(); j++)
                text += " ";
            text += "]";
            printf(text.c_str());

            for (; i < height; i++)
                printf("\n");
        } else {
            while (selected > height + scroll - 2)
                scroll++;
            while (selected < scroll)
                scroll--;

            int max = std::min(height + scroll, static_cast<int>(videos.size()));
            for (int i = scroll; i < max; i++) {
                const auto &video = videos[i];

                if (i == selected) {
                    terminal::set_background_color(terminal::e_color::white);
                    terminal::set_text_color(terminal::e_color::black);
                }

                std::string text = video.channel + " - " + video.title;
                if (text.size() > width)
                    text = text.substr(0, width - 3) + "...";

                printf(text.c_str());
                if (i != max - 1)
                    printf("\n");

                if (i == selected)
                    terminal::reset();
            }
        }

        terminal::set_background_color(terminal::e_color::white);
        terminal::set_text_color(terminal::e_color::black);

        std::string binds;
        if (searched)
            binds = "[tab] subscriptions [q] quit [s] show searchbox";
        else
            binds = "[tab] subscriptions [enter] search";

        if (binds.size() > width)
            binds = binds.substr(0, width - 3) + "...";

        printf("\n");
        printf(binds.c_str());
        for (int i = 0; i < width - binds.length(); i++)
            printf(" ");

        terminal::reset();
    }

    // returns true if a refresh is required TODO: arrow keys for input
    static bool handle_input(const char& input) {
        if (searched) {
            if (input == 10 && !videos.empty()) { // enter
                std::string cmd = "mpv \"" + requests::extract_video_link(videos[selected]) + "\"";
                system(cmd.c_str());
                return true;
            }
            if (input == 's') {
                awaiting_refresh = false;
                searched = false;
                search_text = ""; // maybe dont reset this
                scroll = 0;
                return true;
            } else if (input == 65) { // up
                if (selected > 0)
                    selected--;
                return true;
            } else if (input == 66) { // down
                if (selected < videos.size() - 1)
                    selected++;
                return true;
            }
        } else {
            if (input == 10 && !search_text.empty()) { // TODO: same as subs refresh
                searched = true;
                awaiting_refresh = true;
                do_search();
            } else if (input == 127) {
                if (search_text.size() > 0)
                    search_text = search_text.substr(0, search_text.size() - 1);
            } else
                search_text += input;
            return true;
        }

        return false;
    }
}

#endif //TUITUBE_SEARCH_H
