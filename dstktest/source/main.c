#include <nds.h>
#include <stdio.h>
#include <dstk.h>

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
    consoleDemoInit();
    iprintf("Hello World!\n");

#define STRING1
//#define CLONE1
#define OBJECT1
#define CLONE2

#ifdef STRING1
    malloc_stats();
    struct String *txt = obj_new(String, "Tiny text!");
    iprintf("\"%s\"\n", obj_repr(txt));

    #ifdef CLONE1
        malloc_stats();
        struct String *txt1 = obj_clone(txt);
        iprintf("\"%s\"\n", obj_repr(txt1));

        malloc_stats();
        obj_delete(txt1);
    #endif
#endif

#ifdef OBJECT1
    malloc_stats();
    struct Object *obj = obj_new(Object, obj_new(String, "Tst"), obj_new(AType, 6, "12345"), obj_clone(txt), NULL);
    iprintf("%d children\n", OLIST(obj)->count);
    iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj)->first->data), REFCNT(OLIST(obj)->first->data));
    iprintf("%s from last (RC=%d)\n", obj_repr(OLIST(obj)->last->data), REFCNT(OLIST(obj)->last->data));

    #ifdef CLONE2
        malloc_stats();
        struct Object *obj1 = obj_clone(obj);
        iprintf("%d children\n", OLIST(obj1)->count);
        iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj1)->first->data), REFCNT(OLIST(obj1)->first->data));
        iprintf("%s from last (RC=%d)\n", obj_repr(OLIST(obj1)->last->data), REFCNT(OLIST(obj1)->last->data));
    #endif

    malloc_stats();
    obj_delete(obj);

    #ifdef CLONE2
        malloc_stats();
        obj_delete(obj1);
    #endif
#endif

#ifdef STRING1
    malloc_stats();
    obj_delete(txt);
#endif

   malloc_stats();
    iprintf("ok\n");

    while(1)
        swiWaitForVBlank();

}
