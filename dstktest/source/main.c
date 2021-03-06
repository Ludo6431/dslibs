#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fat.h>
#include <dstk.h>

#include "prof.h"

FILE *flog = NULL;

void test0() {  // first little test
    struct String *o = obj_new(String, "Test0!");
    iprintf("\"%s\"\n", obj_repr(o));
    obj_delete(o);
    malloc_stats();
}

void test1() {  // properties test
    struct Object *o = obj_new(Object);

    obj_setprop(o, 0, "wtf", 4);
    obj_setprop(o, 1, "wth", 4);
    obj_setprop(o, 2, "Test1!", 7);

    printf("0:%s\n", obj_getprop(o, 0, NULL));
    printf("1:%s\n", obj_getprop(o, 1, NULL));
    printf("2:%s\n", obj_getprop(o, 2, NULL));

    obj_setprop(o, 0, "omg", 4);
    obj_setprop(o, 1, "Test1...", 9);
    obj_setprop(o, 2, "lac", 4);

    printf("0:%s\n", obj_getprop(o, 0, NULL));
    printf("1:%s\n", obj_getprop(o, 1, NULL));
    printf("2:%s\n", obj_getprop(o, 2, NULL));

slice_dump_all(stderr);

    obj_delete(o);

    malloc_stats();
}

void test2() {  // clone test
    struct String *o = obj_new(String, "Test2"), *m;
    iprintf("\"%s\"\n", obj_repr(o));
    obj_setprop(o, 0, "wtf", 4);
    printf("0:%s\n", obj_getprop(o, 0, NULL));

    m = obj_clone(o);
    printf("0:%s\n", obj_getprop(m, 0, NULL));
    printf("%d:%s\n", PROP_DATA, obj_getprop(m, PROP_DATA, NULL));

slice_dump_all(stderr);

    obj_delete(o);
    obj_delete(m);

    malloc_stats();
}

#define NUM 100
void _bench0(int len) {
    int i;
    unsigned long time;
    void *data = malloc(len);
    if(!data)
        return;

    struct Data *objs[NUM];

    // allocate
        PROF_START();
        for(i=0; i<NUM; i++)
            objs[i] = obj_new(Data, len, data);
        PROF_END(time);

        printf("create %04d %09d %04.3f\n", len, time, (float)time/(float)NUM);

        if(flog)
            fprintf(flog, "\"C\",%04d,%09d,%04.3f\n", len, time, (float)time/(float)NUM);

    // free
        PROF_START();
        for(i=0; i<NUM; i++)
            obj_delete(objs[i]);
        PROF_END(time);

        printf("delete %04d %09d %04.3f\n", len, time, (float)time/(float)NUM);

        if(flog)
            fprintf(flog, "\"D\",%04d,%09d,%04.3f\n", len, time, (float)time/(float)NUM);

    free(data);
}

void bench0() {
    int i;

    printf("Benchmark0:\n");
    printf("op     len  cycles    cyc/op\n");

    if(flog)
        fprintf(flog, "\"Benchmark0\"\n\"NUM=\",%d\n\"operation\",\"len\",\"cycles\",\"cyc/op\"\n", NUM);

    for(i=1; i<10; i+=1)
        _bench0(i);

    for(i=10; i<=100; i+=10)
        _bench0(i);
}
#undef NUM

#define NUM 1000
#define NBCLONES 27
void _bench1(int len) {
    int i, j;
    unsigned long time;
    void *data = malloc(len);
    if(!data)
        return;

    struct Data *txt = obj_new(Data, len, data);

    struct Data *clones[NBCLONES];

    PROF_START();
    for(i=0; i<NUM; i++) {
        for(j=0; j<NBCLONES; j++)
            clones[j] = obj_clone(txt);

        for(j=0; j<NBCLONES; j++)
            if(clones[j])
                obj_delete(clones[j]);
    }
    PROF_END(time);

    obj_delete(txt);

    printf("%04d %09d %04.3f\n", len, time, (float)time/(float)NUM);

    if(flog)
        fprintf(flog, "%04d,%09d,%04.3f\n", len, time, (float)time/(float)NUM);

    free(data);
}

void bench1() {
    int i;

    printf("Benchmark1:\n");
    printf("NUM=%d; NBCLONES=%d\n", NUM, NBCLONES);
    printf("len  cycles    cyc/op\n");

    if(flog)
        fprintf(flog, "\"Benchmark1\"\n\"NUM=\",%d\n\"NBCLONES=\",%d\n\"len\",\"cycles\",\"cyc/op\"\n", NUM, NBCLONES);

    for(i=1; i<10; i+=1)
        _bench1(i);

    for(i=10; i<=40; i+=10)
        _bench1(i);
}
#undef NUM
#undef NBCLONES

