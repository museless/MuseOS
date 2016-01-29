#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "nest.h"
#include "proto.h"


/*-----mid_worker_job-----*/
void small_worker_job(void)
{
	disp_str("Small worker reported.\n");

	for( ; ; ) {
	}
}
