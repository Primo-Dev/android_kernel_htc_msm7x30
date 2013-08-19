/*
 * Procedures for maintaining information about logical memory blocks.
 *
 * Peter Bergner, IBM Corp.	June 2001.
 * Copyright (C) 2001 Peter Bergner.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/poison.h>
#include <linux/pfn.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/memblock.h>

<<<<<<< HEAD
struct memblock memblock __initdata_memblock;

int memblock_debug __initdata_memblock;
int memblock_can_resize __initdata_memblock;
static struct memblock_region memblock_memory_init_regions[INIT_MEMBLOCK_REGIONS + 1] __initdata_memblock;
static struct memblock_region memblock_reserved_init_regions[INIT_MEMBLOCK_REGIONS + 1] __initdata_memblock;
=======
static struct memblock_region memblock_memory_init_regions[INIT_MEMBLOCK_REGIONS] __initdata_memblock;
static struct memblock_region memblock_reserved_init_regions[INIT_MEMBLOCK_REGIONS] __initdata_memblock;

struct memblock memblock __initdata_memblock = {
	.memory.regions		= memblock_memory_init_regions,
	.memory.cnt		= 1,	/* empty dummy entry */
	.memory.max		= INIT_MEMBLOCK_REGIONS,

	.reserved.regions	= memblock_reserved_init_regions,
	.reserved.cnt		= 1,	/* empty dummy entry */
	.reserved.max		= INIT_MEMBLOCK_REGIONS,

	.current_limit		= MEMBLOCK_ALLOC_ANYWHERE,
};

int memblock_debug __initdata_memblock;
static int memblock_can_resize __initdata_memblock;
>>>>>>> upstream/4.3_primoc

/* inline so we don't get a warning when pr_debug is compiled out */
static inline const char *memblock_type_name(struct memblock_type *type)
{
	if (type == &memblock.memory)
		return "memory";
	else if (type == &memblock.reserved)
		return "reserved";
	else
		return "unknown";
}

/* adjust *@size so that (@base + *@size) doesn't overflow, return new size */
static inline phys_addr_t memblock_cap_size(phys_addr_t base, phys_addr_t *size)
{
	return *size = min(*size, (phys_addr_t)ULLONG_MAX - base);
}

/*
 * Address comparison utilities
 */
<<<<<<< HEAD

static phys_addr_t __init_memblock memblock_align_down(phys_addr_t addr, phys_addr_t size)
{
	return addr & ~(size - 1);
}

static phys_addr_t __init_memblock memblock_align_up(phys_addr_t addr, phys_addr_t size)
{
	return (addr + (size - 1)) & ~(size - 1);
}

=======
>>>>>>> upstream/4.3_primoc
static unsigned long __init_memblock memblock_addrs_overlap(phys_addr_t base1, phys_addr_t size1,
				       phys_addr_t base2, phys_addr_t size2)
{
	return ((base1 < (base2 + size2)) && (base2 < (base1 + size1)));
}

<<<<<<< HEAD
long __init_memblock memblock_overlaps_region(struct memblock_type *type, phys_addr_t base, phys_addr_t size)
=======
static long __init_memblock memblock_overlaps_region(struct memblock_type *type,
					phys_addr_t base, phys_addr_t size)
>>>>>>> upstream/4.3_primoc
{
	unsigned long i;

	for (i = 0; i < type->cnt; i++) {
		phys_addr_t rgnbase = type->regions[i].base;
		phys_addr_t rgnsize = type->regions[i].size;
		if (memblock_addrs_overlap(base, size, rgnbase, rgnsize))
			break;
	}

	return (i < type->cnt) ? i : -1;
}

/*
 * Find, allocate, deallocate or reserve unreserved regions. All allocations
 * are top-down.
 */

static phys_addr_t __init_memblock memblock_find_region(phys_addr_t start, phys_addr_t end,
					  phys_addr_t size, phys_addr_t align)
{
	phys_addr_t base, res_base;
	long j;

	/* In case, huge size is requested */
	if (end < size)
<<<<<<< HEAD
		return MEMBLOCK_ERROR;

	base = memblock_align_down((end - size), align);
=======
		return 0;

	base = round_down(end - size, align);
>>>>>>> upstream/4.3_primoc

	/* Prevent allocations returning 0 as it's also used to
	 * indicate an allocation failure
	 */
	if (start == 0)
		start = PAGE_SIZE;

	while (start <= base) {
		j = memblock_overlaps_region(&memblock.reserved, base, size);
		if (j < 0)
			return base;
		res_base = memblock.reserved.regions[j].base;
		if (res_base < size)
			break;
<<<<<<< HEAD
		base = memblock_align_down(res_base - size, align);
	}

	return MEMBLOCK_ERROR;
}

static phys_addr_t __init_memblock memblock_find_base(phys_addr_t size,
			phys_addr_t align, phys_addr_t start, phys_addr_t end)
=======
		base = round_down(res_base - size, align);
	}

	return 0;
}

/*
 * Find a free area with specified alignment in a specific range.
 */
phys_addr_t __init_memblock memblock_find_in_range(phys_addr_t start, phys_addr_t end,
					phys_addr_t size, phys_addr_t align)
>>>>>>> upstream/4.3_primoc
{
	long i;

	BUG_ON(0 == size);

	/* Pump up max_addr */
	if (end == MEMBLOCK_ALLOC_ACCESSIBLE)
		end = memblock.current_limit;

	/* We do a top-down search, this tends to limit memory
	 * fragmentation by keeping early boot allocs near the
	 * top of memory
	 */
	for (i = memblock.memory.cnt - 1; i >= 0; i--) {
		phys_addr_t memblockbase = memblock.memory.regions[i].base;
		phys_addr_t memblocksize = memblock.memory.regions[i].size;
		phys_addr_t bottom, top, found;

		if (memblocksize < size)
			continue;
		if ((memblockbase + memblocksize) <= start)
			break;
		bottom = max(memblockbase, start);
		top = min(memblockbase + memblocksize, end);
		if (bottom >= top)
			continue;
		found = memblock_find_region(bottom, top, size, align);
<<<<<<< HEAD
		if (found != MEMBLOCK_ERROR)
			return found;
	}
	return MEMBLOCK_ERROR;
}

