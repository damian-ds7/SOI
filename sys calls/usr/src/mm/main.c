/* This file contains the main program of the memory manager and some related
 * procedures.  When MINIX starts up, the kernel runs for a little while,
 * initializing itself and its tasks, and then it runs MM and FS.  Both MM
 * and FS initialize themselves as far as they can.  FS then makes a call to
 * MM, because MM has to wait for FS to acquire a RAM disk.  MM asks the
 * kernel for all free memory and starts serving requests.
 *
 * The entry points into this file are:
 *   main:	starts MM running
 *   setreply:	set the reply to be sent to process making an MM system call */

#include "mm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "mproc.h"
#include "param.h"

FORWARD _PROTOTYPE( void get_work, (void)				);
FORWARD _PROTOTYPE( void mm_init, (void)				);

#define click_to_round_k(n) \
	((unsigned) ((((unsigned long) (n) << CLICK_SHIFT) + 512) / 1024))

/*===========================================================================*
 *				main					     *
 *===========================================================================*/
PUBLIC void main()
{
/* Main routine of the memory manager. */

  int result, proc_nr;
  struct mproc *rmp;

  mm_init();			/* initialize memory manager tables */

  /* This is MM's main loop-  get work and do it, forever and forever. */
  while (TRUE) {
	get_work();		/* wait for an MM system call */

	/* If the call number is valid, perform the call. */
	if ((unsigned) mm_call >= NCALLS) {
		result = ENOSYS;
	} else {
		result = (*call_vec[mm_call])();
	}

	/* Send the results back to the user to indicate completion. */
	if (result != E_NO_MESSAGE) setreply(who, result);

	swap_in();		/* maybe a process can be swapped in? */

	/* Send out all pending reply messages, including the answer to
	 * the call just made above.  The processes must not be swapped out.
	 */
	for (proc_nr = 0, rmp = mproc; proc_nr < NR_PROCS; proc_nr++, rmp++) {
		if ((rmp->mp_flags & (REPLY | ONSWAP)) == REPLY) {
			if (send(proc_nr, &rmp->mp_reply) != OK)
				panic("MM can't reply to", proc_nr);
			rmp->mp_flags &= ~REPLY;
		}
	}
  }
}


/*===========================================================================*
 *				get_work				     *
 *===========================================================================*/
PRIVATE void get_work()
{
/* Wait for the next message and extract useful information from it. */

  if (receive(ANY, &mm_in) != OK) panic("MM receive error", NO_NUM);
  who = mm_in.m_source;		/* who sent the message */
  mm_call = mm_in.m_type;	/* system call number */

  /* Process slot of caller.  Misuse MM's own process slot for tasks (KSIG?). */
  mp = &mproc[who < 0 ? MM_PROC_NR : who];
}


/*===========================================================================*
 *				setreply				     *
 *===========================================================================*/
PUBLIC void setreply(proc_nr, result)
int proc_nr;			/* process to reply to */
int result;			/* result of the call (usually OK or error #)*/
{
/* Fill in a reply message to be sent later to a user process.  System calls
 * may occasionally fill in other fields, this is only for the main return
 * value, and for setting the "must send reply" flag.
 */

  register struct mproc *rmp = &mproc[proc_nr];

  rmp->reply_res = result;
  rmp->mp_flags |= REPLY;	/* reply pending */

  if (rmp->mp_flags & ONSWAP)
	swap_inqueue(rmp);	/* must swap this process back in */
}


/*===========================================================================*
 *				mm_init					     *
 *===========================================================================*/
