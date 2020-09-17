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
    static std::string fastest_instance = "";
  }

  static std::vector<std::string> instances = {"invidious.snopyta.org"};

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


    auto send_request = [&](const std::string& instance) {
                          make_request("https://" + instance + "/search?q=test");
                          if (fastest_instance.empty())
                            fastest_instance = instance;
                        };

    for (const auto& instance : instances) {
      threads.push_back(std::thread([&]() {send_request(instance);}));
    }

    while (fastest_instance.empty())
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (auto& thread : threads)
      thread.detach();

    return fastest_instance;
  }
}

#endif
