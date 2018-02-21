
#include "list.h"

#include <stddef.h>
#include <stdlib.h>

ListNode * ListNext(ListNode *n){
	if( n == NULL )
		return NULL;
	else
		return n->next;
}

ListNode * ListPush(ListNode *base, ListNode *node){
	if( base == NULL ){
		node->next = NULL;
		return node;
	} else {
		node->next = base;
		return node;
	}
}

ListNode * ListRemove(ListNode *base, ListNode *node){
	if( node == base )
		return node->next;
	
	for(ListNode *curr=base; curr != NULL; curr = curr->next){
		if( curr->next != node )
			continue;
		
		curr->next = node->next;
		return base;
	}
	
	return base;
}

