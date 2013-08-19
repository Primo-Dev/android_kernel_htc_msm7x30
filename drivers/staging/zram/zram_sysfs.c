/*
 * Compressed RAM block device
 *
 * Copyright (C) 2008, 2009, 2010  Nitin Gupta
 *
 * This code is released using a dual license strategy: BSD/GPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of 3-clause BSD License
 * Released under the terms of GNU General Public License Version 2.0
 *
 * Project home: http://compcache.googlecode.com/
 */

#include <linux/device.h>
#include <linux/genhd.h>
#include <linux/mm.h>

#include "zram_drv.h"

static u64 zram_stat64_read(struct zram *zram, u64 *v)
{
	u64 val;

	spin_lock(&zram->stat64_lock);
	val = *v;
	spin_unlock(&zram->stat64_lock);

	return val;
}

static struct zram *dev_to_zram(struct device *dev)
{
	int i;
	struct zram *zram = NULL;

<<<<<<< HEAD
	for (i = 0; i < num_devices; i++) {
=======
	for (i = 0; i < zram_get_num_devices(); i++) {
>>>>>>> upstream/4.3_primoc
		zram = &zram_devices[i];
		if (disk_to_dev(zram->disk) == dev)
			break;
	}

	return zram;
}

static ssize_t disksize_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n", zram->disksize);
}

static ssize_t disksize_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t len)
{
	int ret;
<<<<<<< HEAD
	struct zram *zram = dev_to_zram(dev);

	if (zram->init_done) {
=======
	u64 disksize;
	struct zram *zram = dev_to_zram(dev);

	ret = kstrtoull(buf, 10, &disksize);
	if (ret)
		return ret;

	down_write(&zram->init_lock);
	if (zram->init_done) {
		up_write(&zram->init_lock);
>>>>>>> upstream/4.3_primoc
		pr_info("Cannot change disksize for initialized device\n");
		return -EBUSY;
	}

<<<<<<< HEAD
	ret = strict_strtoull(buf, 10, &zram->disksize);
	if (ret)
		return ret;

	zram->disksize &= PAGE_MASK;
	set_capacity(zram->disk, zram->disksize >> SECTOR_SHIFT);
=======
	zram->disksize = PAGE_ALIGN(disksize);
	set_capacity(zram->disk, zram->disksize >> SECTOR_SHIFT);
	zram_init_device(zram);
	up_write(&zram->init_lock);
>>>>>>> upstream/4.3_primoc

	return len;
}

static ssize_t initstate_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%u\n", zram->init_done);
}

static ssize_t reset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t len)
{
	int ret;
<<<<<<< HEAD
	unsigned long do_reset;
=======
	unsigned short do_reset;
>>>>>>> upstream/4.3_primoc
	struct zram *zram;
	struct block_device *bdev;

	zram = dev_to_zram(dev);
	bdev = bdget_disk(zram->disk, 0);

	/* Do not reset an active device! */
	if (bdev->bd_holders)
		return -EBUSY;

<<<<<<< HEAD
	ret = strict_strtoul(buf, 10, &do_reset);
=======
	ret = kstrtou16(buf, 10, &do_reset);
>>>>>>> upstream/4.3_primoc
	if (ret)
		return ret;

	if (!do_reset)
		return -EINVAL;

	/* Make sure all pending I/O is finished */
	if (bdev)
		fsync_bdev(bdev);

<<<<<<< HEAD
	if (zram->init_done)
		zram_reset_device(zram);

=======
	zram_reset_device(zram);
>>>>>>> upstream/4.3_primoc
	return len;
}

static ssize_t num_reads_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		zram_stat64_read(zram, &zram->stats.num_reads));
}

static ssize_t num_writes_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		zram_stat64_read(zram, &zram->stats.num_writes));
}

static ssize_t invalid_io_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		zram_stat64_read(zram, &zram->stats.invalid_io));
}

static ssize_t notify_free_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		zram_stat64_read(zram, &zram->stats.notify_free));
}

<<<<<<< HEAD
static ssize_t discard_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		zram_stat64_read(zram, &zram->stats.discard));
}

=======
>>>>>>> upstream/4.3_primoc
static ssize_t zero_pages_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%u\n", zram->stats.pages_zero);
}

static ssize_t orig_data_size_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		(u64)(zram->stats.pages_stored) << PAGE_SHIFT);
}

static ssize_t compr_data_size_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct zram *zram = dev_to_zram(dev);

	return sprintf(buf, "%llu\n",
		zram_stat64_read(zram, &zram->stats.compr_size));
}

static ssize_t mem_used_total_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	u64 val = 0;
	struct zram *zram = dev_to_zram(dev);

<<<<<<< HEAD
	if (zram->init_done) {
		val = xv_get_total_size_bytes(zram->mem_pool) +
			((u64)(zram->stats.pages_expand) << PAGE_SHIFT);
	}
=======
	if (zram->init_done)
		val = zs_get_total_size_bytes(zram->mem_pool);
>>>>>>> upstream/4.3_primoc

	return sprintf(buf, "%llu\n", val);
}

static DEVICE_ATTR(disksize, S_IRUGO | S_IWUSR,
		disksize_show, disksize_store);
static DEVICE_ATTR(initstate, S_IRUGO, initstate_show, NULL);
static DEVICE_ATTR(reset, S_IWUSR, NULL, reset_store);
static DEVICE_ATTR(num_reads, S_IRUGO, num_reads_show, NULL);
static DEVICE_ATTR(num_writes, S_IRUGO, num_writes_show, NULL);
static DEVICE_ATTR(invalid_io, S_IRUGO, invalid_io_show, NULL);
static DEVICE_ATTR(notify_free, S_IRUGO, notify_free_show, NULL);
<<<<<<< HEAD
static DEVICE_ATTR(discard, S_IRUGO, discard_show, NULL);
=======
>>>>>>> upstream/4.3_primoc
static DEVICE_ATTR(zero_pages, S_IRUGO, zero_pages_show, NULL);
static DEVICE_ATTR(orig_data_size, S_IRUGO, orig_data_size_show, NULL);
static DEVICE_ATTR(compr_data_size, S_IRUGO, compr_data_size_show, NULL);
static DEVICE_ATTR(mem_used_total, S_IRUGO, mem_used_total_show, NULL);

static struct attribute *zram_disk_attrs[] = {
	&dev_attr_disksize.attr,
	&dev_attr_initstate.attr,
	&dev_attr_reset.attr,
	&dev_attr_num_reads.attr,
	&dev_attr_num_writes.attr,
	&dev_attr_invalid_io.attr,
	&dev_attr_notify_free.attr,
<<<<<<< HEAD
	&dev_attr_discard.attr,
=======
>>>>>>> upstream/4.3_primoc
	&dev_attr_zero_pages.attr,
	&dev_attr_orig_data_size.attr,
	&dev_attr_compr_data_size.attr,
	&dev_attr_mem_used_total.attr,
	NULL,
};

struct attribute_group zram_disk_attr_group = {
	.attrs = zram_disk_attrs,
};
<<<<<<< HEAD


=======
>>>>>>> upstream/4.3_primoc
