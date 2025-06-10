# HEX Version α-0.1

A command-line hexworld/crawl builder with a TUI and modal editing.

## Build

Download the repo and run

    make

from the git root. Will create a `./build/` subdirectory with build objects. The
executable is `./build/hex` for you to use as you see fit.

## Features

### DONE

 - [X] navigate tiles
 - [X] paint terrain, roads, rivers, and map features
 - [X] simple colours and ui
 - [X] commandline (`q[uit]`, `w[rite]`, `e[dit]`)
 - [X] contextual help/hint ui panels

### IN PROGRESS

 - [ ] full/adaptive colour
 - [ ] improved commandline usefulness (<C-h>, <C-w>, <C-u>, tab complete, etc)

### Roadmap to 1.0

Definite targets

 - [ ] consistent UI/UX
     - - [ ] 'j' _always_ toggles view of tile detail
     - - [ ] directional keys _always_ move the reticule
 - [ ] expanded commands and options
     - - [ ] :h[elp] for more in-depth help viewer
     - - [ ] repeat/undo last action

### Midterm Goals

Subject to change

 - [ ] minimap overlay option [looking doubtful if next point happens]
 - [ ] map layer changes using logtree structure for rapid traversal
 - [ ] naming of map features / customisable map data
 - [ ] searchable locations
 - [ ] single tile focus deep zoom
 - [ ] set/goto map marks

### Longterm Goals

Completely uncertain

 - [ ] `hex --crawl` option for 'player' mode
 - [ ] location zoom. dungeon/settlement layout painting
 - [ ] other fantasy ttrpg elements: items, monsters, npcs?
 - [ ] calendar, seasons
