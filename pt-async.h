#ifndef __PT_ASYNC_H__
#define __PT_ASYNC_H__
#include "pt.h"

struct pt_async {
    struct pt pt;
    struct pt_async *next;
    bool delete_flag;
    char (*fun)(struct pt *);
};
void async_add_pt(struct pt_async *_async, struct pt_async *pt);
void async_del_pt(struct pt_async *pt);
void async_poll(struct pt_async *_async);

#define PT_ASYNC_INITIALIZER(struct_name,fun) struct pt_async struct_name={.fun=fun,.next=0,.delete_flag=false,.pt.lc=0}

#ifdef PT_ASYNC_IMPLEMENTATION

void async_add_pt(struct pt_async *_async, struct pt_async *pt)
{
    struct pt_async *_head = _async;
    if ((_async == 0) || (_async == pt) || (pt == 0)) return;
    for(;;) {
        if ((_async->next == pt) || (_async->next == _head)) return;
        if(_async->next == 0){
            _async->next = pt;
            _async->next->delete_flag = false;
            _async->next->next = 0;
            _async->next->pt.lc = 0;
            return;
        }
        _async = _async->next;
    }
}
static bool __del_flage__ = false;
static void sync_del_pt(struct pt_async *_async)
{
    // if (_async == 0) return;
    if (__del_flage__ == false) return;
    while (_async->next != 0)
    {
        if(_async->next->delete_flag){
            _async->next = _async->next->next;
            continue;
        }
        _async = _async->next;
    }
    __del_flage__ = false;
}
void async_del_pt(struct pt_async *pt)
{
    // if(pt == 0) return;
    pt->delete_flag = true;
    __del_flage__ = true;
}

void async_poll(struct pt_async *_async)
{
    struct pt_async *_head = _async;
    // if (_async == 0) return;
    if (_async->next == 0) return;
    _async = _async->next;
    do {
        if ((_async->delete_flag == false) && (_async->fun != 0)) switch (_async->fun(&_async->pt)) {
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
