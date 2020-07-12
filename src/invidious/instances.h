#ifndef TUITUBE_INSTANCES_H
#define TUITUBE_INSTANCES_H
#include <thread>
#include <vector>
#include <sstream>

#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>

namespace invidious::instances {
  namespace {
    std::string fastest_instance = "";
  }

  static std::vector<std::string> instances = {"invidious.snopyta.org", "invidio.us", "invidious.13ad.de"};

  static std::string get_fastest_instance() {
    if (!fastest_instance.empty()) {
      return fastest_instance;
    }

    std::vector<std::thread> threads;

    // there has to be a better way to do this

    auto make_request = [](const std::string& url) {
                          curlpp::Cleanup myCleanup;
                          std::stringstream result;

                          curlpp::Easy request;
                          request.setOpt(cURLpp::Options::WriteStream(&result));
                          request.setOpt<curlpp::options::Url>(url);
                          request.perform();
                        };

    std::string first_finished = "";
    for (const auto& instance : instances) {
      threads.push_back(std::thread([&]() {make_request("https://" + instance); first_finished = instance;}));
    }

    while (first_finished.empty())
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    fastest_instance = first_finished;

    for (auto& thread : threads)
      thread.detach();

    return fastest_instance;
  }
}

#endif
