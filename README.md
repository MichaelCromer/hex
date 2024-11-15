# HEX Alpha Version

A command-line hexworld/crawl builder with a TUI.

## Build

Download the repo and run

    make

from the git root. Will create a `./build/` subdirectory with build objects. The
executable is `./build/hex` for you to use as you see fit.

## Features

[X] navigate tiles
[X] paint terrain
[X] simple colours and ui

### Roadmap to 1.0

Definite targets

[ ] save/load map files
[ ] 'command-line' interaction mode
[ ] consistent UI/UX
[ ] paint roads and rivers w/appropriate interaction mode
[ ] expanded commands and options
[ ] full/adaptive colour

### Midterm Goals

Subject to change

[ ] minimap overlay option
[ ] deep map zoom (using logtree structure)
[ ] naming of map features / customisable map data
[ ] searchable locations
[ ] single tile focus
[ ] set/goto map marks

### Longterm Goals

Completely uncertain

[ ] `hex --crawl` option for 'player' mode
[ ] location zoom. dungeon/settlement layout
[ ] other fantasy ttrpg elements: items, monsters, npcs?
[ ] calendar, seasons
