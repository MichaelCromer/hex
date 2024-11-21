# HEX Version α-0.1

A command-line hexworld/crawl builder with a TUI and modal editing.

## Build

Download the repo and run

    make

from the git root. Will create a `./build/` subdirectory with build objects. The
executable is `./build/hex` for you to use as you see fit.

## Features

### DONE

[X] navigate tiles
[X] paint terrain
[X] simple colours and ui

### IN PROGRESS

[ ] 'command-line' interaction mode (for meta controls; quit/write/edit/help/etc)
[ ] save/load map files

### Roadmap to 1.0

Definite targets

[ ] consistent UI/UX
    - the problem with Panel -- when to use, and where to place them?
    - what data to show all the time in statusline?
    - what should 'j' and 'J' do in various modes?
[ ] paint roads and rivers w/appropriate interaction mode
    - along hex edges for rivers
    - between hex centres/through hex edges for roads
[ ] expanded commands and options
    - '?' for contextual help/commands, :h[elp] for more in-depth help viewer
    - repeat/undo last action
    - place 'location'

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
[ ] location zoom. dungeon/settlement layout painting
[ ] other fantasy ttrpg elements: items, monsters, npcs?
[ ] calendar, seasons
