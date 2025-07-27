#include <limits.h>
#include <ncurses.h>
#include <stdio.h>

#include "hdr/action.h"
#include "hdr/draw.h"
#include "hdr/state.h"


struct State *state = NULL;


void initialise(const char *filename)
{
    initscr();                  /* init the lib                                     */
    cbreak();                   /* send by char (raw ignores ssigs! use cbreak)     */
    noecho();                   /* don't echo user input                            */
    keypad(stdscr, TRUE);       /* capture special keys                             */
    intrflush(stdscr, FALSE);   /* interpret screen changes from ssigs correctly    */
    curs_set(0);                /* disable cursor                                   */
    ESCDELAY = 10;

    colour_initialise();
    state_initialise(stdscr, filename);
}


void terminate(void)
{
    erase();
    endwin();

    state_deinitialise();
}


int main(int argc, char *argv[])
{
    if (argc > 2) {
        fprintf(stderr, "\nToo many arguments\n");
        return 1;
    }

    initialise((argc == 2) ? argv[1] : NULL);

    while (!state_quit()) {
        erase();
        draw_state();
        refresh();

        state_update();
    }

    terminate();
    return 0;
}
