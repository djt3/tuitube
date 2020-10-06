# tuitube
A minimal tui youtube (invidious) frontend made in c++

https://gitlab.com/djt3/tuitube

## Dependencies:
- youtube-dl
- mpv (can be changed in config.h) (mpv MUST be built with "luajit" (default in arch))
- curlpp

## What's planned:
- Include channel / playlist / mix results in search
- Suggested videos

## Known bugs:
- None, please create an issue if you find any or have a feature suggestion

## Installing
Kiss linux:

`kiss b tuitube && kiss i tuitube`

Arch based (arch, manjaro, artix):

`{aur helper} -S tuitube-git`

Other:

Install dependencies and

`git clone https://gitlab.com/djt3/tuitube && cd tuitube`

`cmake . && sudo make install`

![Screenshot](https://github.com/djt3/tuitube/blob/master/tuitube.png?raw=true)
