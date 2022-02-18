#include "listx.h"

/* semaphore descriptor data structure */
typedef struct semd_t {
struct list_head s_next;
/* Semaphore key */
int *s_key;
/* PCBs blocked on the semaphore */
struct list_head s_procQ;
} semd_t;

static struct semd_t semd_table[MAX_PROC];  //array semafori
static struct list_head semdFree_h;     	//lista semd liberi
static struct list_head semd_h;          	//lista send attivi (ASL)

semd_t* getSemd(int *semAdd) {
	semd_t* s_iter;
	if(!list_empty(&semd_h)) {
		list_for_each_entry(s_iter, &semd_h, s_next);		//scorro la lista ASL
	    if(s_iter->s_key == key)
			return s_iter; 		//se semd corrente contiene la chiave cercata, ritorno il suo puntatore
		else
			return NULL;		//non trova semd con la chiave cercata, ritorno NULL
	}
	else
		return NULL;		//ASL è vuota, ritorno NULL
}

//14
int insertBlocked(int *semAdd, pcb_t *p) {	
	semd_t* semd_key = getSemd(semAdd);	
	if(semd_key == NULL) {		//se semd non è presente nella ASL
		if(list_empty(&semdFree_h)) return TRUE; 		//se la lista dei semd liberi è vuota, ritorno TRUE
		else {			//c'è almeno un semaforo libero
			semd_key = container_of(semdFree_h.next, semd_t, s_next);		//assegno il primo elemento di semdFree a s_key
			list_del(semdFree_h.next);		//elimino il semaforo dalla lista di quelli liberi
		//inizializzo i campi del semaforo
			INIT_LIST_HEAD(&semAdd->s_next); 
			INIT_LIST_HEAD(&semAdd->s_procQ); 
			semd_key->s_key = key;		//assegno la key al semaforo
			insertProcQ(&semd_key->s_procQ, p);			//aggiungo p alla coda dei processi bloccati
			list_add(&semd_key->s_next, &semd_h);		//aggiungo semd_key in testa alla ASL
			p->p_semkey = key;			//chiave del semaforo a cui andrà in coda
			return FALSE;
		}	
	}
	else {		//semd è presente nella ASL
		//imposto i campi di semd_key e di p
		p->p_semkey = key;				//chiave del semaforo a cui andrà in coda
		insertProcQ(&(semd_key->s_procQ), p);		//aggiungo p alla coda dei processi bloccati
		return FALSE;
	}
} 

//15
pcb_t* removeBlocked(int *semAdd){
	pcb_t* procQ_removed;
	semd_t* ASL_Semd = getSemd(semAdd);
	if(ASL_Semd == NULL) return NULL; 	//se semd non è nella ASL ritorno NULL
	    else {    
			procQ_removed = removeProcQ(&(ASL_Semd->s_procQ)); //rimuovo il primo processo dalla lista di quelli bloccati
			if(list_empty(&(ASL_Semd->s_procQ))) {  //se la lista dei processi bloccati è vuota
		    	list_del(&(ASL_Semd->s_next));		//rimuovo semd dalla ASL
				list_add_tail(&(ASL_Semd->s_next), &semdFree_h);    //aggiungo semd alla semdFree
                }
			return procQ_removed;		//ritorno puntatore al processo rimosso
            }
}

//16
pcb_t* outBlocked(pcb_t *p) {
    int *semKey = p->p_semkey;
    semd_t* pSem = getSemd(semKey);
    if (pSem == NULL) return NULL;      //se il semaforo non esiste nella ASL ritorno NULL
    else {    		
		struct list_head* cur; 			//inizializzo elemento corrente usato come iteratore
    	list_for_each(cur, &(pSem->s_procQ)){
            pcb_t* curPcb = container_of(cur, pcb_t, p_next);	//indirizzo del contenitore dell'elemento corrente
                if (curPcb == p) {    
                     list_del(cur);		//rimuovo elemento dalla coda dei processi bloccati
                     return p;
                }
           }
    return NULL;
}


//17
pcb_t* headBlocked(int *semAdd) {
	semd_t* ASL_Semd = getSemd(semAdd);
	if(ASL_Semd == NULL || list_empty(&(ASL_Semd->s_procQ))) return NULL; 	//se semd non è presente nella ASL o la sua coda dei processi è vuota, ritorno NULL
	else {  
		return (headProcQ(&(ASL_Semd->s_procQ))); 		//ritorno puntatore al pcb in testa alla coda
	}
} 

//18
void initASL() {
	int i;
	INIT_LIST_HEAD(&(semd_h));			//per evitare conflitti nel caso ci siano residui in memoria
	INIT_LIST_HEAD(&(semdFree_h));
	for (i=0; i < MAX_PROC; i++)
		semd_t* newSemdEl = &semd_table[i];	{	//inizializzo campi della semd_table
		list_add_tail(&(newSemdEl->s_next), &(semdFree_h));		//aggiungo descrittore della semd_table alla semdFree
	}	
}
