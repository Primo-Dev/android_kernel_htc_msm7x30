/*
  File: fs/ext4/acl.h

  (C) 2001 Andreas Gruenbacher, <a.gruenbacher@computer.org>
*/

#include <linux/posix_acl_xattr.h>

#define EXT4_ACL_VERSION	0x0001

typedef struct {
	__le16		e_tag;
	__le16		e_perm;
	__le32		e_id;
} ext4_acl_entry;

typedef struct {
	__le16		e_tag;
	__le16		e_perm;
} ext4_acl_entry_short;

typedef struct {
	__le32		a_version;
} ext4_acl_header;

static inline size_t ext4_acl_size(int count)
{
	if (count <= 4) {
		return sizeof(ext4_acl_header) +
		       count * sizeof(ext4_acl_entry_short);
	} else {
		return sizeof(ext4_acl_header) +
		       4 * sizeof(ext4_acl_entry_short) +
		       (count - 4) * sizeof(ext4_acl_entry);
	}
}

static inline int ext4_acl_count(size_t size)
{
	ssize_t s;
	size -= sizeof(ext4_acl_header);
	s = size - 4 * sizeof(ext4_acl_entry_short);
	if (s < 0) {
		if (size % sizeof(ext4_acl_entry_short))
			return -1;
		return size / sizeof(ext4_acl_entry_short);
	} else {
		if (s % sizeof(ext4_acl_entry))
			return -1;
		return s / sizeof(ext4_acl_entry) + 4;
	}
}

#ifdef CONFIG_EXT4_FS_POSIX_ACL

/* acl.c */
<<<<<<< HEAD
extern int ext4_check_acl(struct inode *, int, unsigned int);
=======
struct posix_acl *ext4_get_acl(struct inode *inode, int type);
>>>>>>> upstream/4.3_primoc
extern int ext4_acl_chmod(struct inode *);
extern int ext4_init_acl(handle_t *, struct inode *, struct inode *);

#else  /* CONFIG_EXT4_FS_POSIX_ACL */
#include <linux/sched.h>
<<<<<<< HEAD
#define ext4_check_acl NULL
=======
#define ext4_get_acl NULL
>>>>>>> upstream/4.3_primoc

static inline int
ext4_acl_chmod(struct inode *inode)
{
	return 0;
}

static inline int
ext4_init_acl(handle_t *handle, struct inode *inode, struct inode *dir)
{
	return 0;
}
#endif  /* CONFIG_EXT4_FS_POSIX_ACL */

