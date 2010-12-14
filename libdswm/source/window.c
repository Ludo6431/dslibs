#include <stdlib.h>
#include <stdio.h>
#include <nds.h>

#include <dswm/window.h>

gObject *new_window() {
    return malloc(sizeof(gObject));
}