/*
 * Find a free area with specified alignment in a specific range.
 */
u64 __init_memblock memblock_find_in_range(u64 start, u64 end, u64 size, u64 align)
{
	return memblock_find_base(size, align, start, end);
=======
		if (found)
			return found;
	}
	return 0;
>>>>>>> upstream/4.3_primoc
}

/*
 * Free memblock.reserved.regions
 */
int __init_memblock memblock_free_reserved_regions(void)
{
	if (memblock.reserved.regions == memblock_reserved_init_regions)
		return 0;

	return memblock_free(__pa(memblock.reserved.regions),
		 sizeof(struct memblock_region) * memblock.reserved.max);
}

/*
 * Reserve memblock.reserved.regions
 */
int __init_memblock memblock_reserve_reserved_regions(void)
{
	if (memblock.reserved.regions == memblock_reserved_init_regions)
		return 0;

	return memblock_reserve(__pa(memblock.reserved.regions),
		 sizeof(struct memblock_region) * memblock.reserved.max);
}

static void __init_memblock memblock_remove_region(struct memblock_type *type, unsigned long r)
{
<<<<<<< HEAD
	unsigned long i;

	for (i = r; i < type->cnt - 1; i++) {
		type->regions[i].base = type->regions[i + 1].base;
		type->regions[i].size = type->regions[i + 1].size;
	}
=======
	type->total_size -= type->regions[r].size;
	memmove(&type->regions[r], &type->regions[r + 1],
		(type->cnt - (r + 1)) * sizeof(type->regions[r]));
>>>>>>> upstream/4.3_primoc
	type->cnt--;

	/* Special case for empty arrays */
	if (type->cnt == 0) {
<<<<<<< HEAD
		type->cnt = 1;
		type->regions[0].base = 0;
		type->regions[0].size = 0;
	}
}

/* Defined below but needed now */
static long memblock_add_region(struct memblock_type *type, phys_addr_t base, phys_addr_t size);

=======
		WARN_ON(type->total_size != 0);
		type->cnt = 1;
		type->regions[0].base = 0;
		type->regions[0].size = 0;
		memblock_set_region_node(&type->regions[0], MAX_NUMNODES);
	}
}

>>>>>>> upstream/4.3_primoc
static int __init_memblock memblock_double_array(struct memblock_type *type)
{
	struct memblock_region *new_array, *old_array;
	phys_addr_t old_size, new_size, addr;
	int use_slab = slab_is_available();

	/* We don't allow resizing until we know about the reserved regions
	 * of memory that aren't suitable for allocation
	 */
	if (!memblock_can_resize)
		return -1;

	/* Calculate new doubled size */
	old_size = type->max * sizeof(struct memblock_region);
	new_size = old_size << 1;

	/* Try to find some space for it.
	 *
	 * WARNING: We assume that either slab_is_available() and we use it or
	 * we use MEMBLOCK for allocations. That means that this is unsafe to use
	 * when bootmem is currently active (unless bootmem itself is implemented
	 * on top of MEMBLOCK which isn't the case yet)
	 *
	 * This should however not be an issue for now, as we currently only
	 * call into MEMBLOCK while it's still active, or much later when slab is
	 * active for memory hotplug operations
	 */
	if (use_slab) {
		new_array = kmalloc(new_size, GFP_KERNEL);
<<<<<<< HEAD
		addr = new_array == NULL ? MEMBLOCK_ERROR : __pa(new_array);
	} else
		addr = memblock_find_base(new_size, sizeof(phys_addr_t), 0, MEMBLOCK_ALLOC_ACCESSIBLE);
	if (addr == MEMBLOCK_ERROR) {
=======
		addr = new_array ? __pa(new_array) : 0;
	} else
		addr = memblock_find_in_range(0, MEMBLOCK_ALLOC_ACCESSIBLE, new_size, sizeof(phys_addr_t));
	if (!addr) {
>>>>>>> upstream/4.3_primoc
		pr_err("memblock: Failed to double %s array from %ld to %ld entries !\n",
		       memblock_type_name(type), type->max, type->max * 2);
		return -1;
	}
	new_array = __va(addr);

	memblock_dbg("memblock: %s array is doubled to %ld at [%#010llx-%#010llx]",
		 memblock_type_name(type), type->max * 2, (u64)addr, (u64)addr + new_size - 1);

	/* Found space, we now need to move the array over before
	 * we add the reserved region since it may be our reserved
	 * array itself that is full.
	 */
	memcpy(new_array, type->regions, old_size);
	memset(new_array + type->max, 0, old_size);
	old_array = type->regions;
	type->regions = new_array;
	type->max <<= 1;

	/* If we use SLAB that's it, we are done */
	if (use_slab)
		return 0;

	/* Add the new reserved region now. Should not fail ! */
<<<<<<< HEAD
	BUG_ON(memblock_add_region(&memblock.reserved, addr, new_size));
=======
	BUG_ON(memblock_reserve(addr, new_size));
>>>>>>> upstream/4.3_primoc

	/* If the array wasn't our static init one, then free it. We only do
	 * that before SLAB is available as later on, we don't know whether
	 * to use kfree or free_bootmem_pages(). Shouldn't be a big deal
	 * anyways
	 */
	if (old_array != memblock_memory_init_regions &&
	    old_array != memblock_reserved_init_regions)
		memblock_free(__pa(old_array), old_size);

	return 0;
}

<<<<<<< HEAD
extern int __init_memblock __weak memblock_memory_can_coalesce(phys_addr_t addr1, phys_addr_t size1,
					  phys_addr_t addr2, phys_addr_t size2)
{
	return 1;
}