PRIVATE void mm_init()
{
/* Initialize the memory manager. */

  static char core_sigs[] = { SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
			SIGEMT, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2 };
  static char ign_sigs[] = { SIGCHLD };
  register int proc_nr;
  register struct mproc *rmp;
  register char *sig_ptr;
  phys_clicks ram_clicks, total_clicks, minix_clicks, free_clicks;
  message mess;
  struct mem_map kernel_map[NR_SEGS];
  int mem;

  /* Build the set of signals which cause core dumps, and the set of signals
   * that are by default ignored.
   */
  sigemptyset(&core_sset);
  for (sig_ptr = core_sigs; sig_ptr < core_sigs+sizeof(core_sigs); sig_ptr++)
	sigaddset(&core_sset, *sig_ptr);
  sigemptyset(&ign_sset);
  for (sig_ptr = ign_sigs; sig_ptr < ign_sigs+sizeof(ign_sigs); sig_ptr++)
	sigaddset(&ign_sset, *sig_ptr);

  /* Get the memory map of the kernel to see how much memory it uses. */
  sys_getmap(SYSTASK, kernel_map);
  minix_clicks = (kernel_map[S].mem_phys + kernel_map[S].mem_len)
				- kernel_map[T].mem_phys;

  /* Initialize MM's tables. */
  for (proc_nr = 0; proc_nr <= INIT_PROC_NR; proc_nr++) {
	rmp = &mproc[proc_nr];
	rmp->mp_flags |= IN_USE;
	sys_getmap(proc_nr, rmp->mp_seg);
	if (rmp->mp_seg[T].mem_len != 0) rmp->mp_flags |= SEPARATE;
	minix_clicks += (rmp->mp_seg[S].mem_phys + rmp->mp_seg[S].mem_len)
				- rmp->mp_seg[T].mem_phys;
  }
  mproc[INIT_PROC_NR].mp_pid = INIT_PID;
  sigemptyset(&mproc[INIT_PROC_NR].mp_ignore);
  sigemptyset(&mproc[INIT_PROC_NR].mp_catch);
  procs_in_use = LOW_USER + 1;

  /* Wait for FS to send a message telling the RAM disk size then go "on-line".
   */
  if (receive(FS_PROC_NR, &mess) != OK)
	panic("MM can't obtain RAM disk size from FS", NO_NUM);

  ram_clicks = mess.m1_i1;

  /* Initialize tables to all physical mem. */
  mem_init(&total_clicks, &free_clicks);

  /* Print memory information. */
  printf("\nMemory size = %uK   ", click_to_round_k(total_clicks));
  printf("MINIX = %uK   ", click_to_round_k(minix_clicks));
  printf("RAM disk = %uK   ", click_to_round_k(ram_clicks));
  printf("Available = %uK\n\n", click_to_round_k(free_clicks));

  /* Tell FS to continue. */
  if (send(FS_PROC_NR, &mess) != OK)
	panic("MM can't sync up with FS", NO_NUM);

  /* Tell the memory task where my process table is for the sake of ps(1). */
  if ((mem = open("/dev/ram", O_RDWR)) != -1) {
	ioctl(mem, MIOCSPSINFO, (void *) mproc);
	close(mem);
  }
}

int count_descendants(int parent_idx, int* levels) {
/* if levels is -1 it will check all levels */
    int count = 0;
    int i;

    /* if (*levels == 0) {
        return count;
    } */
    for (i = 0; i < NR_PROCS; i++) {
        if ((i != parent_idx) && (mproc[i].mp_flags & IN_USE) && (mproc[i].mp_parent == parent_idx)) {
            if (*levels != 1) {
                (*levels)--;
                count += count_descendants(i, levels);
                (*levels)++;
            }
            ++count;
        }
    }

    return count;
}

int iterative_count_desc(int parent_idx, int levels) {
    /* if levels is -1 function will count all descendants */
    int j;
    int descendants[NR_PROCS] = {0};   /* indices of each descendant in the mproc table */
    int curr_element = 0;              /* index of currently analyzed process in descendants table */
    int count = 0;
    int level_ends[NR_PROCS] = {0};    /* indices at which each level ends */
    int curr_level = 0;
    int curr_proc;                     /* index of currently analyzed process in the mproc table */


    for (j = 0; j < NR_PROCS; ++j) {
        descendants[j] = -1;
    }

    descendants[0] = parent_idx;
    
    while (descendants[curr_element] != -1 && curr_level != levels) {
        curr_proc = descendants[curr_element];
        for (j = 0; j < NR_PROCS; ++j) {
            if ((j != curr_proc) && (mproc[j].mp_flags & IN_USE) && (mproc[j].mp_parent == curr_proc)) {
                ++count;
                descendants[count] = j;
            }
        }
        if (curr_element == level_ends[curr_level] && curr_level != -1) {
            ++curr_level;
            level_ends[curr_level] = count;
        }
        ++curr_element;
    }

    return count;
}


int do_maxdesc() {
    int max_descendants = 0;
    pid_t max_pid;

    int a = mm_in.m1_i1;
    int b = mm_in.m1_i2;

    int i;
    int count;

    int levels = -1;

    for (i = 0; i < NR_PROCS; i++) {
        if (mproc[i].mp_flags & IN_USE) {
            /* count = count_descendants(i, &levels); */
            count = iterative_count_desc(i, levels);
        }
        if ((count <= b) && (count >= a) && (count > max_descendants)) {
            max_descendants = count;
            max_pid = mproc[i].mp_pid;
        }
    }

    mproc[mm_in.m_source].mp_reply.m1_i1 = max_descendants;
    mproc[mm_in.m_source].mp_reply.m1_i2 = max_pid;

    return OK;
}

int do_nleveldesc() {
    int max_descendants = 0;
    pid_t max_pid;

    int n = mm_in.m1_i1;

    int i;
    int count;

    for (i = 0; i < NR_PROCS; i++) {
        if (mproc[i].mp_flags & IN_USE) {
            /* count = count_descendants(i, &n); */
            count = iterative_count_desc(i, n);
        }
        if (count > max_descendants) {
            max_descendants = count;
            max_pid = mproc[i].mp_pid;
        }
    }
    
    mproc[mm_in.m_source].mp_reply.m1_i1 = max_descendants;
    mproc[mm_in.m_source].mp_reply.m1_i2 = max_pid;

    return OK;
}
