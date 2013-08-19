/* memcontrol.h - Memory Controller
 *
 * Copyright IBM Corporation, 2007
 * Author Balbir Singh <balbir@linux.vnet.ibm.com>
 *
 * Copyright 2007 OpenVZ SWsoft Inc
 * Author: Pavel Emelianov <xemul@openvz.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _LINUX_MEMCONTROL_H
#define _LINUX_MEMCONTROL_H
#include <linux/cgroup.h>
#include <linux/vm_event_item.h>

struct mem_cgroup;
struct page_cgroup;
struct page;
struct mm_struct;

/* Stats that can be updated by kernel. */
enum mem_cgroup_page_stat_item {
	MEMCG_NR_FILE_MAPPED, /* # of pages charged as file rss */
};

<<<<<<< HEAD
extern unsigned long mem_cgroup_isolate_pages(unsigned long nr_to_scan,
					struct list_head *dst,
					unsigned long *scanned, int order,
					isolate_mode_t mode,
					struct zone *z,
					struct mem_cgroup *mem_cont,
					int active, int file);
=======
struct mem_cgroup_reclaim_cookie {
	struct zone *zone;
	int priority;
	unsigned int generation;
};
>>>>>>> upstream/4.3_primoc

#ifdef CONFIG_CGROUP_MEM_RES_CTLR
/*
 * All "charge" functions with gfp_mask should use GFP_KERNEL or
 * (gfp_mask & GFP_RECLAIM_MASK). In current implementatin, memcg doesn't
 * alloc memory but reclaims memory from all available zones. So, "where I want
 * memory from" bits of gfp_mask has no meaning. So any bits of that field is
 * available but adding a rule is better. charge functions' gfp_mask should
 * be set to GFP_KERNEL or gfp_mask & GFP_RECLAIM_MASK for avoiding ambiguous
 * codes.
 * (Of course, if memcg does memory allocation in future, GFP_KERNEL is sane.)
 */

extern int mem_cgroup_newpage_charge(struct page *page, struct mm_struct *mm,
				gfp_t gfp_mask);
/* for swap handling */
extern int mem_cgroup_try_charge_swapin(struct mm_struct *mm,
<<<<<<< HEAD
		struct page *page, gfp_t mask, struct mem_cgroup **ptr);
extern void mem_cgroup_commit_charge_swapin(struct page *page,
					struct mem_cgroup *ptr);
extern void mem_cgroup_cancel_charge_swapin(struct mem_cgroup *ptr);

extern int mem_cgroup_cache_charge(struct page *page, struct mm_struct *mm,
					gfp_t gfp_mask);
extern void mem_cgroup_add_lru_list(struct page *page, enum lru_list lru);
extern void mem_cgroup_del_lru_list(struct page *page, enum lru_list lru);
extern void mem_cgroup_rotate_reclaimable_page(struct page *page);
extern void mem_cgroup_rotate_lru_list(struct page *page, enum lru_list lru);
extern void mem_cgroup_del_lru(struct page *page);
extern void mem_cgroup_move_lists(struct page *page,
				  enum lru_list from, enum lru_list to);
=======
		struct page *page, gfp_t mask, struct mem_cgroup **memcgp);
extern void mem_cgroup_commit_charge_swapin(struct page *page,
					struct mem_cgroup *memcg);
extern void mem_cgroup_cancel_charge_swapin(struct mem_cgroup *memcg);

extern int mem_cgroup_cache_charge(struct page *page, struct mm_struct *mm,
					gfp_t gfp_mask);

struct lruvec *mem_cgroup_zone_lruvec(struct zone *, struct mem_cgroup *);
struct lruvec *mem_cgroup_lru_add_list(struct zone *, struct page *,
				       enum lru_list);
void mem_cgroup_lru_del_list(struct page *, enum lru_list);
void mem_cgroup_lru_del(struct page *);
struct lruvec *mem_cgroup_lru_move_lists(struct zone *, struct page *,
					 enum lru_list, enum lru_list);