static long __init_memblock memblock_add_region(struct memblock_type *type,
						phys_addr_t base, phys_addr_t size)
{
	phys_addr_t end = base + memblock_cap_size(base, &size);
	int i, slot = -1;

	/* First try and coalesce this MEMBLOCK with others */
	for (i = 0; i < type->cnt; i++) {
		struct memblock_region *rgn = &type->regions[i];
		phys_addr_t rend = rgn->base + rgn->size;

		/* Exit if there's no possible hits */
		if (rgn->base > end || rgn->size == 0)
			break;

		/* Check if we are fully enclosed within an existing
		 * block
		 */
		if (rgn->base <= base && rend >= end)
			return 0;

		/* Check if we overlap or are adjacent with the bottom
		 * of a block.
		 */
		if (base < rgn->base && end >= rgn->base) {
			/* If we can't coalesce, create a new block */
			if (!memblock_memory_can_coalesce(base, size,
							  rgn->base,
							  rgn->size)) {
				/* Overlap & can't coalesce are mutually
				 * exclusive, if you do that, be prepared
				 * for trouble
				 */
				WARN_ON(end != rgn->base);
				goto new_block;
			}
			/* We extend the bottom of the block down to our
			 * base
			 */
			rgn->base = base;
			rgn->size = rend - base;

			/* Return if we have nothing else to allocate
			 * (fully coalesced)
			 */
			if (rend >= end)
				return 0;

			/* We continue processing from the end of the
			 * coalesced block.
			 */
			base = rend;
			size = end - base;
		}

		/* Now check if we overlap or are adjacent with the
		 * top of a block
		 */
		if (base <= rend && end >= rend) {
			/* If we can't coalesce, create a new block */
			if (!memblock_memory_can_coalesce(rgn->base,
							  rgn->size,
							  base, size)) {
				/* Overlap & can't coalesce are mutually
				 * exclusive, if you do that, be prepared
				 * for trouble
				 */
				WARN_ON(rend != base);
				goto new_block;
			}
			/* We adjust our base down to enclose the
			 * original block and destroy it. It will be
			 * part of our new allocation. Since we've
			 * freed an entry, we know we won't fail
			 * to allocate one later, so we won't risk
			 * losing the original block allocation.
			 */
			size += (base - rgn->base);
			base = rgn->base;
			memblock_remove_region(type, i--);
		}
	}

	/* If the array is empty, special case, replace the fake
	 * filler region and return
	 */
	if ((type->cnt == 1) && (type->regions[0].size == 0)) {
		type->regions[0].base = base;
		type->regions[0].size = size;
		return 0;
	}

 new_block:
	/* If we are out of space, we fail. It's too late to resize the array
	 * but then this shouldn't have happened in the first place.
	 */
	if (WARN_ON(type->cnt >= type->max))
		return -1;

	/* Couldn't coalesce the MEMBLOCK, so add it to the sorted table. */
	for (i = type->cnt - 1; i >= 0; i--) {
		if (base < type->regions[i].base) {
			type->regions[i+1].base = type->regions[i].base;
			type->regions[i+1].size = type->regions[i].size;
		} else {
			type->regions[i+1].base = base;
			type->regions[i+1].size = size;
			slot = i + 1;
			break;
		}
	}
	if (base < type->regions[0].base) {
		type->regions[0].base = base;
		type->regions[0].size = size;
		slot = 0;
	}
	type->cnt++;

	/* The array is full ? Try to resize it. If that fails, we undo
	 * our allocation and return an error
	 */
	if (type->cnt == type->max && memblock_double_array(type)) {
		BUG_ON(slot < 0);
		memblock_remove_region(type, slot);
		return -1;
	}

	return 0;
}

long __init_memblock memblock_add(phys_addr_t base, phys_addr_t size)
{
	return memblock_add_region(&memblock.memory, base, size);

}

static long __init_memblock __memblock_remove(struct memblock_type *type,
					      phys_addr_t base, phys_addr_t size)
=======
/**
 * memblock_merge_regions - merge neighboring compatible regions
 * @type: memblock type to scan
 *
 * Scan @type and merge neighboring compatible regions.
 */
static void __init_memblock memblock_merge_regions(struct memblock_type *type)
{
	int i = 0;

	/* cnt never goes below 1 */
	while (i < type->cnt - 1) {
		struct memblock_region *this = &type->regions[i];
		struct memblock_region *next = &type->regions[i + 1];

		if (this->base + this->size != next->base ||
		    memblock_get_region_node(this) !=
		    memblock_get_region_node(next)) {
			BUG_ON(this->base + this->size > next->base);
			i++;
			continue;
		}

		this->size += next->size;
		memmove(next, next + 1, (type->cnt - (i + 1)) * sizeof(*next));
		type->cnt--;
	}
}

/**
 * memblock_insert_region - insert new memblock region
 * @type: memblock type to insert into
 * @idx: index for the insertion point
 * @base: base address of the new region
 * @size: size of the new region
 *
 * Insert new memblock region [@base,@base+@size) into @type at @idx.
 * @type must already have extra room to accomodate the new region.
 */
static void __init_memblock memblock_insert_region(struct memblock_type *type,
						   int idx, phys_addr_t base,
						   phys_addr_t size, int nid)
{
	struct memblock_region *rgn = &type->regions[idx];

	BUG_ON(type->cnt >= type->max);
	memmove(rgn + 1, rgn, (type->cnt - idx) * sizeof(*rgn));
	rgn->base = base;
	rgn->size = size;
	memblock_set_region_node(rgn, nid);
	type->cnt++;
	type->total_size += size;
}

