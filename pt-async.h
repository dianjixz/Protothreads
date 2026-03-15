#ifndef __PT_ASYNC_H__
#define __PT_ASYNC_H__
#include "pt.h"

struct pt_async {
    struct pt pt;
    struct pt_async *next;
    char delete_flag;
    char (*fun)(struct pt *);
};
void async_add_pt(struct pt_async *_async, struct pt_async *pt);
void async_del_pt(struct pt_async *pt);
void async_poll(struct pt_async *_async);

#define PT_ASYNC_INITIALIZER(struct_name,func) struct pt_async struct_name={.fun=func,.next=0,.delete_flag=0,.pt.lc=0}
#define PT_ASYNC_INIT(struct_name,func) struct_name=(struct pt_async){.fun=func,.next=0,.delete_flag=0,.pt.lc=0}

#ifdef PT_ASYNC_IMPLEMENTATION

void async_add_pt(struct pt_async *_async, struct pt_async *pt)
{
    struct pt_async *_head = _async;
    if ((_async == 0) || (_async == pt) || (pt == 0)) return;
    for(;;) {
        if ((_async->next == pt) || (_async->next == _head)) return;
        if(_async->next == 0){
            _async->next = pt;
            _async->next->delete_flag = 0;
            _async->next->next = 0;
            _async->next->pt.lc = 0;
            return;
        }
        _async = _async->next;
    }
}
static char __del_flage__ = 0;
static void sync_del_pt(struct pt_async *_async)
{
    // if (_async == 0) return;
    if (__del_flage__ == 0) return;
    while (_async->next != 0)
    {
        if(_async->next->delete_flag){
            _async->next = _async->next->next;
            continue;
        }
        _async = _async->next;
    }
    __del_flage__ = 0;
}
void async_del_pt(struct pt_async *pt)
{
    // if(pt == 0) return;
    pt->delete_flag = 1;
    __del_flage__ = 1;
}

void async_poll(struct pt_async *_async)
{
    struct pt_async *_head = _async;
    // if (_async == 0) return;
    if (_async->next == 0) return;
    _async = _async->next;
    do {
        if ((_async->delete_flag == 0) && (_async->fun != 0)) switch (_async->fun(&_async->pt)) {
                case PT_EXITED:
                case PT_ENDED: {
                    async_del_pt(_async);
                } break;
                default:
                    break;
            }
        _async = _async->next;
    } while (_async != 0);
    sync_del_pt(_head);
}
#endif

#endif /* __PT_ASYNC_H__ */
