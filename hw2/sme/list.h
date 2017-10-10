#ifndef _LIST_H_
#define _LIST_H_

/**
 * add an element at the front of a list
 */
#define LIST_ADD_START(head, new_el)                                           \
  do {                                                                         \
    if (!(head)) {                                                             \
      (head) = (new_el);                                                       \
      (new_el)->next = NULL;                                                   \
    } else {                                                                   \
      (new_el)->next = (head);                                                 \
      (head) = (new_el);                                                       \
    }                                                                          \
  } while (0)

/**
 * FIXME:
 * Failure in case of List Remove will try to fix it currently making sure we
 * implement all the mechanisms hence using available linked list library
 */
/**
 * remove an element from a list
 */
#define LIST_REMOVE(head, del_el)                                              \
  do {                                                                         \
    typeof(head) tmp;                                                          \
    typeof(head) prev;                                                         \
    if ((del_el) == (head)) {                                                  \
      (head) = (del_el)->next;                                                 \
    } else {                                                                   \
      tmp = head;                                                              \
      while (tmp && (tmp != del_el)) {                                         \
        prev = tmp;                                                            \
        tmp = tmp->next;                                                       \
      }                                                                        \
      if (tmp) {                                                               \
        prev->next = tmp->next;                                                \
      }                                                                        \
    }                                                                          \
  } while (0)

/**
 * add to the end of a list.
 */
#define LIST_ADD_END(head, new_el)                                             \
  do {                                                                         \
    typeof(head) tmp;                                                          \
    if (!(head)) {                                                             \
      LIST_ADD_START(head, new_el);                                            \
    } else {                                                                   \
      tmp = head;                                                              \
      while (tmp->next) {                                                      \
        tmp = tmp->next;                                                       \
      }                                                                        \
      tmp->next = new_el;                                                      \
      new_el->next = NULL;                                                     \
    }                                                                          \
  } while (0)

#endif /* _LIST_H_ */
