//
// Created by dan on 28/04/2020.
//

#ifndef TUITUBE_REQUESTS_H
#define TUITUBE_REQUESTS_H

#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>

#include "invidious/video.h"
#include "config.h"

namespace requests {
    namespace  {
        std::string make_request(const std::string& url) {
            curlpp::Cleanup myCleanup;
            std::stringstream result;

            curlpp::Easy request;
            request.setOpt(cURLpp::Options::WriteStream(&result));
            request.setOpt<curlpp::options::Url>(url);
            request.perform();

            return result.str();
        }
    }

    std::vector<invidious::c_video> extract_videos(const std::string& url,
            const std::string& override_channel_url = "") {
        std::string full_url = config::invidious_instance + url;
        std::string response = make_request(full_url);

        std::vector<invidious::c_video> videos {};

        std::size_t index = 1;
        while (true) {
            index = response.find("<p class=\"length\">");
            if (index == std::string::npos)
                break;

            response = response.substr(index + 18);

            if(response.find(':') > 5)
                continue;

            invidious::c_video video;
            video.length = response.substr(0, response.find("</p>"));

            index = response.find("<p><a href=");

            response = response.substr(index + 12);
            index = response.find('\"');
            video.url = response.substr(0, index);

            response = response.substr(index + 2);
            index = response.find('<');
            video.title = response.substr(0, index);
            video.cleanup_title();

            if (override_channel_url == "") {
                index = response.find("channel/");
                response = response.substr(index + 8);
                index = response.find('\"');
                video.channel_url = response.substr(0, index);
            } else {
                video.channel_url = override_channel_url;
            }

            index = response.find("\">");
            response = response.substr(index + 2);
            index = response.find('<');
            video.channel_name = response.substr(0, index);

            // calculate time
            index = response.find("<div class=");
            response = response.substr(index + 31);
            video.time_str = response.substr(0, response.find("</div>"));

            size_t space_index = response.find(' ');

            uint mult = 1;
            if (response[space_index + 1] == 's') // seconds
                mult = 1;
            else if (response[space_index + 2] == 'i') // minutes
                mult = 60;
            else if (response[space_index + 1] == 'h') // hours
                mult = 3600;
            else if (response[space_index + 1] == 'd') // days
                mult = 86400;
            else if (response[space_index + 1] == 'w') // weeks
                mult = 604800;
            else if (response[space_index + 2] == 'o') // months
                mult = 18144000;
            else if (response[space_index + 1] == 'y') // years
                mult = 217728000;

            try { // can fail if there is a playlist / mix / channel TODO: return a separate vector if wanted
                video.time = std::stoi(response.substr(0, space_index)) * mult;
            }
            catch (...) {
                continue;
            }

            videos.push_back(video);
        }

        return videos;
    }

    std::string extract_video_link(const invidious::c_video& video) {
        std::string full_url = config::invidious_instance + video.url;
        std::string response = make_request(full_url);

        auto index = response.find("<source");
        if (index == std::string::npos)
            return "";

        response = response.substr(index);
        index = response.find('\"');
        response = response.substr(index + 1);
        index = response.find('\"');
        response = response.substr(0, index);

        return "https://invidious.snopyta.org" + response;
    }
}

#endif //TUITUBE_REQUESTS_H
