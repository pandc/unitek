#include <stdlib.h>
#include "bsp.h"
#include "com.h"

#define POOLS	1
#define POOLMAX

//#define NULL	((void *)0)

#define HBUSY	0x4488
#define HFREE	0x1122

#if POOLS == 2
#define HEAP_POOL_BIG_SIZE		0x7400
#define HEAP_POOL_TINY_SIZE		0x1800
#define MAX_TINY_SIZE			200
#define MAX_BIG_SIZE			(HEAP_POOL_BIG_SIZE-sizeof(struct heap_st))
#define TOTAL_HEAP_SIZE			(HEAP_POOL_BIG_SIZE+HEAP_POOL_TINY_SIZE)
#elif POOLS == 1
	#if defined(POOLMAX)
		#pragma section = "MHEAPL"
		#pragma section = "MHEAPH"
		#define BASE_HEAP			NULL
		#define MAX_POOL_SIZE		0
		#define RT_BASE_HEAP		((uint8_t *)__section_begin("MHEAPL"))
		#define RT_MAX_POOL_SIZE	(((uint32_t)__section_end("MHEAPH")) - ((uint32_t)__section_begin("MHEAPL")))
		#define TOTAL_HEAP_SIZE		RT_MAX_POOL_SIZE
	#else
		#include "freertosconfig.h"
		#define BASE_HEAP				heap_pool
		#define HEAP_POOL_SIZE			configTOTAL_HEAP_SIZE
		#define MAX_POOL_SIZE			HEAP_POOL_SIZE
		#define TOTAL_HEAP_SIZE			HEAP_POOL_SIZE
	#endif
#endif

#define MIN_BLOCK_SIZE			4

struct heap_st {
	uint16_t status;
	uint16_t size;
};

#if POOLS == 2
static uint8_t heap_pool_big[HEAP_POOL_BIG_SIZE];
static uint8_t heap_pool_tiny[HEAP_POOL_TINY_SIZE];
#elif POOLS == 1
	#if !defined(POOLMAX)
		static uint8_t heap_pool[HEAP_POOL_SIZE];
	#endif
#endif
static int heap_initdone;

static struct heap_info {
	uint16_t min_maxblock;
	uint16_t min_totsize;
	uint16_t max_blocks;
	uint16_t max_freeblocks;
	uint16_t max_busyblocks;
	uint16_t max_blocksize;
	uint16_t max_reqblock,min_reqblock;
	uint16_t poolsize;
	uint8_t *pool;
	const char *name;
} pools[POOLS] = {
#if POOLS == 2
	{ .pool = heap_pool_tiny, .poolsize = sizeof(heap_pool_tiny), .max_blocksize = MAX_TINY_SIZE,
		.min_maxblock = 0xffff, .min_totsize = 0xffff, .min_reqblock = 0xffff, .name = "Tiny" },
	{ .pool = heap_pool_big, .poolsize = sizeof(heap_pool_big), .max_blocksize = MAX_BIG_SIZE,
		.min_maxblock = 0xffff, .min_totsize = 0xffff, .min_reqblock = 0xffff, .name = "Big" }
#elif POOLS == 1
	{ .pool = BASE_HEAP, .poolsize = MAX_POOL_SIZE, .max_blocksize = MAX_POOL_SIZE,
		.min_maxblock = 0xffff, .min_totsize = 0xffff, .min_reqblock = 0xffff, .name = "Pool" }
#endif
};

static void heap_fail(void)
{
volatile int dummy;

	for (dummy = 0; !dummy; );
}

static void heap_init(void)
{
uint16_t i;

	heap_initdone = 1;
	pools[0].pool = RT_BASE_HEAP;
	pools[0].poolsize = pools[0].max_blocksize = RT_MAX_POOL_SIZE;
	for (i = 0; i < POOLS; i++)
	{
		struct heap_st *hp = (struct heap_st *)pools[i].pool;
		hp->size = pools[i].poolsize - sizeof(struct heap_st);
		hp->status = HFREE;
	}
}

static void heap_check(struct heap_info *pp)
{
struct heap_st *hp;
uint16_t pos,totsize,freeblocks,busyblocks,maxblock;

	if (!heap_initdone)
		heap_init();
	maxblock = 0;
	totsize = freeblocks = busyblocks = 0;
	for (pos = 0; pos < pp->poolsize; pos += sizeof(struct heap_st)+hp->size)
	{
		hp = (struct heap_st *)(pp->pool+pos);
		if (hp->status == HFREE)
		{
			freeblocks++;
			totsize += hp->size;
			if (hp->size > maxblock)
				maxblock = hp->size;
		}
		else if (hp->status == HBUSY)
			busyblocks++;
		else
			heap_fail();
	}
	if (maxblock < pp->min_maxblock)
		pp->min_maxblock = maxblock;
	if (totsize < pp->min_totsize)
		pp->min_totsize = totsize;
	if (freeblocks > pp->max_freeblocks)
		pp->max_freeblocks = freeblocks;
	if (busyblocks > pp->max_busyblocks)
		pp->max_busyblocks = busyblocks;
	pos = freeblocks + busyblocks;
	if (pos > pp->max_blocks)
		pp->max_blocks = pos;
}