/**
 * memblock_add_region - add new memblock region
 * @type: memblock type to add new region into
 * @base: base address of the new region
 * @size: size of the new region
 * @nid: nid of the new region
 *
 * Add new memblock region [@base,@base+@size) into @type.  The new region
 * is allowed to overlap with existing ones - overlaps don't affect already
 * existing regions.  @type is guaranteed to be minimal (all neighbouring
 * compatible regions are merged) after the addition.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
static int __init_memblock memblock_add_region(struct memblock_type *type,
				phys_addr_t base, phys_addr_t size, int nid)
{
	bool insert = false;
	phys_addr_t obase = base;
	phys_addr_t end = base + memblock_cap_size(base, &size);
	int i, nr_new;

	/* special case for empty array */
	if (type->regions[0].size == 0) {
		WARN_ON(type->cnt != 1 || type->total_size);
		type->regions[0].base = base;
		type->regions[0].size = size;
		memblock_set_region_node(&type->regions[0], nid);
		type->total_size = size;
		return 0;
	}
repeat:
	/*
	 * The following is executed twice.  Once with %false @insert and
	 * then with %true.  The first counts the number of regions needed
	 * to accomodate the new area.  The second actually inserts them.
	 */
	base = obase;
	nr_new = 0;

	for (i = 0; i < type->cnt; i++) {
		struct memblock_region *rgn = &type->regions[i];
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;
		/*
		 * @rgn overlaps.  If it separates the lower part of new
		 * area, insert that portion.
		 */
		if (rbase > base) {
			nr_new++;
			if (insert)
				memblock_insert_region(type, i++, base,
						       rbase - base, nid);
		}
		/* area below @rend is dealt with, forget about it */
		base = min(rend, end);
	}

	/* insert the remaining portion */
	if (base < end) {
		nr_new++;
		if (insert)
			memblock_insert_region(type, i, base, end - base, nid);
	}

	/*
	 * If this was the first round, resize array and repeat for actual
	 * insertions; otherwise, merge and return.
	 */
	if (!insert) {
		while (type->cnt + nr_new > type->max)
			if (memblock_double_array(type) < 0)
				return -ENOMEM;
		insert = true;
		goto repeat;
	} else {
		memblock_merge_regions(type);
		return 0;
	}
}

int __init_memblock memblock_add_node(phys_addr_t base, phys_addr_t size,
				       int nid)
{
	return memblock_add_region(&memblock.memory, base, size, nid);
}

int __init_memblock memblock_add(phys_addr_t base, phys_addr_t size)
{
	return memblock_add_region(&memblock.memory, base, size, MAX_NUMNODES);
}

/**
 * memblock_isolate_range - isolate given range into disjoint memblocks
 * @type: memblock type to isolate range for
 * @base: base of range to isolate
 * @size: size of range to isolate
 * @start_rgn: out parameter for the start of isolated region
 * @end_rgn: out parameter for the end of isolated region
 *
 * Walk @type and ensure that regions don't cross the boundaries defined by
 * [@base,@base+@size).  Crossing regions are split at the boundaries,
 * which may create at most two more regions.  The index of the first
 * region inside the range is returned in *@start_rgn and end in *@end_rgn.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
static int __init_memblock memblock_isolate_range(struct memblock_type *type,
					phys_addr_t base, phys_addr_t size,
					int *start_rgn, int *end_rgn)
>>>>>>> upstream/4.3_primoc
{
	phys_addr_t end = base + memblock_cap_size(base, &size);
	int i;

<<<<<<< HEAD
	/* Walk through the array for collisions */
	for (i = 0; i < type->cnt; i++) {
		struct memblock_region *rgn = &type->regions[i];
		phys_addr_t rend = rgn->base + rgn->size;

		/* Nothing more to do, exit */
		if (rgn->base > end || rgn->size == 0)
			break;

		/* If we fully enclose the block, drop it */
		if (base <= rgn->base && end >= rend) {
			memblock_remove_region(type, i--);
			continue;
		}

		/* If we are fully enclosed within a block
		 * then we need to split it and we are done
		 */
		if (base > rgn->base && end < rend) {
			rgn->size = base - rgn->base;
			if (!memblock_add_region(type, end, rend - end))
				return 0;
			/* Failure to split is bad, we at least
			 * restore the block before erroring
			 */
			rgn->size = rend - rgn->base;
			WARN_ON(1);
			return -1;
		}

		/* Check if we need to trim the bottom of a block */
		if (rgn->base < end && rend > end) {
			rgn->size -= end - rgn->base;
			rgn->base = end;
			break;
		}

		/* And check if we need to trim the top of a block */
		if (base < rend)
			rgn->size -= rend - base;

	}
	return 0;
}

long __init_memblock memblock_remove(phys_addr_t base, phys_addr_t size)
=======
	*start_rgn = *end_rgn = 0;

	/* we'll create at most two more regions */
	while (type->cnt + 2 > type->max)
		if (memblock_double_array(type) < 0)
			return -ENOMEM;

	for (i = 0; i < type->cnt; i++) {
		struct memblock_region *rgn = &type->regions[i];
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;

		if (rbase < base) {
			/*
			 * @rgn intersects from below.  Split and continue
			 * to process the next region - the new top half.
			 */
			rgn->base = base;
			rgn->size -= base - rbase;
			type->total_size -= base - rbase;
			memblock_insert_region(type, i, rbase, base - rbase,
					       memblock_get_region_node(rgn));
		} else if (rend > end) {
			/*
			 * @rgn intersects from above.  Split and redo the
			 * current region - the new bottom half.
			 */
			rgn->base = end;
			rgn->size -= end - rbase;
			type->total_size -= end - rbase;
			memblock_insert_region(type, i--, rbase, end - rbase,
					       memblock_get_region_node(rgn));
		} else {
			/* @rgn is fully contained, record it */
			if (!*end_rgn)
				*start_rgn = i;
			*end_rgn = i + 1;
		}
	}

	return 0;
}

