#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  assert(free_ != NULL);
  WP *current = free_;
  WP *prev = free_; 
  while(current != NULL) {
    if(current->next == NULL) {
      prev->next = NULL;
      return current;
    }
    prev = current;
    current = current->next;
  }
  return NULL;
}

void free_wp(WP *wp) {
  assert(wp);
  if(free_ == NULL) {
    free_ = wp;
    return;
  }

  WP *current = free_;
  while(current != NULL){
    if(current->next == NULL) {
      current->next = wp;
      return ;
    }
    current = current->next;
  }    
}