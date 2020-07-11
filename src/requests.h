#ifndef TUITUBE_REQUESTS_H
#define TUITUBE_REQUESTS_H

#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>

#include "invidious/video.h"
#include "config.h"

namespace requests {
  std::string make_request(const std::string& url) {
    curlpp::Cleanup myCleanup;
    std::stringstream result;

    curlpp::Easy request;
    request.setOpt(cURLpp::Options::WriteStream(&result));
    request.setOpt<curlpp::options::Url>(url);
    request.perform();

    return result.str();
  }

  std::vector<invidious::c_video> extract_videos(const std::string& url,
                                                 const std::string& override_channel_url = "",
                                                 bool is_popular_tab = false) {
    std::string full_url = "";
    if (!is_popular_tab)
      full_url = "https://" + config::get_value("Invidious Instance") + url;
    else
      full_url = "https://" + config::get_value("Instance For Popular Videos") + url;

    std::string response = make_request(full_url);

    std::vector<invidious::c_video> videos {};

    bool is_channel_page = response.find("channel-profile") != std::string::npos;
    std::string channel_page_name = "";

    if (is_channel_page) {
      auto index = response.find("channel-profile");
      std::string temp_response = response.substr(index);
      index = temp_response.find("<span>") + 6;
      temp_response = temp_response.substr(index);
      auto end_index = temp_response.find("</span>");
      channel_page_name = temp_response.substr(0, end_index);
    }

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

      if (override_channel_url == "") {
        index = response.find("channel/");
        response = response.substr(index + 8);
        index = response.find('\"');
        video.channel_url = response.substr(0, index);
      } else {
        video.channel_url = override_channel_url;
      }

      if (!is_channel_page) {
        index = response.find("\">");
        response = response.substr(index + 2);
        index = response.find('<');
        video.channel_name = response.substr(0, index);
      }
      else {
        video.channel_name = channel_page_name;
      }

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

      video.cleanup_text();
      videos.push_back(video);
    }

    return videos;
  }

  std::string extract_video_link(const std::string& url) {
    if (config::get_value("Video Source") == "Invidious") {
      std::string invidious_url = "https://" + config::get_value("Invidious Instance");
      std::string full_url = invidious_url + url;
      std::string response = make_request(full_url);

      auto index = response.find("<source");
      if (index == std::string::npos)
        return "";

      response = response.substr(index);
      index = response.find('\"');
      response = response.substr(index + 1);
      index = response.find('\"');
      response = response.substr(0, index);

      return invidious_url + response;
    } else {
      return "https://youtube.com" + url;
    }
  }
}

#endif //TUITUBE_REQUESTS_H