static int __init_memblock __memblock_remove(struct memblock_type *type,
					     phys_addr_t base, phys_addr_t size)
{
	int start_rgn, end_rgn;
	int i, ret;

	ret = memblock_isolate_range(type, base, size, &start_rgn, &end_rgn);
	if (ret)
		return ret;

	for (i = end_rgn - 1; i >= start_rgn; i--)
		memblock_remove_region(type, i);
	return 0;
}

int __init_memblock memblock_remove(phys_addr_t base, phys_addr_t size)
>>>>>>> upstream/4.3_primoc
{
	return __memblock_remove(&memblock.memory, base, size);
}

<<<<<<< HEAD
long __init_memblock memblock_free(phys_addr_t base, phys_addr_t size)
{
	return __memblock_remove(&memblock.reserved, base, size);
}

long __init_memblock memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	struct memblock_type *_rgn = &memblock.reserved;

	BUG_ON(0 == size);

	return memblock_add_region(_rgn, base, size);
}

=======
int __init_memblock memblock_free(phys_addr_t base, phys_addr_t size)
{
	memblock_dbg("   memblock_free: [%#016llx-%#016llx] %pF\n",
		     (unsigned long long)base,
		     (unsigned long long)base + size,
		     (void *)_RET_IP_);

	return __memblock_remove(&memblock.reserved, base, size);
}

int __init_memblock memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	struct memblock_type *_rgn = &memblock.reserved;

	memblock_dbg("memblock_reserve: [%#016llx-%#016llx] %pF\n",
		     (unsigned long long)base,
		     (unsigned long long)base + size,
		     (void *)_RET_IP_);
	BUG_ON(0 == size);

	return memblock_add_region(_rgn, base, size, MAX_NUMNODES);
}

/**
 * __next_free_mem_range - next function for for_each_free_mem_range()
 * @idx: pointer to u64 loop variable
 * @nid: nid: node selector, %MAX_NUMNODES for all nodes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 *
 * Find the first free area from *@idx which matches @nid, fill the out
 * parameters, and update *@idx for the next iteration.  The lower 32bit of
 * *@idx contains index into memory region and the upper 32bit indexes the
 * areas before each reserved region.  For example, if reserved regions
 * look like the following,
 *
 *	0:[0-16), 1:[32-48), 2:[128-130)
 *
 * The upper 32bit indexes the following regions.
 *
 *	0:[0-0), 1:[16-32), 2:[48-128), 3:[130-MAX)
 *
 * As both region arrays are sorted, the function advances the two indices
 * in lockstep and returns each intersection.
 */
void __init_memblock __next_free_mem_range(u64 *idx, int nid,
					   phys_addr_t *out_start,
					   phys_addr_t *out_end, int *out_nid)
{
	struct memblock_type *mem = &memblock.memory;
	struct memblock_type *rsv = &memblock.reserved;
	unsigned int mi = *idx & 0xffffffff;
	unsigned int ri = *idx >> 32;

	for ( ; mi < mem->cnt; mi++) {
		struct memblock_region *m = &mem->regions[mi];
		phys_addr_t m_start = m->base;
		phys_addr_t m_end = m->base + m->size;

		/* only memory regions are associated with nodes, check it */
		if (nid != MAX_NUMNODES && nid != memblock_get_region_node(m))
			continue;

		/* scan areas before each reservation for intersection */
		for ( ; ri < rsv->cnt + 1; ri++) {
			struct memblock_region *r = &rsv->regions[ri];
			phys_addr_t r_start = ri ? r[-1].base + r[-1].size : 0;
			phys_addr_t r_end = ri < rsv->cnt ? r->base : ULLONG_MAX;

			/* if ri advanced past mi, break out to advance mi */
			if (r_start >= m_end)
				break;
			/* if the two regions intersect, we're done */
			if (m_start < r_end) {
				if (out_start)
					*out_start = max(m_start, r_start);
				if (out_end)
					*out_end = min(m_end, r_end);
				if (out_nid)
					*out_nid = memblock_get_region_node(m);
				/*
				 * The region which ends first is advanced
				 * for the next iteration.
				 */
				if (m_end <= r_end)
					mi++;
				else
					ri++;
				*idx = (u32)mi | (u64)ri << 32;
				return;
			}
		}
	}

	/* signal end of iteration */
	*idx = ULLONG_MAX;
}

#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
/*
 * Common iterator interface used to define for_each_mem_range().
 */
void __init_memblock __next_mem_pfn_range(int *idx, int nid,
				unsigned long *out_start_pfn,
				unsigned long *out_end_pfn, int *out_nid)
{
	struct memblock_type *type = &memblock.memory;
	struct memblock_region *r;

	while (++*idx < type->cnt) {
		r = &type->regions[*idx];

		if (PFN_UP(r->base) >= PFN_DOWN(r->base + r->size))
			continue;
		if (nid == MAX_NUMNODES || nid == r->nid)
			break;
	}
	if (*idx >= type->cnt) {
		*idx = -1;
		return;
	}

	if (out_start_pfn)
		*out_start_pfn = PFN_UP(r->base);
	if (out_end_pfn)
		*out_end_pfn = PFN_DOWN(r->base + r->size);
	if (out_nid)
		*out_nid = r->nid;
}

/**
 * memblock_set_node - set node ID on memblock regions
 * @base: base of area to set node ID for
 * @size: size of area to set node ID for
 * @nid: node ID to set
 *
 * Set the nid of memblock memory regions in [@base,@base+@size) to @nid.
 * Regions which cross the area boundaries are split as necessary.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
int __init_memblock memblock_set_node(phys_addr_t base, phys_addr_t size,
				      int nid)
{
	struct memblock_type *type = &memblock.memory;
	int start_rgn, end_rgn;
	int i, ret;

	ret = memblock_isolate_range(type, base, size, &start_rgn, &end_rgn);
	if (ret)
		return ret;

	for (i = start_rgn; i < end_rgn; i++)
		type->regions[i].nid = nid;

	memblock_merge_regions(type);
	return 0;
}
#endif /* CONFIG_HAVE_MEMBLOCK_NODE_MAP */

