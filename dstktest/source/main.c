#include <nds.h>
#include <stdio.h>
#include <dstk.h>

void test0() {  // first little test
    struct String *o = obj_new(String, "Test0!");
    iprintf("\"%s\"\n", obj_repr(o));
    obj_delete(o);
    malloc_stats();
    iprintf("ok\n");
}

void test1() {  // general tests (creation / children / deletion / memory leaks)
#define CLONE1
#define OBJECT1
#define CLONE2

    malloc_stats();
    struct String *txt = obj_new(String, "Test1!");
    iprintf("\"%s\"\n", obj_repr(txt));

    #ifdef CLONE1
        malloc_stats();
        struct String *txt1 = obj_clone(txt);
        iprintf("\"%s\"\n", obj_repr(txt1));

        malloc_stats();
        obj_delete(txt1);
    #endif

#ifdef OBJECT1
    malloc_stats();
    struct Object *obj = obj_new(Object, obj_new(String, "Tst"), obj_new(Data, 6, "12345"), obj_clone(txt), NULL);
    iprintf("%d children\n", OLIST(obj)->count);
    iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj)->first->data), O_REFCNT(OLIST(obj)->first->data));
    iprintf("%s from last (RC=%d)\n", obj_repr(OLIST(obj)->last->data), O_REFCNT(OLIST(obj)->last->data));

    #ifdef CLONE2
        malloc_stats();
        struct Object *obj1 = obj_clone(obj);
        iprintf("%d children\n", OLIST(obj1)->count);
        iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj1)->first->data), O_REFCNT(OLIST(obj1)->first->data));
        iprintf("%s from last (RC=%d)\n", obj_repr(OLIST(obj1)->last->data), O_REFCNT(OLIST(obj1)->last->data));
    #endif

    malloc_stats();
    obj_delete(obj);

    #ifdef CLONE2
        malloc_stats();
        obj_delete(obj1);
    #endif
#endif

    malloc_stats();
    obj_delete(txt);

    malloc_stats();
    iprintf("ok\n");
}

void test2() {  // multiple parents test
    struct String *txt = obj_new(String, "Test2!");
    iprintf("\"%s\"\n", obj_repr(txt));

    struct Objet *obj1 = obj_new(Object, txt, NULL);
    struct Objet *obj2 = obj_new(Object, txt, NULL);

    iprintf("RC=%d\n", O_REFCNT(txt));
    iprintf("%dB\n", DATA(txt)->datasize);
    iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj1)->first->data), O_REFCNT(OLIST(obj1)->first->data));

    obj_delete(obj1);

    iprintf("RC=%d\n", O_REFCNT(txt));
    iprintf("%dB\n", DATA(txt)->datasize);
    iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj2)->first->data), O_REFCNT(OLIST(obj2)->first->data));

    obj_delete(obj2);

    // txt is now invalid here

    malloc_stats();
    iprintf("ok\n");
}

void test3() {  // children management
    malloc_stats();

    struct String *txt = obj_new(String, "Test3!");
    iprintf("\"%s\"\n", obj_repr(txt));

    struct Objet *obj = obj_new(Object, NULL);

    iprintf("%d children and RC=%d\n", OLIST(obj)->count, O_REFCNT(txt));

    // add two elements
    obj_add(obj, obj_clone(txt));
    obj_add(obj, txt);

    iprintf("%d children and RC=%d\n", OLIST(obj)->count, O_REFCNT(txt));

    // add 20 elements
    int i;
    for(i=0; i<20; i++)
        obj_add(obj, obj_clone(txt));

    iprintf("%d children and RC=%d\n", OLIST(obj)->count, O_REFCNT(txt));

    if(obj_find(obj, txt)) {
        // remove txt from the children
        obj_delete(obj_drop(obj, txt));

        iprintf("%d children and RC=%d\n", OLIST(obj)->count, O_REFCNT(txt));
    }
    else
        iprintf("Wtf ? txt is not there!\n");

    // remove all the children and obj
    obj_delete(obj);

    malloc_stats();
    iprintf("ok\n");
}

void test4() {  // property test
    struct PObj *o = obj_new(PObj);

    obj_setprop(o, 0, "wtf", 4);
    obj_setprop(o, 1, "wth", 4);
    obj_setprop(o, 2, "ludovic", 8);

    printf("0:%s\n", obj_getprop(o, 0, NULL));
    printf("1:%s\n", obj_getprop(o, 1, NULL));
    printf("2:%s\n", obj_getprop(o, 2, NULL));

    obj_setprop(o, 0, "omg", 4);
    obj_setprop(o, 1, "lacoste", 8);
    obj_setprop(o, 2, "lac", 4);

    printf("0:%s\n", obj_getprop(o, 0, NULL));
    printf("1:%s\n", obj_getprop(o, 1, NULL));
    printf("2:%s\n", obj_getprop(o, 2, NULL));

    obj_delete(o);

    malloc_stats();
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
    consoleDemoInit();
    iprintf("Hello World!\n");

/*    test0();*/
/*    test1();*/
/*    test2();*/
/*    test3();*/
    test4();

    while(1)
        swiWaitForVBlank();
}

