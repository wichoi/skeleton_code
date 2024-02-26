#ifndef __TOOLS_LOST_H__
#define __TOOLS_LOST_H__

typedef struct list_item_tag
{
    struct list_item_tag *tail;
    char *data;
    u32 len;
} list_item_t;

typedef struct list_tag
{
    int (*push_back)(struct list_tag*, char*, u32);
    list_item_t* (*get)(struct list_tag*, u32);
    int (*erase)(struct list_tag*, u32);
    int (*clear)(struct list_tag*);
    int (*free)(struct list_tag*);
    list_item_t *item;
    u32 cnt;
} list_t;

list_t* list_new(void);

#endif
