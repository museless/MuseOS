#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"


/*-----access_mutex_lock-----*/
int access_mutex_lock(QUEUE *queue)
{
	int	procOff = proc_getpid();

	close_int();

	if(queue->buf_count > 0) {
		if(queue->buf_count == NR_IMBUF) {
			*queue->buf_head = procOff;
		} else {
			queue->buf_tail = (queue->buf_tail == queue->buf) ? &queue->buf[NR_IMBUF] : queue->buf_tail - 1;
			*queue->buf_tail = procOff;
			block(procOff, NONPROC, BLOCK_ONE);
		}

		queue->buf_count--;

	} else {
		printx(MAGIC_KERN, "queue is full: %d", queue->buf_count);
	}

	open_int();

	return	0;
}


/*-----access_mutex_unlock-----*/
int access_mutex_unlock(QUEUE *queue)
{
	int	procOff;

	close_int();
	*queue->buf_head = 0;

	if(++queue->buf_count != NR_IMBUF) {
		queue->buf_head = (queue->buf_head == queue->buf) ? &queue->buf[NR_IMBUF] : queue->buf_head - 1;
		procOff = *queue->buf_head;
		unblock(procOff);
	}

	open_int();

	return	0;
}