>>>>>>> upstream/4.3_primoc

/* For coalescing uncharge for reducing memcg' overhead*/
extern void mem_cgroup_uncharge_start(void);
extern void mem_cgroup_uncharge_end(void);

extern void mem_cgroup_uncharge_page(struct page *page);
extern void mem_cgroup_uncharge_cache_page(struct page *page);
<<<<<<< HEAD
extern int mem_cgroup_shmem_charge_fallback(struct page *page,
			struct mm_struct *mm, gfp_t gfp_mask);

extern void mem_cgroup_out_of_memory(struct mem_cgroup *mem, gfp_t gfp_mask);
int task_in_mem_cgroup(struct task_struct *task, const struct mem_cgroup *mem);
=======

extern void mem_cgroup_out_of_memory(struct mem_cgroup *memcg, gfp_t gfp_mask,
				     int order);
int task_in_mem_cgroup(struct task_struct *task, const struct mem_cgroup *memcg);
>>>>>>> upstream/4.3_primoc

extern struct mem_cgroup *try_get_mem_cgroup_from_page(struct page *page);
extern struct mem_cgroup *mem_cgroup_from_task(struct task_struct *p);
extern struct mem_cgroup *try_get_mem_cgroup_from_mm(struct mm_struct *mm);

static inline
int mm_match_cgroup(const struct mm_struct *mm, const struct mem_cgroup *cgroup)
{
<<<<<<< HEAD
	struct mem_cgroup *mem;
	rcu_read_lock();
	mem = mem_cgroup_from_task(rcu_dereference((mm)->owner));
	rcu_read_unlock();
	return cgroup == mem;
}

extern struct cgroup_subsys_state *mem_cgroup_css(struct mem_cgroup *mem);

extern int
mem_cgroup_prepare_migration(struct page *page,
	struct page *newpage, struct mem_cgroup **ptr, gfp_t gfp_mask);
extern void mem_cgroup_end_migration(struct mem_cgroup *mem,
	struct page *oldpage, struct page *newpage, bool migration_ok);

/*
 * For memory reclaim.
 */
int mem_cgroup_inactive_anon_is_low(struct mem_cgroup *memcg);
int mem_cgroup_inactive_file_is_low(struct mem_cgroup *memcg);
int mem_cgroup_select_victim_node(struct mem_cgroup *memcg);
unsigned long mem_cgroup_zone_nr_lru_pages(struct mem_cgroup *memcg,
						struct zone *zone,
						enum lru_list lru);
=======
	struct mem_cgroup *memcg;
	rcu_read_lock();
	memcg = mem_cgroup_from_task(rcu_dereference((mm)->owner));
	rcu_read_unlock();
	return cgroup == memcg;
}

extern struct cgroup_subsys_state *mem_cgroup_css(struct mem_cgroup *memcg);

extern int
mem_cgroup_prepare_migration(struct page *page,
	struct page *newpage, struct mem_cgroup **memcgp, gfp_t gfp_mask);
extern void mem_cgroup_end_migration(struct mem_cgroup *memcg,
	struct page *oldpage, struct page *newpage, bool migration_ok);

struct mem_cgroup *mem_cgroup_iter(struct mem_cgroup *,
				   struct mem_cgroup *,
				   struct mem_cgroup_reclaim_cookie *);
void mem_cgroup_iter_break(struct mem_cgroup *, struct mem_cgroup *);

/*
 * For memory reclaim.
 */
int mem_cgroup_inactive_anon_is_low(struct mem_cgroup *memcg,
				    struct zone *zone);
int mem_cgroup_inactive_file_is_low(struct mem_cgroup *memcg,
				    struct zone *zone);
int mem_cgroup_select_victim_node(struct mem_cgroup *memcg);
unsigned long mem_cgroup_zone_nr_lru_pages(struct mem_cgroup *memcg,
					int nid, int zid, unsigned int lrumask);
