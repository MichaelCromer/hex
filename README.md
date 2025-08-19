# HEX Version α-0.1

A command-line hexworld/crawl builder with a TUI and modal editing.

## Build

Download the repo and run

    make

from the git root. Will create a `./bdl/` subdirectory with build objects. The
executable is `./bdl/hex` for you to use as you see fit.

## Use

Run `hex` from the command line with or without a filename argument. If passed a file
name, `hex` will try to read it as hex save data and load it for editing. Otherwise,
you'll be opened up in a new empty world.

### Modes

`hex` is a modal editor, like `vim`. Different modes are for painting different kinds of
map features (terrain, roads, rivers, and locations). There is also a command mode for
saving and loading hex files. The info line at the bottom of the screen will change to
indicate what mode you are in.

Modes can be activated in two different ways: one is to 'lock' them on so that all
keypresses are interpreted in that mode until you change the mode again manually; the
other way is a 'soft' version that will await the next keypress only, then return to the
mode you were in previously. This is so you can make small edits as you notice them
without breaking flow. The former is always performed with an uppercase command letter,
the latter with the corresponding lowercase letter. The info line will indicate the
difference in mode type with square or round brackets.

For example, if you start a new `hex` session without specifying a file name (and
dismiss the welcome panel with `<Enter>`), the info line will show the text
```
    [NAVIGATE]
```
to show that you are in "navigate" mode, the default. If you then press `T` (that's
`<Shift-t>`, an uppercase `'t'`), you will select and lock-on "terrain" mode, the info
line will change to
```
    [TERRAIN]
```
and you will be able to paint terrain tiles. Then if you press `r`, you will enter
"roads" await-mode, and the info line will now look like
```
    [TERRAIN] (roads)
```
to show that one keypress will be interpreted in "roads" mode, and then you will be
taken back to terrain mode.

### Keygroups

If you have a standard qwerty keyboard, you will find that commands in `hex` are grouped
up so that different regions of the keyboard have different, consistent use cases: the
keys under the left hand are for mode-specific commands (such as `q` for painting a
water tile in terrain mode), while the keys under the right hand are for navigating
around the map. The keys in the middle of the keyboard are for changing input modes
(except `:` for entering command mode; `vim` users will recognise the inspiration).

#### Moving the cursor

Most modes allow you to navigate through the hex grid one tile at a time with `u`, `i`,
`h`, `l`, `n`, and `m` as directional keys (on a standard keyboard, they form a
hexagon):
```
       u   i
        \ /
     h -   - k
        / \
       n   m
```

Note that these are all lowercase characters. Modes will usually do something useful
when you use the uppercase versions, too. In navigate mode, uppercase motions move the
cursor by up to three tiles at a time, while in terrain mode they copy the terrain under
the cursor and drag it onto the neighbouring tile in that direction. In fact this is the
suggested method for painting terrain (since terrain "usually" appears in blocks of more
than one tile).

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
     - - [ ] repeat/undo last action

### Midterm Goals

Subject to change

 - [ ] undo tree traversal
 - [ ] minimap overlay
 - [ ] naming of map features / customisable map data
 - [ ] searchable locations
 - [ ] single tile focus deep zoom
 - [ ] set/goto map marks
 - [ ] :h[elp] for more in-depth help viewer

### Longterm Goals

Completely uncertain

 - [ ] `hex --crawl` option for 'player' mode
 - [ ] location zoom, dungeon/settlement layout painting
 - [ ] other fantasy ttrpg elements: items, monsters, npcs?
 - [ ] calendar, seasons