void slice_test() {
    typedef struct {
        unsigned int f1, f2, f3;
    } val;

    iprintf("Slice test\n");

    val *v = slice_new(val);
printf("v=%p\n", v);

    v->f1 = 1;
    v->f2 = 43;
    v->f3 = 51;

#define NB 100
    val *w[NB];
    int i;

    for(i=0; i<NB; i++) {
        w[i] = slice_dup(val, i?w[i-1]:v);
        printf("w[%02d]=%p\n", i, w[i]);
    }

    printf("f1=%d\n", w[NB-1]->f1);
    printf("f2=%d\n", w[NB-1]->f2);
    printf("f3=%d\n", w[NB-1]->f3);

    slice_delete(val, v);
    for(i=0; i<NB; i++)
        slice_delete(val, w[i]);
}

void dlist_test() {
    DList *list = NULL, *el, *list2;
    int i;

    // allocate a list
    for(i=0; i<30; i++)
        list = dlist_prepend(list, (void *)i);

fprintf(flog, "first dump\n");
slice_dump_all(flog);
dlist_dump(list, flog);

    // remove some elements
    el = dlist_nth(list, 0);
    list = dlist_drop(list, el);
    dlist_free(el);

    el = dlist_nth(list, 4);
    list = dlist_drop(list, el);
    dlist_free(el);

    el = dlist_nth(list, dlist_length(list)-1);
    list = dlist_drop(list, el);
    dlist_free(el);

fprintf(flog, "second dump\n");
slice_dump_all(flog);
dlist_dump(list, flog);

    // copy the list
    list2 = dlist_copy(list);

fprintf(flog, "third dump\n");
slice_dump_all(flog);
dlist_dump(list2, flog);

    // free all the elements of the first list
    list = dlist_free_all(list);

fprintf(flog, "fourth dump\n");
slice_dump_all(flog);

    // free all the elements of the second list
    list2 = dlist_free_all(list2);

fprintf(flog, "fifth dump\n");
slice_dump_all(flog);
}

void slist_test() {
    typedef struct MyList MyList;
    struct MyList {
        MyList *next;   // you need 'next' in the first place
        // GSList _;     // or like that

        unsigned int num;
    };

    MyList *list = NULL, *el, *elprev, *list2;
    int i;

    // allocate a list
    for(i=0; i<30; i++) {
        el = gslist_new(MyList);
        el->num = i;
        list = gslist_prepend(list, el);
    }

fprintf(flog, "first dump\n");
slice_dump_all(flog);
gslist_dump(list, flog);

    // remove some elements
    el = gslist_nth(list, 0);
    list = gslist_drop_next(list, NULL); // remove the first element
    gslist_free(el);

    elprev = gslist_nth(list, 4-1);
    el = elprev->next;
    list = gslist_drop_next(list, elprev);   // remove the 5th element
    gslist_free(el);

    el = gslist_nth(list, 6);
    list = gslist_drop(list, el);   // remove the 7th element
    gslist_free(el);

    elprev = gslist_nth(list, gslist_length(list)-1-1);
    el = elprev->next;
    list = gslist_drop_next(list, elprev);   // remove the last element
    gslist_free(el);

fprintf(flog, "second dump\n");
slice_dump_all(flog);
gslist_dump(list, flog);

    // copy the list
    list2 = gslist_copy(list);

fprintf(flog, "third dump\n");
slice_dump_all(flog);
gslist_dump(list2, flog);

    // free all the elements of the first list
    list = gslist_free_all(list);

fprintf(flog, "fourth dump\n");
slice_dump_all(flog);

    // free all the elements of the second list
    list2 = gslist_free_all(list2);

fprintf(flog, "fifth dump\n");
slice_dump_all(flog);
}

int main(void) {
    consoleDemoInit();
    consoleDebugInit(DebugDevice_NOCASH);   // uses 1032 bytes on the heap
    defaultExceptionHandler();

    iprintf("dstktest-"BRANCHNAME"\n");

    if(fatInitDefault())
        flog = fopen("/dstktest_log.csv", "ab");

    if(flog) {
        time_t result;
        struct tm *timeptr;

        result = time(NULL);
        timeptr = localtime(&result);

        iprintf("Log opened\n");
        fprintf(flog, "\"Log opened ["BRANCHNAME"] - %d/%d/%d %.2d:%.2d:%.2d\"\n",
            1900 + timeptr->tm_year, 1 + timeptr->tm_mon, timeptr->tm_mday,
            timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec
        );
    }

/*    test0();*/
/*    test1();*/
/*    test2();*/

/*    bench0();*/
/*    bench1();*/

/*    slice_test();*/
/*    dlist_test();*/
    slist_test();

    if(flog) {
        time_t result;
        struct tm *timeptr;

        result = time(NULL);
        timeptr = localtime(&result);

        iprintf("Log closed\n");
        fprintf(flog, "\"Log closed ["BRANCHNAME"] - %d/%d/%d %.2d:%.2d:%.2d\"\n",
            1900 + timeptr->tm_year, 1 + timeptr->tm_mon, timeptr->tm_mday,
            timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec
        );

        fclose(flog);
    }

    iprintf("ok");

    while(1)
        swiWaitForVBlank();
}

