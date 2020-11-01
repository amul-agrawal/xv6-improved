// 8c60c451ba0933cf2b4c7e40967bfa38

#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"
#include "traps.h"
#include "x86.h"

struct node* find_node() {

    int found = 0;
    int ptr = 0;

    while(!found && ptr < NPROC) {
        if (avl_nodes[ptr].use == 0) {
            avl_nodes[ptr].use = 1;
            found = 1;
            continue;
        }
        ptr++;
    }

    if (found) return &avl_nodes[ptr];
    else return 0; 
}

int size(struct node* head) {
    // Passing a copy of head pointer.
    int num = 0;
    for(; head!=0; head = head->next) {
        num++;
    }
    return num;
}

void initialize_found_node(struct node* this, struct proc* p)  {
    this->p = p;
    this->next = 0;
    this->use = 0;
}

struct node* push(struct node* head, struct proc* p) {
    struct node* found_node = find_node();

    if(!found_node) return 0;
    
    initialize_found_node(head, p);

    // this is the head.
    if (head == 0) {
        return found_node;
    }

    struct node* cur = head;
    while (cur->next != 0) {
        cur = cur->next;
    }
    cur->next = found_node;
    return head;
}

struct node* pop(struct node* head) {
    // returns the new head.
    if (head == 0) {
        return 0;
    } else {
        struct node* temp = head->next;
        head->use = 0;
        return temp;
    }
}

// move from "down" queue to "up" queue, for process which have exceeded threshold.

void moveup(struct node** down, struct node** up, int threshold) {
    if (down == 0 || up == 0) {
        return;
    } 
    else if (*down == 0) {
        return;
    }

    struct node* cur = *down;
    struct node* prev = 0;

    int count = 0;

    while (cur != 0 && (ticks - cur->p->age_time > threshold) ) {
        cur->p->queue--;
        cur->p->cur_timeslices = ticks;
        count++;
        prev = cur;
        cur = cur->next;
    }
    if (prev == 0) {
        return ;
    }

    prev->next = 0;
    *down = cur;

    if (*up != 0) {
        struct node* up_end = *up;
        for(; up_end->next!=0; up_end = up_end->next) ;
        up_end->next = *down;
    } else {
        *up = *down;
    }

    return ;
}
