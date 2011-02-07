#include <stdlib.h>
#include <stdio.h>
#include <nds.h>

#include "dsdual/dd2D.h"

static inline void ddVertex3i(v16 x, v16 y, v16 z) {
    GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
    GFX_VERTEX16 = ((uint32)(uint16)z);
}

static inline void ddVertex2i(v16 x, v16 y) {
    GFX_VERTEX_XY = (y << 16) | (x & 0xFFFF);
}

static inline void ddTexcoord2i(t16 u, t16 v) {
    GFX_TEX_COORD = (v << 20) | ( (u << 4) & 0xFFFF );
}

static inline void ddScalef32(s32 x, s32 y, s32 z) {
    MATRIX_SCALE = x;
    MATRIX_SCALE = y;
    MATRIX_SCALE = z;
}

static inline void ddTranslate3f32(int32 x, int32 y, int32 z) {
    MATRIX_TRANSLATE = x;
    MATRIX_TRANSLATE = y;
    MATRIX_TRANSLATE = z;
}

inline void SetOrtho() {
    glMatrixMode(GL_PROJECTION);    // set matrixmode to projection
    glLoadIdentity();               // reset
    glOrthof32(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1024, 1<<12);    // downscale projection matrix
}

void ddBegin2D() {
    // save 3d perpective projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    // save 3d modelview matrix for safety
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_ANTIALIAS);    // disable AA
    glDisable(GL_OUTLINE);      // disable edge-marking

    glColor(0x7FFF);                 // max color

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);  // no culling

    SetOrtho();

    glMatrixMode(GL_TEXTURE);          // reset texture matrix just in case we did some funky stuff with it
    glLoadIdentity();

    glMatrixMode( GL_MODELVIEW );        // reset modelview matrix. No need to scale up by << 12
    glLoadIdentity();
}

void ddEnd2D() {
    // restore 3d matrices and set current matrix to modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(1);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix(1);
}

// textured quad, you must bind the texture before
inline void ddTexQuad2D(u16 u, u16 v, u8 w, u8 h, s16 x, s16 y, s16 z) {
    glBegin(GL_QUADS);
        ddTexcoord2i(u, v);     ddVertex3i(x, y, z);    
        ddTexcoord2i(u, v+h);   ddVertex2i(x, y+h);
        ddTexcoord2i(u+w, v+h); ddVertex2i(x+w, y+h);
        ddTexcoord2i(u+w, v);   ddVertex2i(x+w, y);        
    glEnd();
}