>>>>>>> upstream/4.3_primoc
phys_addr_t __init __memblock_alloc_base(phys_addr_t size, phys_addr_t align, phys_addr_t max_addr)
{
	phys_addr_t found;

	/* We align the size to limit fragmentation. Without this, a lot of
	 * small allocs quickly eat up the whole reserve array on sparc
	 */
<<<<<<< HEAD
	size = memblock_align_up(size, align);

	found = memblock_find_base(size, align, 0, max_addr);
	if (found != MEMBLOCK_ERROR &&
	    !memblock_add_region(&memblock.reserved, found, size))
=======
	size = round_up(size, align);

	found = memblock_find_in_range(0, max_addr, size, align);
	if (found && !memblock_reserve(found, size))
>>>>>>> upstream/4.3_primoc
		return found;

	return 0;
}

phys_addr_t __init memblock_alloc_base(phys_addr_t size, phys_addr_t align, phys_addr_t max_addr)
{
	phys_addr_t alloc;

	alloc = __memblock_alloc_base(size, align, max_addr);

	if (alloc == 0)
		panic("ERROR: Failed to allocate 0x%llx bytes below 0x%llx.\n",
		      (unsigned long long) size, (unsigned long long) max_addr);

	return alloc;
}

phys_addr_t __init memblock_alloc(phys_addr_t size, phys_addr_t align)
{
	return memblock_alloc_base(size, align, MEMBLOCK_ALLOC_ACCESSIBLE);
}


/*
<<<<<<< HEAD
 * Additional node-local allocators. Search for node memory is bottom up
 * and walks memblock regions within that node bottom-up as well, but allocation
 * within an memblock region is top-down. XXX I plan to fix that at some stage
=======
 * Additional node-local top-down allocators.
>>>>>>> upstream/4.3_primoc
 *
 * WARNING: Only available after early_node_map[] has been populated,
 * on some architectures, that is after all the calls to add_active_range()
 * have been done to populate it.
 */

<<<<<<< HEAD
phys_addr_t __weak __init memblock_nid_range(phys_addr_t start, phys_addr_t end, int *nid)
{
#ifdef CONFIG_ARCH_POPULATES_NODE_MAP
	/*
	 * This code originates from sparc which really wants use to walk by addresses
	 * and returns the nid. This is not very convenient for early_pfn_map[] users
	 * as the map isn't sorted yet, and it really wants to be walked by nid.
	 *
	 * For now, I implement the inefficient method below which walks the early
	 * map multiple times. Eventually we may want to use an ARCH config option
	 * to implement a completely different method for both case.
	 */
	unsigned long start_pfn, end_pfn;
	int i;

	for (i = 0; i < MAX_NUMNODES; i++) {
		get_pfn_range_for_nid(i, &start_pfn, &end_pfn);
		if (start < PFN_PHYS(start_pfn) || start >= PFN_PHYS(end_pfn))
			continue;
		*nid = i;
		return min(end, PFN_PHYS(end_pfn));
	}
#endif
	*nid = 0;

	return end;
}