>>>>>>> upstream/4.3_primoc
struct zone_reclaim_stat *mem_cgroup_get_reclaim_stat(struct mem_cgroup *memcg,
						      struct zone *zone);
struct zone_reclaim_stat*
mem_cgroup_get_reclaim_stat_from_page(struct page *page);
extern void mem_cgroup_print_oom_info(struct mem_cgroup *memcg,
					struct task_struct *p);
extern void mem_cgroup_replace_page_cache(struct page *oldpage,
					struct page *newpage);

#ifdef CONFIG_CGROUP_MEM_RES_CTLR_SWAP
extern int do_swap_account;
#endif

static inline bool mem_cgroup_disabled(void)
{
	if (mem_cgroup_subsys.disabled)
		return true;
	return false;
}

void mem_cgroup_update_page_stat(struct page *page,
				 enum mem_cgroup_page_stat_item idx,
				 int val);

static inline void mem_cgroup_inc_page_stat(struct page *page,
					    enum mem_cgroup_page_stat_item idx)
{
	mem_cgroup_update_page_stat(page, idx, 1);
}

static inline void mem_cgroup_dec_page_stat(struct page *page,
					    enum mem_cgroup_page_stat_item idx)
{
	mem_cgroup_update_page_stat(page, idx, -1);
}

unsigned long mem_cgroup_soft_limit_reclaim(struct zone *zone, int order,
						gfp_t gfp_mask,
						unsigned long *total_scanned);
<<<<<<< HEAD
u64 mem_cgroup_get_limit(struct mem_cgroup *mem);

void mem_cgroup_count_vm_event(struct mm_struct *mm, enum vm_event_item idx);
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
void mem_cgroup_split_huge_fixup(struct page *head, struct page *tail);
=======
u64 mem_cgroup_get_limit(struct mem_cgroup *memcg);

void mem_cgroup_count_vm_event(struct mm_struct *mm, enum vm_event_item idx);
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
void mem_cgroup_split_huge_fixup(struct page *head);
>>>>>>> upstream/4.3_primoc
#endif

#ifdef CONFIG_DEBUG_VM
bool mem_cgroup_bad_page_check(struct page *page);
void mem_cgroup_print_bad_page(struct page *page);
#endif
#else /* CONFIG_CGROUP_MEM_RES_CTLR */
struct mem_cgroup;

static inline int mem_cgroup_newpage_charge(struct page *page,
					struct mm_struct *mm, gfp_t gfp_mask)
{
	return 0;
}

static inline int mem_cgroup_cache_charge(struct page *page,
					struct mm_struct *mm, gfp_t gfp_mask)
{
	return 0;
}

static inline int mem_cgroup_try_charge_swapin(struct mm_struct *mm,
<<<<<<< HEAD
		struct page *page, gfp_t gfp_mask, struct mem_cgroup **ptr)
=======
		struct page *page, gfp_t gfp_mask, struct mem_cgroup **memcgp)
>>>>>>> upstream/4.3_primoc
{
	return 0;
}

static inline void mem_cgroup_commit_charge_swapin(struct page *page,
<<<<<<< HEAD
					  struct mem_cgroup *ptr)
{
}

static inline void mem_cgroup_cancel_charge_swapin(struct mem_cgroup *ptr)
=======
					  struct mem_cgroup *memcg)
{
}

static inline void mem_cgroup_cancel_charge_swapin(struct mem_cgroup *memcg)
>>>>>>> upstream/4.3_primoc
{
}

static inline void mem_cgroup_uncharge_start(void)
{
}

static inline void mem_cgroup_uncharge_end(void)
{
}

static inline void mem_cgroup_uncharge_page(struct page *page)
{
}

static inline void mem_cgroup_uncharge_cache_page(struct page *page)
{
}

