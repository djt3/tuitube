# tuitube
A minimal tui youtube (invidious) frontend made in c++

## Dependancies:
- mpv (can be changed in config.h)
- curlpp

## What's working:
- Playing videos
- Searching for videos
- Subscriptions page
- Adding subscriptions from search
- Removing subscriptions
- Changing invidious instance (config.h, needs recompile)
- Viewing channels

## What's planned:
- Include channel / playlist / mix results in search
- Suggested videos
- Auto invidious instance selector
- Removing all calls to system("clear") for smoother navigation

## Known bugs:
- Video duration is messed up on the search page, will be fixed shortly

## Installing
Kiss linux:

`kiss b tuitube && kiss i tuitube`

Arch based (arch, manjaro, artix):

`{aur helper} -S tuitube`

Other:

Install dependancies and

`git clone https://github.com/djt3/tuitube && cd tuitube`

`cmake . && make && make install`

## How to change subscriptions manually:
The file is at "~/.config/tuitube_subs", add as "{channel id / name}" from the channel url, with one per line.

If your mpv can't play web links, check the comments in config.h (note, requires youtube-dl).

![Screenshot](https://github.com/djt3/tuitube/blob/master/tuitube.png?raw=true)
