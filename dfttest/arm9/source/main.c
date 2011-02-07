#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <nds.h>
#include <fat.h>

#include <dsdual.h>
#include <dstk.h>
#include <dft.h>

/*#undef assert*/
/*#define assert(cond) if(!(cond)) return*/

void renderText(sDFT *font, char *text, u16 x, u16 y) {
    assert(font);
    assert(text);
    assert(font->texId);

    ddBegin2D();

    glBindTexture(0, font->texId);
    glColorTable(font->texFmt, font->palAddr);

    UTF8_ITER iter;
    wchar32 c;

    for(c = utf8_iter(&iter, text); c>0; c = utf8_iter(&iter, NULL)) {
        sDFT_GPROPS *glyph = dft_get_glyph(font, c);  // get position of the current character on the loaded texture

        if(!glyph) continue;

        ddTexQuad2D(glyph->u, glyph->v, glyph->w, glyph->h, x + glyph->dx, y - glyph->dy, 1 /* z */);
        x += glyph->adx;
    }

    ddEnd2D();
}

#define BREAK do { while(!(keysDown()&KEY_B)) { scanKeys(); swiWaitForVBlank(); } scanKeys(); } while(0)

int main() {
    // set video mode
    videoSetMode(MODE_5_3D);
    videoSetModeSub(MODE_5_2D);

    ddVideoinit();  // init 3D dual screen using VRAM banks C and D           TODO : allow vram allocation setting
    fatInitDefault();

#if 0
#   if 0
    // setup bg1 as textbg and put bg0 behind bg1
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 0 /* 1536 bytes map data @ 0x06000000 */, 1 /* 4kiB tiles data @ 0x06000800 */, true, true);
    bgSetPriority(0, 1);
#   else
    vramSetBankE(VRAM_E_MAIN_BG /* 0x06000000 */);
    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 0 /* 1536 bytes map data @ 0x06000000 */, 1 /* 4kiB tiles data @ 0x06000800 */, true, true);
    bgSetPriority(0, 1);
#   endif

    printf("hello world... on 2 screens !");
#endif

    vramSetBankA(VRAM_A_TEXTURE_SLOT0);
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);
/*    vramSetBankC(VRAM_C_TEXTURE_SLOT2);*/
/*    vramSetBankC(VRAM_D_TEXTURE_SLOT3);*/
    vramSetBankE(VRAM_E_TEX_PALETTE);

    sDFT font1, font2, font3, font4;

    dft_init(&font1, "/font1.dft"); // normal
    dft_load(&font1);

    dft_init(&font2, "/font2.dft"); // italic
    dft_load(&font2);

    dft_init(&font3, "/font3.dft"); // with utf-8 characters
    dft_load(&font3);

    dft_init(&font4, "/font4.dft"); // with utf-8 chinese characters
    dft_load(&font4);

    bool top;
    while(true) {
        top = ddSwitch();

        if(top) {    // top screen
            renderText(&font1, "This is the top screen!", 50, 130);

            renderText(&font1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 10, 10);
            renderText(&font2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 10, 25);

            renderText(&font1, "abcdefghijklmnopqrstuvwxyz", 10, 40);
            renderText(&font2, "abcdefghijklmnopqrstuvwxyz", 10, 55);

            renderText(&font3, "even in UTF-8 ? éàèöç€ <= yes!", 10, 90);
/*            renderText(&font4, "世界您好! <= yeah²", 50, 115);*/
        }
        else {  // bottom screen
            renderText(&font1, "And ... omfg! ... this is the bottom one!", 30, 100);
            renderText(&font2, "Even in italic? yeah!", 30, 130);
            renderText(&font3, "Another font? yes, bigger!", 10, 150);
        }

        ddFlush();
/*        swiWaitForVBlank();*/
    }

    return 0;
}

