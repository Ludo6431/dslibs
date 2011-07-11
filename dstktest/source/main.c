#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fat.h>
#include <dstk.h>

#include "prof.h"

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
    obj_setprop(o, 2, "Test1!", 8);

    printf("0:%s\n", obj_getprop(o, 0, NULL));
    printf("1:%s\n", obj_getprop(o, 1, NULL));
    printf("2:%s\n", obj_getprop(o, 2, NULL));

    obj_setprop(o, 0, "omg", 4);
    obj_setprop(o, 1, "Test1...", 8);
    obj_setprop(o, 2, "lac", 4);

    printf("0:%s\n", obj_getprop(o, 0, NULL));
    printf("1:%s\n", obj_getprop(o, 1, NULL));
    printf("2:%s\n", obj_getprop(o, 2, NULL));

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

    obj_delete(o);
    obj_delete(m);

    malloc_stats();
}

void _bench0(int len, unsigned int NUM, FILE *flog) {
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

void bench0(FILE *flog) {
    int i;

    #define NUM 20000

    printf("Benchmark0:\n");
    printf("len  NUM    cycles    cyc/op\n");

    if(flog)
        fprintf(flog, "\"len\",\"NUM\",\"cycles\",\"cyc/op\"\n");

    for(i=1; i<10; i+=1)
        _bench0(i, NUM, flog);

    for(i=10; i<=100; i+=10)
        _bench0(i, NUM, flog);
}

#define NUM 1000
#define NBCLONES 30
void _bench1(int len, FILE *flog) {
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
            obj_delete(clones[j]);
    }
    PROF_END(time);

    obj_delete(txt);

    printf("%04d %09d %04.3f\n", len, time, (float)time/(float)NUM);

    if(flog)
        fprintf(flog, "%04d,%06d,%02d,%09d,%04.3f\n", len, NUM, NBCLONES, time, (float)time/(float)NUM);

    free(data);
}

void bench1(FILE *flog) {
    int i;

    printf("Benchmark1:\n");
    printf("NUM=%d; NBCLONES=%d\n", NUM, NBCLONES);
    printf("len  cycles    cyc/op\n");

    if(flog)
        fprintf(flog, "\"len\",\"NUM\",\"NBCLONES\",\"cycles\",\"cyc/op\"\n");

    for(i=1; i<10; i+=1)
        _bench1(i, flog);

    for(i=10; i<=40; i+=10)
        _bench1(i, flog);
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

#define NB 15
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

int main(void) {
    consoleDemoInit();
    defaultExceptionHandler();
    iprintf("Hello World!\n");

    FILE *flog = NULL;
    if(fatInitDefault())
        flog = fopen("/dstktest_log.csv", "ab");

/*    test0();*/
/*    test1();*/
/*    test2();*/

/*    bench0(flog);*/
    bench1(flog);

/*    slice_test();*/

    iprintf("ok");

    if(flog)
        fclose(flog);

    while(1)
        swiWaitForVBlank();
}

