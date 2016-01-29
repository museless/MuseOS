#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "nest.h"
#include "proto.h"


/* this function used to init */
/* memory, 0 ~ 1M(kernel part) */
/* 1M ~ 5M(pageBuf table) */
/* 5M ~ 15M(buffer) - (Food store) */
/* 15M ~ 39M(new proc) - (egg or larvae) */
/* 39M ~ 64M(leap) - (plc for nursery) */

/*-----memory_init-----*/
void memory_init(void)
{
	NEST	nStru;
	int	nCir;

	memset(pageBuf, 0, NR_PAGE * sizeof(u16));

	/* init food store */
	pBuffer = (struct nest_stru *)(KERNEL_SIZE + PAGE_TABLE_SIZE);
	strncpy(nStru.ns_stru_name, "BUFFER_STORE", 10);
	nStru.ns_magic = Magic_F_S_1;
	nStru.ns_size = NR_BUFFER_SIZE;
	nStru.ns_data.nbuffer = NR_BUFFER_BLK;
	nStru.ns_wait = NULL;
	memcpy(pBuffer, &nStru, sizeof(struct nest_stru));

	/* init nursery */
	pNursery = (struct nest_stru *)(NURSERY_OFF + NURSERY_SIZE);
	strncpy(nStru.ns_stru_name, "NURSERY", 7);
	nStru.ns_magic = Magic_N_1;
	nStru.ns_size = NURSERY_SIZE;
	nStru.ns_data.nfamily = NR_FAMILY;
	nStru.ns_wait = NULL;
	nStru.ns_charge_id = NURSERY_ANT;
	memcpy(pNursery, &nStru, sizeof(struct nest_stru));

	/* init place for nursery */
	pHeap = (struct nest_stru *)(HEAP_OFF + PER_PAGE_BYTE);
	strncpy(nStru.ns_stru_name, "HEAP", 4);
	nStru.ns_magic = Magic_P_N_1;
	nStru.ns_size = HEAP_SIZE - (PER_PAGE_BYTE * 2);
	nStru.ns_data.nheap = HEAP_SIZE / PER_PAGE_BYTE - 2;
	nStru.ns_wait = NULL;
	nStru.ns_charge_id = SMALL_WORKER;
	memcpy(pHeap, &nStru, sizeof(struct nest_stru));
	
	mem_set_nest_protection();

	for(nCir = 0; nCir < HEAP_OFF / PER_PAGE_BYTE + 2; nCir++)
		pageBuf[nCir] |= USED;
}


/*-----mem_alloc_heap-----*/	
void *mem_alloc_heap(int aSize)
{
	if(mem_allow_enter_heap()) {
		PROC	*pWait = pHeap->ns_wait;

		while(pWait != NULL)
			pWait = pWait->proc_alloc_heap_next;

		pWait = proc_on_view;
		pWait->proc_alloc_heap_next = NULL;

		return	NULL;
	}

	if(aSize >= HEAP_SIZE || aSize >= pHeap->ns_data.nheap) {
		proc_on_view->proc_errno = EMEMOUT;
		return	NULL;
	}

	u32	nPage = aSize / PER_PAGE_BYTE + ((aSize % PER_PAGE_BYTE) ? 1 : 0);
	int	nCir, i_loop;

	for(nCir = (HEAP_OFF / PER_PAGE_BYTE) + 2; nCir < MEMORY_SIZE / PER_PAGE_BYTE; nCir++) {
		if(!(pageBuf[nCir] & USED)) {
			for(i_loop = 0; i_loop < nPage; i_loop++) {
				if(pageBuf[nCir + i_loop] & USED)
					break;
			}

			if(i_loop == nPage) {
				pageBuf[nCir] |= (nPage << 1);
				for(; nPage > 0; nPage--)
					pageBuf[nCir + nPage - 1] |= USED;
			} else {
				nCir += i_loop;
				continue;
			}

			pHeap->ns_data.nheap -= nPage;
			return	(void *)(nCir * PER_PAGE_BYTE);
		}
	}

	proc_on_view->proc_errno = EMEMOUT;
	return	NULL;
}


/*-----mem_alloc_nursery-----*/
void *mem_alloc_nursery(int pID)
{
	if(mem_allow_enter_nursery())
		return	NULL;
	
	if(pID > NR_ANT + NR_DF + NR_FAMILY)
		printx(MAGIC_KERN, "!! proc id bigger than system limit");

	if(pNursery->ns_size < PER_FAMILY_SIZE) {
		proc_on_view->proc_errno = EMEMFUL;
		return	NULL;
	}

	int	nCir;

	for(nCir = 0; nCir < NR_FAMILY; nCir++) {
		if(!(famStBuf[nCir] & USED)) {
			famStBuf[nCir] |= (pID << 1);
			famStBuf[nCir] |= USED;

			pNursery->ns_size -= PER_FAMILY_SIZE;
			pNursery->ns_data.nfamily -= 1;

			return	(u32 *)(NURSERY_OFF + nCir * PER_FAMILY_SIZE);
		}
	}

	return	NULL;
}


/*-----mem_free_nursery-----*/
int mem_free_nursery(int pID)
{
	int	pLocate;

	if(pID > NR_ANT + NR_DF + NR_FAMILY)
		printx(MAGIC_KERN, "in free_nursery pID bigger than system limit");

	if(!mem_nursery_find_proc(pID, &pLocate)) {
		proc_on_view->proc_errno = EPNOTINLIST;
		return	0;
	}

	famStBuf[pLocate] &= FREE;
	
	pNursery->ns_size += PER_FAMILY_SIZE;	
	pNursery->ns_data.nfamily += 1;

	return	1;
}


/*-----mem_free_heap-----*/
int mem_free_heap(void *fAddr)
{
	if(((u32)fAddr % PER_PAGE_BYTE) || ((u32)fAddr > MEMORY_SIZE)) {
		proc_on_view->proc_errno = EADDRNORIGHT;
		return	0;
	}

	u32	pageOff = (u32)fAddr / PER_PAGE_BYTE;
	u32	nPage = pageBuf[pageOff] >> 1;

	if(!(pageBuf[pageOff] & USED) || !nPage) {
		proc_on_view->proc_errno = EADDRNORIGHT;
		return	0;
	}

	if((nPage * PER_PAGE_BYTE) + HEAP_SIZE >= MEMORY_SIZE) {
		proc_on_view->proc_errno = EMEMOUT;
		return	0;
	}

	for(; nPage > 0; nPage--)
		pageBuf[pageOff + nPage] &= FREE;

	pageBuf[pageOff] = (u16)0;
	pHeap->ns_data.nheap += nPage;

	return	1;
}


/*-----mem_allow_enter_heap-----*/
int mem_allow_enter_heap(void)
{
	if(pHeap->ns_state & FULL) {
		proc_on_view->proc_errno = EMEMFUL;
		return	1;
	}

	if(pHeap->ns_state & COUNTING)
		return	1;

	return	0;
}


/*-----mem_allow_enter_nursery-----*/
int mem_allow_enter_nursery(void)
{
	if(pNursery->ns_state & FULL) {
		proc_on_view->proc_errno = EMEMFUL;
		return	1;
	}

	return	0;
}


/*-----mem_nursery_find_proc-----*/
int mem_nursery_find_proc(int pID, int *pLocate)
{
	int	nCir;

	for(nCir = 0, *pLocate = -1; nCir < NR_FAMILY; nCir++) {
		if(pID == (famStBuf[nCir] >> 1)) {
			*pLocate = nCir;
			return	1;
		}
	}

	return	0;
}
