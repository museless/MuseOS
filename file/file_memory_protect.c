#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "nest.h"
#include "proto.h"

/* global data */
static	u32	*pNpage;
static	u32	*pHpage;
static	u32	*pBpage;


/*-----mem_set_nest_protection-----*/
int mem_set_nest_protection(void)
{
	u32	*pPageEnt = (u32 *)PAGE_ENT_BASE;

	pNpage = (u32 *)NUR_PAGE_LOC;
	pHpage = (u32 *)HEAP_PAGE_LOC;
	pBpage = (u32 *)BUFFER_PAGE_LOC;

	set_cr0_wp();

	while(pPageEnt < (u32 *)(NURSERY_OFF / PER_PAGE_BYTE + KERNEL_SIZE)) {
		*pPageEnt++ &= PG_SU_LV_MASK;
	}

	mem_nursery_close();
	mem_heap_close();

	return	1;	
}


/*-----mem_buffer_close-----*/
void mem_buffer_close(void)
{
	*pBpage &= ONLY_READ_MASK;
}


/*-----mem_nursery_close-----*/
void mem_nursery_close(void)
{
	*pNpage &= ONLY_READ_MASK;
}


/*-----mem_heap_close-----*/
void mem_heap_close(void)
{
	*pHpage &= ONLY_READ_MASK;
}


/*-----mem_nursery_allow_write-----*/
int mem_nursery_allow_write(void)
{
	if(proc_on_view->proc_id != pNursery->ns_charge_id) {
		proc_on_view->proc_errno = EACCESS;
		return	0;
	}
	
	*pNpage |= RW_MASK;
	return	1;
}


/*-----mem_heap_allow_write-----*/
int mem_heap_allow_change(void)
{
	if(proc_on_view->proc_id != pHeap->ns_charge_id) {
		proc_on_view->proc_errno = EACCESS;
		return	0;
	}
	
	*pHpage |= RW_MASK;
	return	1;
}
