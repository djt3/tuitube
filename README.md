# tuitube
minimal tui youtube (invidious) frontend made in c++

Very much still a work in progress. Build with cmake. Dependancies:
- mpv (can be changed in config.h)
- curlpp

What's working:
- Playing videos
- Searching for videos
- Subscriptions page
- Adding subscriptions from search
- Removing subscriptions
- Changing invidious instance (config.h, needs recompile)

What's planned:
- Popular / trending page
- Include channel / playlist / mix results in search
- Browsing channels
- Suggested videos

Known bugs:
- None, please report any you find :)

How to add subscriptions:
The file is at "~/.config/tuitube_subs", add as "{channel id / name}" from the channel url, with one per line. To use, run the binary file after building and press "r" to refresh the subscriptions.

If your mpv can't play web links, check the comments in config.h (note, requires youtube-dl).

![Screenshot](https://github.com/djt3/tuitube/blob/master/tuitube.png?raw=true)
