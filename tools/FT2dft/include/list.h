#ifndef _LIST_H
#define _LIST_H

typedef struct sLEl sLEl;
struct sLEl {
    sLEl *prev;
    sLEl *next;

    void *data;
};

typedef struct sList sList;
struct sList {
    unsigned count;

    sLEl *head;
    sLEl *tail;
};

typedef void (*datahandler)(void *);
typedef int (*resulthandler)(void *);
typedef int (*cmphandler)(const void *, const void *);

inline sLEl *   L_new_el            (void *data);
void *          L_add_head          (sList *list, void *data);
void *          L_add_tail          (sList *list, void *data);
void *          L_insert_after_el   (sList *list, sLEl *element, void *data);
int             L_fsck              (sList *list);
void            L_foreach           (sList *list, datahandler hnd);
void            L_copy              (sList *dst, sList *src, unsigned datasize);
void            L_sort              (sList *list, cmphandler hnd);
void            L_filter            (sList *list, resulthandler valid, datahandler del);
sLEl *          L_find_el           (sList *list, void *data);
void *          L_drop_el           (sList *list, sLEl *element);
inline void *   L_drop              (sList *list, void *data);
void            L_dropall           (sList *list, datahandler hnd);

#endif

