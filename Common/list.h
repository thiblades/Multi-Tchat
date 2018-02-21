#ifndef list_h
#define list_h

struct ListNode {
	struct ListNode *next;
};

typedef struct ListNode ListNode;

ListNode * ListPush(ListNode *base, ListNode *node);
ListNode * ListRemove(ListNode *base, ListNode *node);

#endif /* list_h */
