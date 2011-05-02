#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fat.h>
#include <dstk.h>

#include "prof.h"

FILE *flog = NULL;

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

    struct String *txt = obj_new(String, "Test1!");
    iprintf("txt:\"%s\"\n", obj_repr(txt));

    #ifdef CLONE1
        malloc_stats();
        struct String *txt1 = obj_clone(txt);
        iprintf("txt1:\"%s\"\n", obj_repr(txt1));

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
    iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj1)->first->data), O_REFCNT(OLIST(obj1)->first->data));

    obj_delete(obj1);

    iprintf("RC=%d\n", O_REFCNT(txt));
    iprintf("%s from first (RC=%d)\n", obj_repr(OLIST(obj2)->first->data), O_REFCNT(OLIST(obj2)->first->data));

    obj_delete(obj2);

    // txt is now invalid here

    malloc_stats();
    iprintf("ok\n");
}

void test3() {  // children management
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

        // txt is now invalid here

        iprintf("%d children\n", OLIST(obj)->count);
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

void test5() {  // clone test
    struct String *o = obj_new(String, "Test5"), *m;
    iprintf("\"%s\"\n", obj_repr(o));
    obj_setprop(o, 0, "wtf", 4);
    printf("0:%s\n", obj_getprop(o, 0, NULL));

    m = obj_clone(o);
    printf("0:%s\n", obj_getprop(m, 0, NULL));
    printf("%d:%s\n", PROP_DATA, obj_getprop(m, PROP_DATA, NULL));

    obj_delete(o);
    obj_delete(m);

    malloc_stats();
    iprintf("ok\n");
}

void test6() {  // multiple cloning
    int i;

    struct String *txt = obj_new(String, "Test1!");
    iprintf("txt:\"%s\"\n", obj_repr(txt));

    for(i=0; i<20; i++) {
        struct String *txt1 = obj_clone(txt);
        iprintf("txt1:\"%s\"\n", obj_repr(txt1));

        obj_delete(txt1);
        malloc_stats();
    }

    obj_delete(txt);
    malloc_stats();
}

void _bench0(int len, unsigned int NUM) {
    int i;
    unsigned long time;
    void *data = malloc(len);
    if(!data)
        return;

    struct Data *txt = obj_new(Data, len, data);

    PROF_START();
    for(i=0; i<NUM; i++)
        obj_delete(obj_clone(txt));
    PROF_END(time);

    obj_delete(txt);

    printf("%04d %06d %09d %04.3f\n", len, NUM, time, (float)time/(float)NUM);

    if(flog)
        fprintf(flog, "%04d,%06d,%09d,%04.3f\n", len, NUM, time, (float)time/(float)NUM);

    free(data);
}

void bench0() {
    int i;

    #define NUM 20000

    printf("Benchmark0:\n");
    printf("len  NUM    cycles    cyc/op\n");

    if(flog)
        fprintf(flog, "\"len\",\"NUM\",\"cycles\",\"cyc/op\"\n");

    for(i=1; i<10; i+=1)
        _bench0(i, NUM);

    for(i=10; i<=100; i+=10)
        _bench0(i, NUM);
}

int main(void) {
    consoleDemoInit();
    defaultExceptionHandler();
    iprintf("Hello World!\n");

    if(fatInitDefault())
        flog = fopen("/dstktest_log.csv", "ab");

/*    test0();*/
/*    test1();*/
/*    test2();*/
/*    test3();*/
/*    test4();*/
/*    test5();*/
/*    test6();*/

    bench0();

    if(flog)
        fclose(flog);

    while(1)
        swiWaitForVBlank();
}

