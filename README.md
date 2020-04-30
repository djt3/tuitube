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
- Include channel / playlist / mix results in search
- Suggested videos
- Auto invidious instance selector

Known bugs:
- None, please report any you find :)

How to change subscriptions manually:
The file is at "~/.config/tuitube_subs", add as "{channel id / name}" from the channel url, with one per line.

If your mpv can't play web links, check the comments in config.h (note, requires youtube-dl).

![Screenshot](https://github.com/djt3/tuitube/blob/master/tuitube.png?raw=true)