static phys_addr_t __init memblock_alloc_nid_region(struct memblock_region *mp,
					       phys_addr_t size,
					       phys_addr_t align, int nid)
{
	phys_addr_t start, end;

	start = mp->base;
	end = start + mp->size;

	start = memblock_align_up(start, align);
	while (start < end) {
		phys_addr_t this_end;
		int this_nid;

		this_end = memblock_nid_range(start, end, &this_nid);
		if (this_nid == nid) {
			phys_addr_t ret = memblock_find_region(start, this_end, size, align);
			if (ret != MEMBLOCK_ERROR &&
			    !memblock_add_region(&memblock.reserved, ret, size))
				return ret;
		}
		start = this_end;
	}

	return MEMBLOCK_ERROR;
=======
static phys_addr_t __init memblock_nid_range_rev(phys_addr_t start,
						 phys_addr_t end, int *nid)
{
#ifdef CONFIG_ARCH_POPULATES_NODE_MAP
	unsigned long start_pfn, end_pfn;
	int i;

	for_each_mem_pfn_range(i, MAX_NUMNODES, &start_pfn, &end_pfn, nid)
		if (end > PFN_PHYS(start_pfn) && end <= PFN_PHYS(end_pfn))
			return max(start, PFN_PHYS(start_pfn));
#endif
	*nid = 0;
	return start;
}

phys_addr_t __init memblock_find_in_range_node(phys_addr_t start,
					       phys_addr_t end,
					       phys_addr_t size,
					       phys_addr_t align, int nid)
{
	struct memblock_type *mem = &memblock.memory;
	int i;

	BUG_ON(0 == size);

	/* Pump up max_addr */
	if (end == MEMBLOCK_ALLOC_ACCESSIBLE)
		end = memblock.current_limit;

	for (i = mem->cnt - 1; i >= 0; i--) {
		struct memblock_region *r = &mem->regions[i];
		phys_addr_t base = max(start, r->base);
		phys_addr_t top = min(end, r->base + r->size);

		while (base < top) {
			phys_addr_t tbase, ret;
			int tnid;

			tbase = memblock_nid_range_rev(base, top, &tnid);
			if (nid == MAX_NUMNODES || tnid == nid) {
				ret = memblock_find_region(tbase, top, size, align);
				if (ret)
					return ret;
			}
			top = tbase;
		}
	}

	return 0;
>>>>>>> upstream/4.3_primoc
}

phys_addr_t __init memblock_alloc_nid(phys_addr_t size, phys_addr_t align, int nid)
{
<<<<<<< HEAD
	struct memblock_type *mem = &memblock.memory;
	int i;

	BUG_ON(0 == size);

	/* We align the size to limit fragmentation. Without this, a lot of
	 * small allocs quickly eat up the whole reserve array on sparc
	 */
	size = memblock_align_up(size, align);

	/* We do a bottom-up search for a region with the right
	 * nid since that's easier considering how memblock_nid_range()
	 * works
	 */
	for (i = 0; i < mem->cnt; i++) {
		phys_addr_t ret = memblock_alloc_nid_region(&mem->regions[i],
					       size, align, nid);
		if (ret != MEMBLOCK_ERROR)
			return ret;
	}
=======
	phys_addr_t found;

	/*
	 * We align the size to limit fragmentation. Without this, a lot of
	 * small allocs quickly eat up the whole reserve array on sparc
	 */
	size = round_up(size, align);

	found = memblock_find_in_range_node(0, MEMBLOCK_ALLOC_ACCESSIBLE,
					    size, align, nid);
	if (found && !memblock_reserve(found, size))
		return found;
>>>>>>> upstream/4.3_primoc

	return 0;
}

phys_addr_t __init memblock_alloc_try_nid(phys_addr_t size, phys_addr_t align, int nid)
{
	phys_addr_t res = memblock_alloc_nid(size, align, nid);

	if (res)
		return res;
<<<<<<< HEAD
	return memblock_alloc_base(size, align, MEMBLOCK_ALLOC_ANYWHERE);
=======
	return memblock_alloc_base(size, align, MEMBLOCK_ALLOC_ACCESSIBLE);
>>>>>>> upstream/4.3_primoc
}


/*
 * Remaining API functions
 */

<<<<<<< HEAD
/* You must call memblock_analyze() before this. */
phys_addr_t __init memblock_phys_mem_size(void)
{
	return memblock.memory_size;
=======
phys_addr_t __init memblock_phys_mem_size(void)
{
	return memblock.memory.total_size;
}

/* lowest address */
phys_addr_t __init_memblock memblock_start_of_DRAM(void)
{
	return memblock.memory.regions[0].base;
>>>>>>> upstream/4.3_primoc
}

phys_addr_t __init_memblock memblock_end_of_DRAM(void)
{
	int idx = memblock.memory.cnt - 1;

	return (memblock.memory.regions[idx].base + memblock.memory.regions[idx].size);
}

<<<<<<< HEAD
/* You must call memblock_analyze() after this. */
void __init memblock_enforce_memory_limit(phys_addr_t memory_limit)
{
	unsigned long i;
	phys_addr_t limit;
	struct memblock_region *p;

	if (!memory_limit)
		return;

	/* Truncate the memblock regions to satisfy the memory limit. */
	limit = memory_limit;
	for (i = 0; i < memblock.memory.cnt; i++) {
		if (limit > memblock.memory.regions[i].size) {
			limit -= memblock.memory.regions[i].size;
			continue;
		}

		memblock.memory.regions[i].size = limit;
		memblock.memory.cnt = i + 1;
		break;
	}

	memory_limit = memblock_end_of_DRAM();

	/* And truncate any reserves above the limit also. */
	for (i = 0; i < memblock.reserved.cnt; i++) {
		p = &memblock.reserved.regions[i];

		if (p->base > memory_limit)
			p->size = 0;
		else if ((p->base + p->size) > memory_limit)
			p->size = memory_limit - p->base;

		if (p->size == 0) {
			memblock_remove_region(&memblock.reserved, i);
			i--;
		}
	}
=======
void __init memblock_enforce_memory_limit(phys_addr_t limit)
{
	unsigned long i;
	phys_addr_t max_addr = (phys_addr_t)ULLONG_MAX;

	if (!limit)
		return;

	/* find out max address */
	for (i = 0; i < memblock.memory.cnt; i++) {
		struct memblock_region *r = &memblock.memory.regions[i];

		if (limit <= r->size) {
			max_addr = r->base + limit;
			break;
		}
		limit -= r->size;
	}

	/* truncate both memory and reserved regions */
	__memblock_remove(&memblock.memory, max_addr, (phys_addr_t)ULLONG_MAX);
	__memblock_remove(&memblock.reserved, max_addr, (phys_addr_t)ULLONG_MAX);
>>>>>>> upstream/4.3_primoc
}

static int __init_memblock memblock_search(struct memblock_type *type, phys_addr_t addr)
{
	unsigned int left = 0, right = type->cnt;

	do {
		unsigned int mid = (right + left) / 2;

		if (addr < type->regions[mid].base)
			right = mid;
		else if (addr >= (type->regions[mid].base +
				  type->regions[mid].size))
			left = mid + 1;
		else
			return mid;
	} while (left < right);
	return -1;
}

int __init memblock_is_reserved(phys_addr_t addr)
{
	return memblock_search(&memblock.reserved, addr) != -1;
}

int __init_memblock memblock_is_memory(phys_addr_t addr)
{
<<<<<<< HEAD
=======

	if (unlikely(addr < memblock_start_of_DRAM() ||
		addr >= memblock_end_of_DRAM()))
		return 0;

>>>>>>> upstream/4.3_primoc
	return memblock_search(&memblock.memory, addr) != -1;
}

int __init_memblock memblock_is_region_memory(phys_addr_t base, phys_addr_t size)
{
	int idx = memblock_search(&memblock.memory, base);
	phys_addr_t end = base + memblock_cap_size(base, &size);

	if (idx == -1)
		return 0;
	return memblock.memory.regions[idx].base <= base &&
		(memblock.memory.regions[idx].base +
		 memblock.memory.regions[idx].size) >= end;
}

int __init_memblock memblock_is_region_reserved(phys_addr_t base, phys_addr_t size)
{
	memblock_cap_size(base, &size);
	return memblock_overlaps_region(&memblock.reserved, base, size) >= 0;
}


void __init_memblock memblock_set_current_limit(phys_addr_t limit)
{
	memblock.current_limit = limit;
}

<<<<<<< HEAD
static void __init_memblock memblock_dump(struct memblock_type *region, char *name)
=======
static void __init_memblock memblock_dump(struct memblock_type *type, char *name)
>>>>>>> upstream/4.3_primoc
{
	unsigned long long base, size;
	int i;

<<<<<<< HEAD
	pr_info(" %s.cnt  = 0x%lx\n", name, region->cnt);

	for (i = 0; i < region->cnt; i++) {
		base = region->regions[i].base;
		size = region->regions[i].size;

		pr_info(" %s[%#x]\t[%#016llx-%#016llx], %#llx bytes\n",
		    name, i, base, base + size - 1, size);
	}
}

void __init_memblock memblock_dump_all(void)
{
	if (!memblock_debug)
		return;

	pr_info("MEMBLOCK configuration:\n");
	pr_info(" memory size = 0x%llx\n", (unsigned long long)memblock.memory_size);
=======
	pr_info(" %s.cnt  = 0x%lx\n", name, type->cnt);

	for (i = 0; i < type->cnt; i++) {
		struct memblock_region *rgn = &type->regions[i];
		char nid_buf[32] = "";

		base = rgn->base;
		size = rgn->size;
#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
		if (memblock_get_region_node(rgn) != MAX_NUMNODES)
			snprintf(nid_buf, sizeof(nid_buf), " on node %d",
				 memblock_get_region_node(rgn));
#endif
		pr_info(" %s[%#x]\t[%#016llx-%#016llx], %#llx bytes%s\n",
			name, i, base, base + size - 1, size, nid_buf);
	}
}

void __init_memblock __memblock_dump_all(void)
{
	pr_info("MEMBLOCK configuration:\n");
	pr_info(" memory size = %#llx reserved size = %#llx\n",
		(unsigned long long)memblock.memory.total_size,
		(unsigned long long)memblock.reserved.total_size);
>>>>>>> upstream/4.3_primoc

	memblock_dump(&memblock.memory, "memory");
	memblock_dump(&memblock.reserved, "reserved");
}

<<<<<<< HEAD
void __init memblock_analyze(void)
{
	int i;

	/* Check marker in the unused last array entry */
	WARN_ON(memblock_memory_init_regions[INIT_MEMBLOCK_REGIONS].base
		!= (phys_addr_t)RED_INACTIVE);
	WARN_ON(memblock_reserved_init_regions[INIT_MEMBLOCK_REGIONS].base
		!= (phys_addr_t)RED_INACTIVE);

	memblock.memory_size = 0;

	for (i = 0; i < memblock.memory.cnt; i++)
		memblock.memory_size += memblock.memory.regions[i].size;

	/* We allow resizing from there */
	memblock_can_resize = 1;
}

void __init memblock_init(void)
{
	static int init_done __initdata = 0;

	if (init_done)
		return;
	init_done = 1;

	/* Hookup the initial arrays */
	memblock.memory.regions	= memblock_memory_init_regions;
	memblock.memory.max		= INIT_MEMBLOCK_REGIONS;
	memblock.reserved.regions	= memblock_reserved_init_regions;
	memblock.reserved.max	= INIT_MEMBLOCK_REGIONS;

	/* Write a marker in the unused last array entry */
	memblock.memory.regions[INIT_MEMBLOCK_REGIONS].base = (phys_addr_t)RED_INACTIVE;
	memblock.reserved.regions[INIT_MEMBLOCK_REGIONS].base = (phys_addr_t)RED_INACTIVE;

	/* Create a dummy zero size MEMBLOCK which will get coalesced away later.
	 * This simplifies the memblock_add() code below...
	 */
	memblock.memory.regions[0].base = 0;
	memblock.memory.regions[0].size = 0;
	memblock.memory.cnt = 1;

	/* Ditto. */
	memblock.reserved.regions[0].base = 0;
	memblock.reserved.regions[0].size = 0;
	memblock.reserved.cnt = 1;

	memblock.current_limit = MEMBLOCK_ALLOC_ANYWHERE;
}

=======
void __init memblock_allow_resize(void)
{
	memblock_can_resize = 1;
}

>>>>>>> upstream/4.3_primoc
static int __init early_memblock(char *p)
{
	if (p && strstr(p, "debug"))
		memblock_debug = 1;
	return 0;
}
early_param("memblock", early_memblock);

<<<<<<< HEAD
#if defined(CONFIG_DEBUG_FS) && !defined(ARCH_DISCARD_MEMBLOCK)
=======
#if defined(CONFIG_DEBUG_FS) && !defined(CONFIG_ARCH_DISCARD_MEMBLOCK)
>>>>>>> upstream/4.3_primoc

static int memblock_debug_show(struct seq_file *m, void *private)
{
	struct memblock_type *type = m->private;
	struct memblock_region *reg;
	int i;

	for (i = 0; i < type->cnt; i++) {
		reg = &type->regions[i];
		seq_printf(m, "%4d: ", i);
		if (sizeof(phys_addr_t) == 4)
			seq_printf(m, "0x%08lx..0x%08lx\n",
				   (unsigned long)reg->base,
				   (unsigned long)(reg->base + reg->size - 1));
		else
			seq_printf(m, "0x%016llx..0x%016llx\n",
				   (unsigned long long)reg->base,
				   (unsigned long long)(reg->base + reg->size - 1));

	}
	return 0;
}

static int memblock_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, memblock_debug_show, inode->i_private);
}

static const struct file_operations memblock_debug_fops = {
	.open = memblock_debug_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init memblock_init_debugfs(void)
{
	struct dentry *root = debugfs_create_dir("memblock", NULL);
	if (!root)
		return -ENXIO;
	debugfs_create_file("memory", S_IRUGO, root, &memblock.memory, &memblock_debug_fops);
	debugfs_create_file("reserved", S_IRUGO, root, &memblock.reserved, &memblock_debug_fops);

	return 0;
}
__initcall(memblock_init_debugfs);

#endif /* CONFIG_DEBUG_FS */
