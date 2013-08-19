/*
  File: fs/ext2/acl.h

  (C) 2001 Andreas Gruenbacher, <a.gruenbacher@computer.org>
*/

#include <linux/posix_acl_xattr.h>

#define EXT2_ACL_VERSION	0x0001

typedef struct {
	__le16		e_tag;
	__le16		e_perm;
	__le32		e_id;
} ext2_acl_entry;

typedef struct {
	__le16		e_tag;
	__le16		e_perm;
} ext2_acl_entry_short;

typedef struct {
	__le32		a_version;
} ext2_acl_header;

static inline size_t ext2_acl_size(int count)
{
	if (count <= 4) {
		return sizeof(ext2_acl_header) +
		       count * sizeof(ext2_acl_entry_short);
	} else {
		return sizeof(ext2_acl_header) +
		       4 * sizeof(ext2_acl_entry_short) +
		       (count - 4) * sizeof(ext2_acl_entry);
	}
}

static inline int ext2_acl_count(size_t size)
{
	ssize_t s;
	size -= sizeof(ext2_acl_header);
	s = size - 4 * sizeof(ext2_acl_entry_short);
	if (s < 0) {
		if (size % sizeof(ext2_acl_entry_short))
			return -1;
		return size / sizeof(ext2_acl_entry_short);
	} else {
		if (s % sizeof(ext2_acl_entry))
			return -1;
		return s / sizeof(ext2_acl_entry) + 4;
	}
}

#ifdef CONFIG_EXT2_FS_POSIX_ACL

/* acl.c */
<<<<<<< HEAD
extern int ext2_check_acl (struct inode *, int, unsigned int);
=======
extern struct posix_acl *ext2_get_acl(struct inode *inode, int type);
>>>>>>> upstream/4.3_primoc
extern int ext2_acl_chmod (struct inode *);
extern int ext2_init_acl (struct inode *, struct inode *);

#else
#include <linux/sched.h>
<<<<<<< HEAD
#define ext2_check_acl	NULL
=======
#define ext2_get_acl	NULL
>>>>>>> upstream/4.3_primoc
#define ext2_get_acl	NULL
#define ext2_set_acl	NULL

static inline int
ext2_acl_chmod (struct inode *inode)
{
	return 0;
}

static inline int ext2_init_acl (struct inode *inode, struct inode *dir)
{
	return 0;
}
#endif

