#include <stdio.h>
#include "game.h"

int main(int argc, char **argv) {

    if(init()) {
        while(update()) {
            render();
        }
    }

    quit();

    return 0;

}