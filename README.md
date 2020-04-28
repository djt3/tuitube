# tuitube
minimal tui youtube (invidious) frontend made in c++

Very much still a work in progress. Build with cmake. Dependancies:
- mpv
- libcurlpp

What's working:
- Subscriptions page
- Opening videos

What's planned:
- Search
- Browsing channels
- Suggested videos
- An easier way to add subscriptions

How to add subscriptions:
The file is at "~/.config/tuitube_subs", add as /channel/{channel id / name} one per line

![Screenshot](https://github.com/djt3/tuitube/blob/master/Screenshot_20200428_233430.png?raw=true)
