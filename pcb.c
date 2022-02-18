#include "pandos_types.h"
#include "pandos_const.h"
#include "listx.h"

pcb_t pcbFree_table[MAX_PROC];
list_head pcbFree_h;
// 1
void initPcbs(){
	LIST_HEAD_INIT(pcbFree_h);
	for(int i=0;i<MAX_PROC;i++){
		list_add_tail(&(pcbFree_table[j].p_list),&(pcbFree_h));
		LIST_HEAD_INIT(&(pcbFree_table[j].p_child));
	}
}

//4

void mkEmptyProcQ(struct list_head *head){
	LIST_HEAD_INIT(head);
}
//5
int emptyProcQ(struct list_head *head){
	return (list_empty(head));
}

//6
void insertProcQ(struct list_head* head, pcb_PTR p){
	list_add_tail(p.p_list,head);
}

//7
pcb_t headProcQ(struct list_head* head){
	if (list_empty(head)) return NULL;
	else container_of(list_next(head),pcb_t,p_list);	
}
//8
pcb_t* removeProcQ(struct list_head* haed){
	pcb_PTR tmp;
	if (list_empty(head))
		return NULL;
	else {
		tmp=container_of(head->next,pcb_t,p.list);
		list_del(&(tmp->p_list));
		return tmp;
	}
}
//9
pcb_PTR outProcQ(struct list_head* head, pcb_PTR p){
	pcb_PTR tmp,result = NULL;
	list_for_each_entry(tmp,head,p_list){
		if (tmp == p){
			list_del(&(tmp->p_list));
			result = tmp;
		}
	}
	return result;
}

//10
int emptyChild(pcb_PTR p){
	return (list_empty(p->p_child));
}
//11
void insertChild(pcb_PTR prnt,pcb_PTR p){
	list_add_tail(p->p_sib,prnt->p_child);
	p->p_parent=prnt;
}
//12
pcb_PTR removeChild(pcb_PTR p){
	if (list_empty(p->p.p_child))
		return NULL;
	else{
		pcb_PTR children;
		children=container_of(p->p_child.next,pcb_t,p_sib);
		list_del(children->p_sib);
		children->p_parent=NULL;
		return children;
	}
} 
//13
pcb_PTR outChild(pcb_PTR p) {

	pcb_PTR tmp;

	struct list_head *head = &( p->p_parent->p_child );

	list_for_each_entry(tmp,head,p_sib) {

		if ( tmp==p ) {

			list_del(&(tmp->p_sib));

			return tmp;

		}

	}

	/* Se il pcb puntato da p non ha un padre. */
	return NULL;

}