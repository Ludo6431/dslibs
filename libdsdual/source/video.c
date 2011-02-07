#include <stdlib.h>
#include <stdio.h>
#include <nds.h>

#include "dsdual/video.h"

void initSubSprites(void){
    oamInit(&oamSub, SpriteMapping_Bmp_2D_256, false);

    int x = 0;
    int y = 0;

    // set up a 4x3 grid of 64x64 sprites to cover the screen (graphics starts at SPRITE_GFX_SUB = 0x6600000)
    for(y=0; y<3; y++)
        for(x=0; x<4; x++) {
            u16 *offset = &SPRITE_GFX_SUB[(x * 64) + (y * 64 * 256)];

            oamSet(&oamSub, x+y*4, x*64, y*64, 0, 15, SpriteSize_64x64, SpriteColorFormat_Bmp, offset, -1, false, false, false, false, false);
        }

    swiWaitForVBlank();
    oamUpdate(&oamSub);
}

void ddVideoinit() {
    // init oam to capture 3D scene
    initSubSprites();

    // sub background holds the top image when 3D directed to bottom
    bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);   // will render 96kiB of u16 ARGB data from 0x06200000 to BG3

    // initialize gl
    glInit();

    // enable textures
    glEnable(GL_TEXTURE_2D);

    // enable antialiasing
    glEnable(GL_ANTIALIAS);

    // setup the rear plane
    glClearColor(0, 0, 0, 31); // BG must be opaque for AA to work
    glClearPolyID(63); // BG must have a unique polygon ID for AA to work

    glClearDepth(GL_MAX_DEPTH);

    // this should work the same as the normal gl call
    glViewport(0, 0, 255, 191);

    // any floating point gl call is being converted to fixed prior to being implemented
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0/192.0, 1, 200);

    gluLookAt(
        0.0, 0.0, 1.0,  // camera possition
        0.0, 0.0, 0.0,  // look at
        0.0, 1.0, 0.0   // up
    );

    glMaterialf(GL_AMBIENT, RGB15(31,31,31));
    glMaterialf(GL_DIFFUSE, RGB15(31,31,31));
    glMaterialf(GL_SPECULAR, BIT(15) | RGB15(31,31,31));
    glMaterialf(GL_EMISSION, RGB15(31,31,31));

    // ds uses a table for shinyness..this generates a half-ass one
    glMaterialShinyness();

    //not a real gl function and will likely change
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
}

inline bool ddSwitch() {
    static bool top = true;

    // wait for capture unit to be ready
//    while(REG_DISPCAPCNT & DCAP_ENABLE);
swiWaitForVBlank();
    //-------------------------------------------------------
    //     Switch render targets
    //-------------------------------------------------------
    top = !top;

    if(top) {
        lcdMainOnBottom();
        vramSetBankC(VRAM_C_LCD);
        vramSetBankD(VRAM_D_SUB_SPRITE /* 0x6600000 */);
        REG_DISPCAPCNT = DCAP_BANK(2) | DCAP_ENABLE | DCAP_SIZE(3);
    }
    else {
        lcdMainOnTop();
        vramSetBankD(VRAM_D_LCD);
        vramSetBankC(VRAM_C_SUB_BG_0x06200000);
        REG_DISPCAPCNT = DCAP_BANK(3) | DCAP_ENABLE | DCAP_SIZE(3);
    }

    //-------------------------------------------------------
    //     Render the scene
    //-------------------------------------------------------
    glMatrixMode(GL_MODELVIEW);

    return top;
}

inline void ddFlush() {
    glFlush(0);
}