<<<<<<< HEAD
static inline int mem_cgroup_shmem_charge_fallback(struct page *page,
			struct mm_struct *mm, gfp_t gfp_mask)
{
	return 0;
}

static inline void mem_cgroup_add_lru_list(struct page *page, int lru)
{
}

static inline void mem_cgroup_del_lru_list(struct page *page, int lru)
{
	return ;
}

static inline void mem_cgroup_rotate_reclaimable_page(struct page *page)
{
	return ;
}

static inline void mem_cgroup_rotate_lru_list(struct page *page, int lru)
{
	return ;
}

static inline void mem_cgroup_del_lru(struct page *page)
{
	return ;
}

static inline void
mem_cgroup_move_lists(struct page *page, enum lru_list from, enum lru_list to)
{
=======
static inline struct lruvec *mem_cgroup_zone_lruvec(struct zone *zone,
						    struct mem_cgroup *memcg)
{
	return &zone->lruvec;
}

static inline struct lruvec *mem_cgroup_lru_add_list(struct zone *zone,
						     struct page *page,
						     enum lru_list lru)
{
	return &zone->lruvec;
}

static inline void mem_cgroup_lru_del_list(struct page *page, enum lru_list lru)
{
}

static inline void mem_cgroup_lru_del(struct page *page)
{
}

static inline struct lruvec *mem_cgroup_lru_move_lists(struct zone *zone,
						       struct page *page,
						       enum lru_list from,
						       enum lru_list to)
{
	return &zone->lruvec;
>>>>>>> upstream/4.3_primoc
}

static inline struct mem_cgroup *try_get_mem_cgroup_from_page(struct page *page)
{
	return NULL;
}

static inline struct mem_cgroup *try_get_mem_cgroup_from_mm(struct mm_struct *mm)
{
	return NULL;
}

<<<<<<< HEAD
static inline int mm_match_cgroup(struct mm_struct *mm, struct mem_cgroup *mem)
=======
static inline int mm_match_cgroup(struct mm_struct *mm,
		struct mem_cgroup *memcg)
>>>>>>> upstream/4.3_primoc
{
	return 1;
}

static inline int task_in_mem_cgroup(struct task_struct *task,
<<<<<<< HEAD
				     const struct mem_cgroup *mem)
=======
				     const struct mem_cgroup *memcg)
>>>>>>> upstream/4.3_primoc
{
	return 1;
}

<<<<<<< HEAD
static inline struct cgroup_subsys_state *mem_cgroup_css(struct mem_cgroup *mem)
=======
static inline struct cgroup_subsys_state
		*mem_cgroup_css(struct mem_cgroup *memcg)
>>>>>>> upstream/4.3_primoc
{
	return NULL;
}

static inline int
mem_cgroup_prepare_migration(struct page *page, struct page *newpage,
<<<<<<< HEAD
	struct mem_cgroup **ptr, gfp_t gfp_mask)
=======
	struct mem_cgroup **memcgp, gfp_t gfp_mask)
>>>>>>> upstream/4.3_primoc
{
	return 0;
}