//void *heap_malloc(uint16_t size)
void *pvPortMalloc(size_t size)
{
struct heap_st *hp;
uint8_t *ptr;
uint16_t hsize,pos,i;

	if (!size)
		return NULL;

	if (!heap_initdone)
		heap_init();

	pos = size;
	if (size & (MIN_BLOCK_SIZE-1))
		size += MIN_BLOCK_SIZE-(size & (MIN_BLOCK_SIZE-1));
	for (i = 0; i < POOLS; i++)
	{
		if (size > pools[i].max_blocksize)
			continue;
		if (pos < pools[i].min_reqblock)
			pools[i].min_reqblock = pos;
		if (pos > pools[i].max_reqblock)
			pools[i].max_reqblock = pos;
		hsize = pools[i].poolsize;
		ptr = pools[i].pool;
		for (pos = 0; pos < hsize; pos += sizeof(struct heap_st)+hp->size)
		{
			hp = (struct heap_st *)(ptr + pos);
			if ((hp->status != HFREE) && (hp->status != HBUSY))
				heap_fail();
			if ((hp->status == HFREE) && (hp->size >= size))
			{
				// right block
				if ((hp->size - size) >= (sizeof(struct heap_st) + MIN_BLOCK_SIZE))
				{
					// we have to break the currenct block
					struct heap_st *nhp;
					nhp = (struct heap_st *)(ptr + pos + sizeof(struct heap_st) + size);
					nhp->status = HFREE;
					nhp->size = hp->size - (sizeof(struct heap_st) + size);
					hp->size = size;
				}
				hp->status = HBUSY;	// lock the block
				heap_check(pools+i);
				return (void *)(ptr + pos + sizeof(struct heap_st));
			}
		}
		heap_fail();
		for (;;);
	}
	heap_fail();
	for (;;);
}

//void heap_free(void *block)
void vPortFree(void *block)
{
struct heap_st *hp,*nhp;
uint8_t *ptr;
uint16_t hsize,pos,npos,i;

	for (i = 0; i < POOLS; i++)
	{
		if ((block >= pools[i].pool) && (block < (pools[i].pool+pools[i].poolsize)))
		{
			ptr = pools[i].pool;
			hsize = pools[i].poolsize;
			for (pos = 0; pos < hsize; pos += sizeof(struct heap_st)+hp->size)
			{
				hp = (struct heap_st *)(ptr + pos);
				if ((hp->status != HFREE) && (hp->status != HBUSY))
					heap_fail();
				if (block == (ptr+pos+sizeof(struct heap_st)))
				{
					if (hp->status != HBUSY)
						heap_fail();
					break;
				}
			}
			if (pos >= hsize)
				heap_fail();
			hp->status = HFREE;
			hp = (struct heap_st *)ptr;
			pos = 0;
			npos = sizeof(struct heap_st)+hp->size;
			for (; npos < hsize; )
			{
				nhp = (struct heap_st *)(ptr + npos);
				if ((hp->status != HFREE) && (hp->status != HBUSY))
					heap_fail();
				if ((nhp->status != HFREE) && (nhp->status != HBUSY))
					heap_fail();
				if ((hp->status == HFREE) && (nhp->status == HFREE))
				{
					// join the blocks
					nhp->status = 0;
					hp->size += nhp->size + sizeof(struct heap_st);
					npos = pos + sizeof(struct heap_st)+hp->size;
				}
				else
				{
					// move forward
					hp = nhp;
					pos = npos;
					npos += sizeof(struct heap_st)+hp->size;
				}
			}
			heap_check(pools+i);
			break;
		}
	}
}

void heap_status(void)
{
uint16_t pos,totsize,maxblock,i;
uint16_t freeblocks,busyblocks;
struct heap_st *hp;
struct heap_info *pp;

	for (pp = pools,i = 0; i < POOLS; i++,pp++)
	{
		maxblock = freeblocks = busyblocks = totsize = 0;
		for (pos = 0; pos < pp->poolsize; pos += sizeof(struct heap_st)+hp->size)
		{
			hp = (struct heap_st *)(pp->pool+pos);
			if ((hp->status != HFREE) && (hp->status != HBUSY))
				heap_fail();
			if (hp->status == HFREE)
			{
				freeblocks++;
				totsize += hp->size;
				if (hp->size > maxblock)
					maxblock = hp->size;
			}
			else
				busyblocks++;
		}
		COM_Printf("%s: size=%u space=%u maxblock=%u freeblocks=%u busyblocks=%u\r\n",pp->name,TOTAL_HEAP_SIZE,
			totsize,maxblock,freeblocks,busyblocks);
		COM_Printf("   min_maxblock=%u min_totsize=%u max_blocks=%u max_busyblocks=%u max_freeblocks=%u\r\n",
			pp->min_maxblock,pp->min_totsize,pp->max_blocks,pp->max_busyblocks,pp->max_freeblocks);
		COM_Printf("   min_reqblock=%u max_reqblock=%u\r\n",pp->min_reqblock,pp->max_reqblock);
		pp->min_reqblock = 0xffff;
		pp->max_reqblock = 0;
	}
}