<<<<<<< HEAD
static inline void mem_cgroup_end_migration(struct mem_cgroup *mem,
=======
static inline void mem_cgroup_end_migration(struct mem_cgroup *memcg,
>>>>>>> upstream/4.3_primoc
		struct page *oldpage, struct page *newpage, bool migration_ok)
{
}

<<<<<<< HEAD
static inline int mem_cgroup_get_reclaim_priority(struct mem_cgroup *mem)
=======
static inline struct mem_cgroup *
mem_cgroup_iter(struct mem_cgroup *root,
		struct mem_cgroup *prev,
		struct mem_cgroup_reclaim_cookie *reclaim)
{
	return NULL;
}

static inline void mem_cgroup_iter_break(struct mem_cgroup *root,
					 struct mem_cgroup *prev)
{
}

static inline int mem_cgroup_get_reclaim_priority(struct mem_cgroup *memcg)
>>>>>>> upstream/4.3_primoc
{
	return 0;
}

<<<<<<< HEAD
static inline void mem_cgroup_note_reclaim_priority(struct mem_cgroup *mem,
=======
static inline void mem_cgroup_note_reclaim_priority(struct mem_cgroup *memcg,
>>>>>>> upstream/4.3_primoc
						int priority)
{
}

<<<<<<< HEAD
static inline void mem_cgroup_record_reclaim_priority(struct mem_cgroup *mem,
=======
static inline void mem_cgroup_record_reclaim_priority(struct mem_cgroup *memcg,
>>>>>>> upstream/4.3_primoc
						int priority)
{
}

static inline bool mem_cgroup_disabled(void)
{
	return true;
}

static inline int
<<<<<<< HEAD
mem_cgroup_inactive_anon_is_low(struct mem_cgroup *memcg)
=======
mem_cgroup_inactive_anon_is_low(struct mem_cgroup *memcg, struct zone *zone)
>>>>>>> upstream/4.3_primoc
{
	return 1;
}

static inline int
<<<<<<< HEAD
mem_cgroup_inactive_file_is_low(struct mem_cgroup *memcg)
=======
mem_cgroup_inactive_file_is_low(struct mem_cgroup *memcg, struct zone *zone)
>>>>>>> upstream/4.3_primoc
{
	return 1;
}

static inline unsigned long
<<<<<<< HEAD
mem_cgroup_zone_nr_lru_pages(struct mem_cgroup *memcg, struct zone *zone,
			     enum lru_list lru)
=======
mem_cgroup_zone_nr_lru_pages(struct mem_cgroup *memcg, int nid, int zid,
				unsigned int lru_mask)
>>>>>>> upstream/4.3_primoc
{
	return 0;
}


static inline struct zone_reclaim_stat*
mem_cgroup_get_reclaim_stat(struct mem_cgroup *memcg, struct zone *zone)
{
	return NULL;
}

static inline struct zone_reclaim_stat*
mem_cgroup_get_reclaim_stat_from_page(struct page *page)
{
	return NULL;
}

static inline void
mem_cgroup_print_oom_info(struct mem_cgroup *memcg, struct task_struct *p)
{
}

static inline void mem_cgroup_inc_page_stat(struct page *page,
					    enum mem_cgroup_page_stat_item idx)
{
}

static inline void mem_cgroup_dec_page_stat(struct page *page,
					    enum mem_cgroup_page_stat_item idx)
{
}

static inline
unsigned long mem_cgroup_soft_limit_reclaim(struct zone *zone, int order,
					    gfp_t gfp_mask,
					    unsigned long *total_scanned)
{
	return 0;
}

static inline
<<<<<<< HEAD
u64 mem_cgroup_get_limit(struct mem_cgroup *mem)
=======
u64 mem_cgroup_get_limit(struct mem_cgroup *memcg)
>>>>>>> upstream/4.3_primoc
{
	return 0;
}

<<<<<<< HEAD
static inline void mem_cgroup_split_huge_fixup(struct page *head,
						struct page *tail)
=======
static inline void mem_cgroup_split_huge_fixup(struct page *head)
>>>>>>> upstream/4.3_primoc
{
}

static inline
void mem_cgroup_count_vm_event(struct mm_struct *mm, enum vm_event_item idx)
{
}
static inline void mem_cgroup_replace_page_cache(struct page *oldpage,
				struct page *newpage)
{
}
#endif /* CONFIG_CGROUP_MEM_CONT */

#if !defined(CONFIG_CGROUP_MEM_RES_CTLR) || !defined(CONFIG_DEBUG_VM)
static inline bool
mem_cgroup_bad_page_check(struct page *page)
{
	return false;
}

static inline void
mem_cgroup_print_bad_page(struct page *page)
{
}
#endif

#endif /* _LINUX_MEMCONTROL_H */

