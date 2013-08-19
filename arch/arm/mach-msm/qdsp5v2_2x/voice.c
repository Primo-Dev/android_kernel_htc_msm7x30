/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/msm_audio.h>
#include <mach/qdsp5v2_2x/audio_dev_ctl.h>
#include <mach/dal.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <mach/qdsp5v2_2x/voice.h>
#include <mach/qdsp5v2_2x/audio_def.h>
#include <mach/debug_mm.h>

struct voice_data {
	void *handle; /* DALRPC handle */
	void *cb_handle; /* DALRPC callback handle */
	int network; /* Network information */
	int dev_state;/*READY, CHANGE, REL_DONE,INIT*/
	int voc_state;/*INIT, CHANGE, RELEASE, ACQUIRE */
	struct mutex lock;
	int voc_event;
	int dev_event;
	atomic_t rel_start_flag;
	atomic_t acq_start_flag;
	struct task_struct *task;
	struct completion complete;
	wait_queue_head_t dev_wait;
	wait_queue_head_t voc_wait;
	uint32_t device_events;
	/* cache the values related to Rx and Tx */
	struct device_data dev_rx;
	struct device_data dev_tx;
	/* these default values are for all devices */
	uint32_t default_mute_val;
	uint32_t default_vol_val;
	uint32_t default_sample_val;
	/* call status */
	int v_call_status; /* Start or End */
};

static struct q5v2_hw_info def_audio_hw[Q5V2_HW_COUNT] = {
	[Q5V2_HW_HANDSET] = {
		.max_gain[VOC_NB_INDEX] = 400,
		.min_gain[VOC_NB_INDEX] = -1600,
		.max_gain[VOC_WB_INDEX] = 400,
		.min_gain[VOC_WB_INDEX] = -1600,
	},
	[Q5V2_HW_HEADSET] = {
		.max_gain[VOC_NB_INDEX] = 900,
		.min_gain[VOC_NB_INDEX] = -1100,
		.max_gain[VOC_WB_INDEX] = 900,
		.min_gain[VOC_WB_INDEX] = -1100,
	},
	[Q5V2_HW_SPEAKER] = {
		.max_gain[VOC_NB_INDEX] = 1000,
		.min_gain[VOC_NB_INDEX] = -500,
		.max_gain[VOC_WB_INDEX] = 1000,
		.min_gain[VOC_WB_INDEX] = -500,
	},
	[Q5V2_HW_BT_SCO] = {
		.max_gain[VOC_NB_INDEX] = 0,
		.min_gain[VOC_NB_INDEX] = -1500,
		.max_gain[VOC_WB_INDEX] = 0,
		.min_gain[VOC_WB_INDEX] = -1500,
	},
	[Q5V2_HW_TTY] = {
		.max_gain[VOC_NB_INDEX] = 0,
		.min_gain[VOC_NB_INDEX] = 0,
		.max_gain[VOC_WB_INDEX] = 0,
		.min_gain[VOC_WB_INDEX] = 0,
	},
	[Q5V2_HW_HS_SPKR] = {
		.max_gain[VOC_NB_INDEX] = -500,
		.min_gain[VOC_NB_INDEX] = -2000,
		.max_gain[VOC_WB_INDEX] = -500,
		.min_gain[VOC_WB_INDEX] = -2000,
	},
	[Q5V2_HW_USB_HS] = {
		.max_gain[VOC_NB_INDEX] = 1000,
		.min_gain[VOC_NB_INDEX] = -500,
		.max_gain[VOC_WB_INDEX] = 1000,
		.min_gain[VOC_WB_INDEX] = -500,
	},
	[Q5V2_HW_HAC] = {
		.max_gain[VOC_NB_INDEX] = 1000,
		.min_gain[VOC_NB_INDEX] = -500,
		.max_gain[VOC_WB_INDEX] = 1000,
		.min_gain[VOC_WB_INDEX] = -500,
	},
};

static struct voice_data voice;
static struct q5v2voice_ops default_voice_ops;
static struct q5v2voice_ops *voice_ops = &default_voice_ops;

static int voice_cmd_device_info(struct voice_data *);
static int voice_cmd_acquire_done(struct voice_data *);
static void voice_auddev_cb_function(u32 evt_id,
			union auddev_evt_data *evt_payload,
			void *private_data);

static int get_def_voice_volume(uint8_t hw, int network, int level)
{
	struct q5v2_hw_info *info;
	int vol, maxv, minv;

	info = &def_audio_hw[hw];
	maxv = info->max_gain[network];
	minv = info->min_gain[network];
	vol = minv + ((maxv - minv) * level) / 100;
<<<<<<< HEAD
	pr_aud_info("%s(%d, %d, %d) => %d\n", __func__, hw, network, level, vol);
=======
	pr_info("%s(%d, %d, %d) => %d\n", __func__, hw, network, level, vol);
>>>>>>> upstream/4.3_primoc
	return vol;
}
static int voice_cmd_change(void)
{

	struct voice_header hdr;
	struct voice_data *v = &voice;
	int err;

	hdr.id = CMD_DEVICE_CHANGE;
	hdr.data_len = 0;

<<<<<<< HEAD
	pr_aud_info("[voice] send CMD_DEVICE_CHANGE\n");
=======
	pr_info("[voice] send CMD_DEVICE_CHANGE\n");
>>>>>>> upstream/4.3_primoc

	err = dalrpc_fcn_5(VOICE_DALRPC_CMD, v->handle, &hdr,
			sizeof(struct voice_header));

	if (err)
<<<<<<< HEAD
		pr_aud_err("%s: failed, err %d\n", __func__, err);
=======
		MM_ERR("%s: failed, err %d\n", __func__, err);
>>>>>>> upstream/4.3_primoc
	return err;
}

static void voice_auddev_cb_function(u32 evt_id,
			union auddev_evt_data *evt_payload,
			void *private_data)
{
	struct voice_data *v = &voice;
	int rc = 0, mute = 0;

<<<<<<< HEAD
	pr_aud_info("auddev_cb_function, evt_id = %d, dev_state = %d\n",
=======
	MM_INFO("auddev_cb_function, evt_id = %d, dev_state = %d\n",
>>>>>>> upstream/4.3_primoc
		evt_id, v->dev_state);

	if ((evt_id != AUDDEV_EVT_START_VOICE) ||
			(evt_id != AUDDEV_EVT_END_VOICE)) {
		if (evt_payload == NULL) {
<<<<<<< HEAD
			pr_aud_err("%s: NULL payload\n", __func__);
=======
			MM_ERR("%s: NULL payload\n", __func__);
>>>>>>> upstream/4.3_primoc
			return;
		}
	}
	switch (evt_id) {
	case AUDDEV_EVT_START_VOICE:
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_START_VOICE\n");
=======
		pr_info("[dev ctrl] AUDDEV_EVT_START_VOICE\n");
>>>>>>> upstream/4.3_primoc
		if ((v->dev_state == DEV_INIT) ||
				(v->dev_state == DEV_REL_DONE)) {
			v->v_call_status = VOICE_CALL_START;
			if ((v->dev_rx.enabled == VOICE_DEV_ENABLED)
				&& (v->dev_tx.enabled == VOICE_DEV_ENABLED)) {
				v->dev_state = DEV_READY;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_READY\n");
=======
				pr_info("dev_state -> DEV_READY\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
				if (v->voc_state == VOICE_CHANGE) {
					mutex_lock(&voice.lock);
					v->dev_event = DEV_CHANGE_READY;
					mutex_unlock(&voice.lock);
					complete(&v->complete);
				}
			}
		}
		break;
	case AUDDEV_EVT_DEV_CHG_VOICE:
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_DEV_CHG_VOICE\n");
=======
		pr_info("[dev ctrl] AUDDEV_EVT_DEV_CHG_VOICE\n");
>>>>>>> upstream/4.3_primoc
		if (v->dev_state == DEV_READY) {
			v->dev_rx.enabled = VOICE_DEV_DISABLED;
			v->dev_tx.enabled = VOICE_DEV_DISABLED;
			v->dev_state = DEV_CHANGE;
<<<<<<< HEAD
			pr_aud_info("dev_state -> DEV_CHANGE\n");
=======
			pr_info("dev_state -> DEV_CHANGE\n");
>>>>>>> upstream/4.3_primoc
			if (v->voc_state == VOICE_ACQUIRE) {
				msm_snddev_enable_sidetone(v->dev_rx.dev_id,
				0);
				/* send device change to modem */
				voice_cmd_change();
				/* block to wait for CHANGE_START */
<<<<<<< HEAD
				pr_aud_info("start waiting for "
					"voc_state -> VOICE_CHANGE\n");
				rc = wait_event_interruptible(
				v->voc_wait, (v->voc_state == VOICE_CHANGE)
				|| (atomic_read(&v->rel_start_flag) == 1));
				pr_aud_info("wait done, voc_state = %d\n", v->voc_state);
			} else {
				pr_aud_info("Get AUDDEV_EVT_DEV_CHG_VOICE "
=======
				pr_info("start waiting for "
					"voc_state -> VOICE_CHANGE\n");
				/*Add timeout for wait event interruptible*/
				rc = wait_event_interruptible_timeout(
				v->voc_wait, (v->voc_state == VOICE_CHANGE)
				|| (atomic_read(&v->rel_start_flag) == 1), msecs_to_jiffies(1000));
				if (rc == 0) {
					pr_info("wait timeout, voc_state = %d\n", v->voc_state);
					return;
				}
				pr_aud_info("wait done, voc_state = %d\n", v->voc_state);
			} else {
				MM_ERR("Get AUDDEV_EVT_DEV_CHG_VOICE "
>>>>>>> upstream/4.3_primoc
				       "at improper voc_state %d\n", v->voc_state);
				voice_cmd_change();
			}
		} else if ((v->dev_state == DEV_INIT) ||
				(v->dev_state == DEV_REL_DONE)) {
				v->dev_rx.enabled = VOICE_DEV_DISABLED;
				v->dev_tx.enabled = VOICE_DEV_DISABLED;
		} else {
<<<<<<< HEAD
			pr_aud_info("Get AUDDEV_EVT_DEV_CHG_VOICE "
=======
			MM_ERR("Get AUDDEV_EVT_DEV_CHG_VOICE "
>>>>>>> upstream/4.3_primoc
			       "at improper dev_state %d\n", v->dev_state);
			voice_cmd_change();
		}

		break;
	case AUDDEV_EVT_DEV_RDY:
		/* update the dev info */
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_DEV_RDY\n");
=======
		pr_info("[dev ctrl] AUDDEV_EVT_DEV_RDY\n");
>>>>>>> upstream/4.3_primoc
		if (evt_payload->voc_devinfo.dev_type == DIR_RX)
			v->dev_rx.vol_idx = evt_payload->voc_devinfo.vol_idx;

		if (v->dev_state == DEV_CHANGE) {
			if (evt_payload->voc_devinfo.dev_type == DIR_RX) {
				v->dev_rx.dev_acdb_id =
					evt_payload->voc_devinfo.acdb_dev_id;
				v->dev_rx.sample =
					evt_payload->voc_devinfo.dev_sample;
				v->dev_rx.dev_id =
				evt_payload->voc_devinfo.dev_id;
				v->dev_rx.enabled = VOICE_DEV_ENABLED;
			} else {
				v->dev_tx.dev_acdb_id =
					evt_payload->voc_devinfo.acdb_dev_id;
				v->dev_tx.sample =
					evt_payload->voc_devinfo.dev_sample;
				v->dev_tx.enabled = VOICE_DEV_ENABLED;
				v->dev_tx.dev_id =
				evt_payload->voc_devinfo.dev_id;
			}
			if ((v->dev_rx.enabled == VOICE_DEV_ENABLED) &&
				(v->dev_tx.enabled == VOICE_DEV_ENABLED)) {
				mutex_lock(&voice.lock);
				v->dev_event = DEV_CHANGE_READY;
				mutex_unlock(&voice.lock);
				complete(&v->complete);
				v->dev_state = DEV_READY;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_READY\n");
=======
				pr_info("dev_state -> DEV_READY\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
			}
		} else if ((v->dev_state == DEV_INIT) ||
			(v->dev_state == DEV_REL_DONE)) {
			if (evt_payload->voc_devinfo.dev_type == DIR_RX) {
				v->dev_rx.dev_acdb_id =
					evt_payload->voc_devinfo.acdb_dev_id;
				v->dev_rx.sample =
					evt_payload->voc_devinfo.dev_sample;
				v->dev_rx.dev_id =
				evt_payload->voc_devinfo.dev_id;
				v->dev_rx.enabled = VOICE_DEV_ENABLED;
			} else {
				v->dev_tx.dev_acdb_id =
					evt_payload->voc_devinfo.acdb_dev_id;
				v->dev_tx.sample =
					evt_payload->voc_devinfo.dev_sample;
				v->dev_tx.dev_id =
				evt_payload->voc_devinfo.dev_id;
				v->dev_tx.enabled = VOICE_DEV_ENABLED;
			}
			if ((v->dev_rx.enabled == VOICE_DEV_ENABLED) &&
				(v->dev_tx.enabled == VOICE_DEV_ENABLED) &&
				(v->v_call_status == VOICE_CALL_START)) {
				v->dev_state = DEV_READY;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_READY\n");
=======
				pr_info("dev_state -> DEV_READY\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
				if (v->voc_state == VOICE_CHANGE) {
					mutex_lock(&voice.lock);
					v->dev_event = DEV_CHANGE_READY;
					mutex_unlock(&voice.lock);
					complete(&v->complete);
				}
			}
		} else {
<<<<<<< HEAD
			pr_aud_err("Get AUDDEV_EVT_DEV_RDY "
=======
			MM_ERR("Get AUDDEV_EVT_DEV_RDY "
>>>>>>> upstream/4.3_primoc
			       "at improper dev_state %d\n", v->dev_state);
			voice_cmd_device_info(v);
		}

		break;
	case AUDDEV_EVT_DEVICE_VOL_MUTE_CHG:
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_DEVICE_VOL_MUTE_CHG\n");
=======
		pr_info("[dev ctrl] AUDDEV_EVT_DEVICE_VOL_MUTE_CHG\n");
>>>>>>> upstream/4.3_primoc
		if (evt_payload->voc_devinfo.dev_type == DIR_TX)
			v->dev_tx.mute =
				evt_payload->voc_vm_info.dev_vm_val.mute;
		else {
			mute = (int)evt_payload->voc_vm_info.dev_vm_val.mute;
<<<<<<< HEAD
			pr_aud_info("%s, mute = %d\n", __func__, mute);
=======
			pr_info("%s, mute = %d\n", __func__, mute);
>>>>>>> upstream/4.3_primoc
			if (mute == 1) { /*mute rx*/
				v->dev_rx.mute = evt_payload->
					voc_vm_info.dev_vm_val.mute;
			} else {
				v->dev_rx.mute = 0;
				v->dev_rx.volume = evt_payload->
					voc_vm_info.dev_vm_val.vol;
			}
		}
		/* send device info */
		voice_cmd_device_info(v);
		break;
	case AUDDEV_EVT_REL_PENDING:
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_REL_PENDING, dev_state %d\n",
=======
		pr_info("[dev ctrl] AUDDEV_EVT_REL_PENDING, dev_state %d\n",
>>>>>>> upstream/4.3_primoc
			v->dev_state);
		/* recover the tx mute and rx volume to the default values */
		if (v->dev_state == DEV_READY) {
			if (atomic_read(&v->rel_start_flag)) {
				atomic_dec(&v->rel_start_flag);
				if (evt_payload->voc_devinfo.dev_type == DIR_RX)
					v->dev_rx.enabled = VOICE_DEV_DISABLED;
				else
					v->dev_tx.enabled = VOICE_DEV_DISABLED;
				v->dev_state = DEV_REL_DONE;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_REL_DONE\n");
=======
				pr_info("dev_state -> DEV_REL_DONE\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
			} else if ((v->voc_state == VOICE_RELEASE) ||
					(v->voc_state == VOICE_INIT)) {
				if (evt_payload->voc_devinfo.dev_type
							== DIR_RX) {
					v->dev_rx.enabled = VOICE_DEV_DISABLED;
				} else {
					v->dev_tx.enabled = VOICE_DEV_DISABLED;
				}
				v->dev_state = DEV_REL_DONE;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_REL_DONE\n");
=======
				pr_info("dev_state -> DEV_REL_DONE\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
			} else {
				/* send mute and default volume value to MCAD */
				v->dev_tx.mute = v->default_mute_val;
				v->dev_rx.volume = v->default_vol_val;
				voice_cmd_device_info(v);
				/* send device change to modem */
				voice_cmd_change();
<<<<<<< HEAD
				pr_aud_info("start waiting for "
=======
				pr_info("start waiting for "
>>>>>>> upstream/4.3_primoc
					"voc_state -> VOICE_CHANGE\n");
				rc = wait_event_interruptible(
				v->voc_wait, (v->voc_state == VOICE_CHANGE)
				|| (atomic_read(&v->rel_start_flag) == 1));
				if (atomic_read(&v->rel_start_flag) == 1)
					atomic_dec(&v->rel_start_flag);
				/* clear Rx/Tx to Disable */
				if (evt_payload->voc_devinfo.dev_type == DIR_RX)
					v->dev_rx.enabled = VOICE_DEV_DISABLED;
				else
					v->dev_tx.enabled = VOICE_DEV_DISABLED;
				v->dev_state = DEV_REL_DONE;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_REL_DONE\n");
=======
				pr_info("dev_state -> DEV_REL_DONE\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
			}
		} else if ((v->dev_state == DEV_INIT) ||
				(v->dev_state == DEV_REL_DONE)) {
			if (evt_payload->voc_devinfo.dev_type == DIR_RX)
				v->dev_rx.enabled = VOICE_DEV_DISABLED;
			else
				v->dev_tx.enabled = VOICE_DEV_DISABLED;
		}
		break;
	case AUDDEV_EVT_END_VOICE:
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_END_VOICE\n");
=======
		pr_info("[dev ctrl] AUDDEV_EVT_END_VOICE\n");
>>>>>>> upstream/4.3_primoc
		/* recover the tx mute and rx volume to the default values */
		v->dev_tx.mute = v->default_mute_val;
		v->dev_rx.volume = v->default_vol_val;

		if (v->dev_rx.enabled == VOICE_DEV_ENABLED)
			msm_snddev_enable_sidetone(v->dev_rx.dev_id, 0);

		if ((v->dev_state == DEV_READY) ||
			(v->dev_state == DEV_CHANGE)) {
			if (atomic_read(&v->rel_start_flag)) {
				atomic_dec(&v->rel_start_flag);
				v->v_call_status = VOICE_CALL_END;
				v->dev_state = DEV_REL_DONE;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_REL_DONE\n");
=======
				pr_info("dev_state -> DEV_REL_DONE\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
			} else if ((v->voc_state == VOICE_RELEASE) ||
					(v->voc_state == VOICE_INIT)) {
				v->v_call_status = VOICE_CALL_END;
				v->dev_state = DEV_REL_DONE;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_REL_DONE\n");
				wake_up(&v->dev_wait);
			} else {
				pr_aud_info("send voice_cmd_change at voc_state %d\n",
=======
				pr_info("dev_state -> DEV_REL_DONE\n");
				wake_up(&v->dev_wait);
			} else {
				pr_info("send voice_cmd_change at voc_state %d\n",
>>>>>>> upstream/4.3_primoc
					v->voc_state);
				/* send mute and default volume value to MCAD */
				voice_cmd_device_info(v);
				/* send device change to modem */
				voice_cmd_change();
				/* block to wait for RELEASE_START
						or CHANGE_START */
<<<<<<< HEAD
				pr_aud_info("start waiting for "
=======
				pr_info("start waiting for "
>>>>>>> upstream/4.3_primoc
					"voc_state -> VOICE_CHANGE\n");
				rc = wait_event_interruptible(
				v->voc_wait, (v->voc_state == VOICE_CHANGE)
				|| (atomic_read(&v->rel_start_flag) == 1));
				if (atomic_read(&v->rel_start_flag) == 1)
					atomic_dec(&v->rel_start_flag);
				/* set voice call to END state */
				v->v_call_status = VOICE_CALL_END;
				v->dev_state = DEV_REL_DONE;
<<<<<<< HEAD
				pr_aud_info("dev_state -> DEV_REL_DONE\n");
=======
				pr_info("dev_state -> DEV_REL_DONE\n");
>>>>>>> upstream/4.3_primoc
				wake_up(&v->dev_wait);
			}
		} else
			v->v_call_status = VOICE_CALL_END;
		break;
	case AUDDEV_EVT_FREQ_CHG:
<<<<<<< HEAD
		pr_aud_info("[dev ctrl] AUDDEV_EVT_FREQ_CHG\n");
=======
		pr_info("[dev ctrl] AUDDEV_EVT_FREQ_CHG\n");
>>>>>>> upstream/4.3_primoc
		MM_DBG("Voice Driver got sample rate change Event\n");
		MM_DBG("sample rate %d\n", evt_payload->freq_info.sample_rate);
		MM_DBG("dev_type %d\n", evt_payload->freq_info.dev_type);
		MM_DBG("acdb_dev_id %d\n", evt_payload->freq_info.acdb_dev_id);
		if (v->dev_state == DEV_READY) {
			v->dev_tx.enabled = VOICE_DEV_DISABLED;
			v->dev_state = DEV_CHANGE;
<<<<<<< HEAD
			pr_aud_info("dev_state -> DEV_CHANGE\n");
=======
			pr_info("dev_state -> DEV_CHANGE\n");
>>>>>>> upstream/4.3_primoc
			if (v->voc_state == VOICE_ACQUIRE) {
				/* send device change to modem */
				voice_cmd_change();
				/* block to wait for CHANGE_START */
<<<<<<< HEAD
				pr_aud_info("start waiting for "
=======
				pr_info("start waiting for "
>>>>>>> upstream/4.3_primoc
					"voc_state -> VOICE_CHANGE\n");
				rc = wait_event_interruptible(
				v->voc_wait, (v->voc_state == VOICE_CHANGE)
				|| (atomic_read(&v->rel_start_flag) == 1));
			} else
<<<<<<< HEAD
				pr_aud_err(" Voice is not at ACQUIRE state"
=======
				MM_ERR(" Voice is not at ACQUIRE state"
>>>>>>> upstream/4.3_primoc
				       " (voc_state %d)\n", v->voc_state);
		} else if ((v->dev_state == DEV_INIT) ||
				(v->dev_state == DEV_REL_DONE)) {
				v->dev_tx.enabled = VOICE_DEV_DISABLED;
		} else
<<<<<<< HEAD
			pr_aud_err("Get AUDDEV_EVT_FREQ_CHG "
=======
			MM_ERR("Get AUDDEV_EVT_FREQ_CHG "
>>>>>>> upstream/4.3_primoc
			       "at improper dev_state %d\n", v->dev_state);

		break;
	default:
<<<<<<< HEAD
		pr_aud_err("%s: unknown event %d\n", __func__, evt_id);
=======
		MM_ERR("%s: unknown event %d\n", __func__, evt_id);
>>>>>>> upstream/4.3_primoc
	}
	return;
}
EXPORT_SYMBOL(voice_auddev_cb_function);

static void remote_cb_function(void *context, u32 param,
				void *evt_buf, u32 len)
{
	struct voice_header *hdr;
	struct voice_data *v = context;

	hdr = (struct voice_header *)evt_buf;

<<<<<<< HEAD
	pr_aud_info("%s() len = %d, id = %d\n", __func__, len, hdr->id);

	if (len <= 0) {
		pr_aud_err("%s: invalid param length %d \n", __func__, len);
=======
	MM_INFO("%s() len = %d, id = %d\n", __func__, len, hdr->id);

	if (len <= 0) {
		MM_ERR("%s: invalid param length %d \n", __func__, len);
>>>>>>> upstream/4.3_primoc
		return;
	}

	switch (hdr->id) {
	case EVENT_ACQUIRE_START:
<<<<<<< HEAD
		pr_aud_info("[radio] EVENT_ACQUIRE_START\n");
=======
		pr_info("[radio] EVENT_ACQUIRE_START\n");
>>>>>>> upstream/4.3_primoc
		atomic_inc(&v->acq_start_flag);
		wake_up(&v->dev_wait);
		mutex_lock(&voice.lock);
		v->voc_event = VOICE_ACQUIRE_START;
		v->network = ((struct voice_network *)evt_buf)->network_info;
		mutex_unlock(&voice.lock);
		complete(&v->complete);
		break;
	case EVENT_RELEASE_START:
<<<<<<< HEAD
		pr_aud_info("[radio] EVENT_RELEASE_START\n");
=======
		pr_info("[radio] EVENT_RELEASE_START\n");
>>>>>>> upstream/4.3_primoc
		/* If ACQUIRED come in before the RELEASE,
		* will only services the RELEASE */
		atomic_inc(&v->rel_start_flag);
		wake_up(&v->voc_wait);
		wake_up(&v->dev_wait);
		mutex_lock(&voice.lock);
		v->voc_event = VOICE_RELEASE_START;
		mutex_unlock(&voice.lock);
		complete(&v->complete);
		break;
	case EVENT_CHANGE_START:
<<<<<<< HEAD
		pr_aud_info("[radio] EVENT_CHANGE_START\n");
=======
		pr_info("[radio] EVENT_CHANGE_START\n");
>>>>>>> upstream/4.3_primoc
		mutex_lock(&voice.lock);
		v->voc_event = VOICE_CHANGE_START;
		mutex_unlock(&voice.lock);
		complete(&v->complete);
		break;
	case EVENT_NETWORK_RECONFIG:
		/* send network change to audio_dev,
		if sample rate is less than 16k,
		otherwise, send acquire done */
<<<<<<< HEAD
		pr_aud_info("[radio] EVENT_NETWORK_CONFIG\n");
=======
		pr_info("[radio] EVENT_NETWORK_CONFIG\n");
>>>>>>> upstream/4.3_primoc
		mutex_lock(&voice.lock);
		v->voc_event = VOICE_NETWORK_RECONFIG;
		v->network = ((struct voice_network *)evt_buf)->network_info;
		mutex_unlock(&voice.lock);
		complete(&v->complete);
		break;
	default:
<<<<<<< HEAD
		pr_aud_err("%s: unknown event %d \n", __func__, hdr->id);
=======
		MM_ERR("%s: unknown event %d \n", __func__, hdr->id);
>>>>>>> upstream/4.3_primoc
	}

}

static int voice_cmd_init(struct voice_data *v)
{

	struct voice_init cmd;
	int err;

<<<<<<< HEAD
	pr_aud_info("[voice] send CMD_ACQUIRE_INIT\n");
=======
	pr_info("[voice] send CMD_ACQUIRE_INIT\n");
>>>>>>> upstream/4.3_primoc

	cmd.hdr.id = CMD_VOICE_INIT;
	cmd.hdr.data_len = sizeof(struct voice_init) -
				sizeof(struct voice_header);
	cmd.cb_handle = v->cb_handle;

	err = dalrpc_fcn_5(VOICE_DALRPC_CMD, v->handle, &cmd,
			 sizeof(struct voice_init));

	if (err)
<<<<<<< HEAD
		pr_aud_err("%s: failed, err %d\n", __func__, err);
=======
		MM_ERR("%s: failed, err %d\n", __func__, err);
>>>>>>> upstream/4.3_primoc
	return err;
}

static int voice_cmd_acquire_done(struct voice_data *v)
{
	struct voice_header hdr;
	int err;

	hdr.id = CMD_ACQUIRE_DONE;
	hdr.data_len = 0;

<<<<<<< HEAD
	pr_aud_info("[voice] send CMD_ACQUIRE_DONE\n");
=======
	pr_info("[voice] send CMD_ACQUIRE_DONE\n");
>>>>>>> upstream/4.3_primoc

	/* Enable HW sidetone if device supports it  */
	msm_snddev_enable_sidetone(v->dev_rx.dev_id, 1);

	err = dalrpc_fcn_5(VOICE_DALRPC_CMD, v->handle, &hdr,
			 sizeof(struct voice_header));

	if (err)
<<<<<<< HEAD
		pr_aud_err("%s: failed, err %d\n", __func__, err);
=======
		MM_ERR("%s: failed, err %d\n", __func__, err);
>>>>>>> upstream/4.3_primoc
	return err;
}

static int voice_cmd_release_done(struct voice_data *v)
{
	struct voice_header hdr;
	int err;

	hdr.id = CMD_RELEASE_DONE;
	hdr.data_len = 0;

<<<<<<< HEAD
	pr_aud_info("[voice] send CMD_RELEASE_DONE\n");
=======
	pr_info("[voice] send CMD_RELEASE_DONE\n");
>>>>>>> upstream/4.3_primoc

	err = dalrpc_fcn_5(VOICE_DALRPC_CMD, v->handle, &hdr,
			 sizeof(struct voice_header));

	if (err)
<<<<<<< HEAD
		pr_aud_err("%s: failed, err %d\n", __func__, err);
=======
		MM_ERR("%s: failed, err %d\n", __func__, err);
>>>>>>> upstream/4.3_primoc
	return err;
}

static int voice_cmd_device_info(struct voice_data *v)
{
	struct voice_device cmd;
	int err, vol;

<<<<<<< HEAD
	pr_aud_info("%s(), tx_dev = %d, rx_dev = %d,"
=======
	MM_INFO("%s(), tx_dev = %d, rx_dev = %d,"
>>>>>>> upstream/4.3_primoc
		" tx_sample = %d, rx_sample = %d \n",
		__func__, v->dev_tx.dev_acdb_id, v->dev_rx.dev_acdb_id,
		v->dev_tx.sample, v->dev_rx.sample);

<<<<<<< HEAD
	pr_aud_info("[voice] send CMD_DEVICE_INFO "
=======
	pr_info("[voice] send CMD_DEVICE_INFO "
>>>>>>> upstream/4.3_primoc
		"(tx %d, rate %d) (rx %d, rate %d)\n",
		v->dev_tx.dev_acdb_id, v->dev_tx.sample,
		v->dev_rx.dev_acdb_id, v->dev_rx.sample);


	cmd.hdr.id = CMD_DEVICE_INFO;
	cmd.hdr.data_len = sizeof(struct voice_device) -
			sizeof(struct voice_header);
	cmd.tx_device = v->dev_tx.dev_acdb_id;
	cmd.rx_device = v->dev_rx.dev_acdb_id;
	if (v->network == NETWORK_WCDMA_WB) {
		if (voice_ops->get_rx_vol) {
			vol = voice_ops->get_rx_vol(v->dev_rx.vol_idx,
						VOC_WB_INDEX, v->dev_rx.volume);
		} else {
			vol = get_def_voice_volume(v->dev_rx.vol_idx,
						VOC_WB_INDEX, v->dev_rx.volume);
		}
	} else {
		if (voice_ops->get_rx_vol) {
			vol = voice_ops->get_rx_vol(v->dev_rx.vol_idx,
						VOC_NB_INDEX, v->dev_rx.volume);
		} else {
			vol = get_def_voice_volume(v->dev_rx.vol_idx,
						VOC_NB_INDEX, v->dev_rx.volume);
		}
	}
	cmd.rx_volume = (u32)vol; /* in mb */
	/*cmd.rx_mute = 0;*/
	cmd.rx_mute = v->dev_rx.mute;
	cmd.tx_mute = v->dev_tx.mute;
	cmd.rx_sample = v->dev_rx.sample/1000;
	cmd.tx_sample = v->dev_tx.sample/1000;

<<<<<<< HEAD
	pr_aud_info("rx dev_id = %d, tx_dev_id = %d,"
		"rx_vol = %d, tx_mute = %d, rx_mute = %d\n",
		v->dev_rx.dev_id, v->dev_tx.dev_id,
		cmd.rx_volume, v->dev_tx.mute, v->dev_rx.mute);
	pr_aud_info("rx_vol = %d, tx_mute = %d\n", cmd.rx_volume, v->dev_tx.mute);
=======
	pr_info("rx dev_id = %d, tx_dev_id = %d,"
		"rx_vol = %d, tx_mute = %d, rx_mute = %d\n",
		v->dev_rx.dev_id, v->dev_tx.dev_id,
		cmd.rx_volume, v->dev_tx.mute, v->dev_rx.mute);
	MM_INFO("rx_vol = %d, tx_mute = %d\n", cmd.rx_volume, v->dev_tx.mute);
>>>>>>> upstream/4.3_primoc

	err = dalrpc_fcn_5(VOICE_DALRPC_CMD, v->handle, &cmd,
			 sizeof(struct voice_device));

	if (err)
<<<<<<< HEAD
		pr_aud_err("%s: failed, err %d\n", __func__, err);
=======
		MM_ERR("%s: failed, err %d\n", __func__, err);
>>>>>>> upstream/4.3_primoc
	return err;
}
EXPORT_SYMBOL(voice_cmd_device_info);

void voice_change_sample_rate(struct voice_data *v)
{
	int freq = 8000;
	int rc = 0;

<<<<<<< HEAD
	pr_aud_info("network = %d, vote freq = %d\n", v->network, freq);
	pr_aud_info("%s: network %d, freq %d\n", __func__, v->network, freq);
=======
	MM_INFO("network = %d, vote freq = %d\n", v->network, freq);
	pr_info("%s: network %d, freq %d\n", __func__, v->network, freq);
>>>>>>> upstream/4.3_primoc
	if (freq != v->dev_tx.sample) {
		rc = msm_snddev_request_freq(&freq, 0,
				SNDDEV_CAP_TX, AUDDEV_CLNT_VOC);
		if (rc >= 0) {
			v->dev_tx.sample = freq;
<<<<<<< HEAD
			pr_aud_info("%s: set freq %d success.\n", __func__, freq);
		} else
			pr_aud_err("%s: set freq %d failed.\n\n", __func__, freq);
=======
			MM_INFO("%s: set freq %d success.\n", __func__, freq);
		} else
			MM_ERR("%s: set freq %d failed.\n\n", __func__, freq);
>>>>>>> upstream/4.3_primoc
	}
}

static int voice_thread(void *data)
{
	struct voice_data *v = (struct voice_data *)data;
	int rc = 0;

<<<<<<< HEAD
	pr_aud_info("voice_thread() start\n");
=======
	MM_INFO("voice_thread() start\n");
>>>>>>> upstream/4.3_primoc

	while (!kthread_should_stop()) {
		wait_for_completion(&v->complete);
		init_completion(&v->complete);

<<<<<<< HEAD
		pr_aud_info("handle voice event %d, "
=======
		pr_info("handle voice event %d, "
>>>>>>> upstream/4.3_primoc
			"(voc_state %d, dev_event %d)\n",
			v->voc_event, v->voc_state, v->dev_event);

		if (v->dev_event != DEV_CHANGE_READY) {
			switch (v->voc_event) {
			case VOICE_ACQUIRE_START:
				/* check if dev_state = READY */
				/* if ready, send device_info and acquire_done */
				/* if not ready, block to wait the dev_state = READY */
				if ((v->voc_state == VOICE_INIT) ||
					(v->voc_state == VOICE_RELEASE)) {
					if (v->dev_state == DEV_READY) {
						voice_change_sample_rate(v);
						rc = voice_cmd_device_info(v);
						rc = voice_cmd_acquire_done(v);
						v->voc_state = VOICE_ACQUIRE;
<<<<<<< HEAD
						pr_aud_info("voc_state -> VOICE_ACQUIRE\n");
					} else {
						pr_aud_info("start waiting for "
=======
                                                broadcast_event(AUDDEV_EVT_VOICE_STATE_CHG,
                                                        VOICE_STATE_INCALL, SESSION_IGNORE);
						pr_info("voc_state -> VOICE_ACQUIRE\n");
					} else {
						pr_info("start waiting for "
>>>>>>> upstream/4.3_primoc
							"dev_state -> DEV_READY\n");
						rc = wait_event_interruptible(
							v->dev_wait,
							(v->dev_state == DEV_READY)
							|| (atomic_read(&v->rel_start_flag) == 1));
<<<<<<< HEAD
						if (atomic_read(&v->rel_start_flag)
							== 1) {
							v->voc_state = VOICE_RELEASE;
							pr_aud_info("voc_state -> VOICE_RELEASE\n");
							atomic_dec(&v->rel_start_flag);
							msm_snddev_withdraw_freq(0,
								SNDDEV_CAP_TX, AUDDEV_CLNT_VOC);
=======
						if (atomic_read(&v->rel_start_flag)== 1) {
							v->voc_state = VOICE_RELEASE;
							pr_info("voc_state -> VOICE_RELEASE\n");
							atomic_dec(&v->rel_start_flag);
							msm_snddev_withdraw_freq(0,
								SNDDEV_CAP_TX, AUDDEV_CLNT_VOC);
                                                        broadcast_event(AUDDEV_EVT_VOICE_STATE_CHG,
                                                                VOICE_STATE_OFFCALL, SESSION_IGNORE);
>>>>>>> upstream/4.3_primoc
						} else {
							voice_change_sample_rate(v);
							rc = voice_cmd_device_info(v);
							rc = voice_cmd_acquire_done(v);
							v->voc_state = VOICE_ACQUIRE;
<<<<<<< HEAD
							pr_aud_info("voc_state -> VOICE_ACQUIRE\n");
						}
					}
				} else {
					pr_aud_err("Get VOICE_ACQUIRE_START "
=======
                                                        broadcast_event(AUDDEV_EVT_VOICE_STATE_CHG,
                                                                VOICE_STATE_INCALL, SESSION_IGNORE);
							pr_info("voc_state -> VOICE_ACQUIRE\n");
						}
					}
				} else {
					pr_err("Get VOICE_ACQUIRE_START "
>>>>>>> upstream/4.3_primoc
					       "at wrong voc_state %d\n", v->voc_state);
					/* avoid vocoder state of modem side will be blocked
					when audo path has been changed before acquire start */
					voice_change_sample_rate(v);
					rc = voice_cmd_device_info(v);
					rc = voice_cmd_acquire_done(v);
				}

				if (atomic_read(&v->acq_start_flag))
					atomic_dec(&v->acq_start_flag);
				break;
			case VOICE_RELEASE_START:
				if ((v->dev_state == DEV_REL_DONE) ||
					(v->dev_state == DEV_INIT)) {
					v->voc_state = VOICE_RELEASE;
<<<<<<< HEAD
					pr_aud_info("voc_state -> VOICE_RELEASE\n");
					msm_snddev_withdraw_freq(0, SNDDEV_CAP_TX,
						AUDDEV_CLNT_VOC);
				} else {
					/* wait for the dev_state = RELEASE */
					pr_aud_info("start waiting for "
=======
					pr_info("voc_state -> VOICE_RELEASE\n");
					msm_snddev_withdraw_freq(0, SNDDEV_CAP_TX, AUDDEV_CLNT_VOC);
                                        broadcast_event(AUDDEV_EVT_VOICE_STATE_CHG,
                                                VOICE_STATE_OFFCALL,SESSION_IGNORE);
				} else {
					/* wait for the dev_state = RELEASE */
					pr_info("start waiting for "
>>>>>>> upstream/4.3_primoc
						"dev_state -> DEV_REL_DONE\n");
					rc = wait_event_interruptible(v->dev_wait,
						(v->dev_state == DEV_REL_DONE)
						|| (atomic_read(&v->acq_start_flag) == 1));
					if (atomic_read(&v->acq_start_flag) == 1)
						atomic_dec(&v->acq_start_flag);
<<<<<<< HEAD
					else {
						rc = voice_cmd_release_done(v);
						pr_aud_info("voc_state -> VOICE_RELEASE\n");
						msm_snddev_withdraw_freq(0, SNDDEV_CAP_TX,
							AUDDEV_CLNT_VOC);
					}
					v->voc_state = VOICE_RELEASE;
=======
					else 
						rc = voice_cmd_release_done(v);
					v->voc_state = VOICE_RELEASE;
					pr_info("voc_state -> VOICE_RELEASE\n");
					msm_snddev_withdraw_freq(0, SNDDEV_CAP_TX,AUDDEV_CLNT_VOC);
                                        broadcast_event(AUDDEV_EVT_VOICE_STATE_CHG,
                                                VOICE_STATE_OFFCALL,SESSION_IGNORE);
>>>>>>> upstream/4.3_primoc
				}
				if (atomic_read(&v->rel_start_flag))
					atomic_dec(&v->rel_start_flag);
				break;
			case VOICE_CHANGE_START:
				if (v->voc_state == VOICE_ACQUIRE) {
					v->voc_state = VOICE_CHANGE;
<<<<<<< HEAD
					pr_aud_info("voc_state -> VOICE_CHANGE\n");
				} else
					pr_aud_err("Get VOICE_CHANGE_START "
=======
					pr_info("voc_state -> VOICE_CHANGE\n");
				} else
					MM_ERR("Get VOICE_CHANGE_START "
>>>>>>> upstream/4.3_primoc
					       "at wrong voc_state %d\n", v->voc_state);

				if (v->dev_state == DEV_READY)
					voice_cmd_device_info(v);

				wake_up(&v->voc_wait);
				break;
			case VOICE_NETWORK_RECONFIG:
				if ((v->voc_state == VOICE_ACQUIRE)
					|| (v->voc_state == VOICE_CHANGE)) {
					voice_change_sample_rate(v);
					rc = voice_cmd_device_info(v);
					rc = voice_cmd_acquire_done(v);
					rc = voice_cmd_device_info(v);
				} else
<<<<<<< HEAD
					pr_aud_err("Get VOICE_NETWORK_RECONFIG "
=======
					pr_err("Get VOICE_NETWORK_RECONFIG "
>>>>>>> upstream/4.3_primoc
					       "at wrong voc_state %d\n", v->voc_state);
				break;
			default:
				break;
			}
		}

		switch (v->dev_event) {
		case DEV_CHANGE_READY:
<<<<<<< HEAD
			pr_aud_info("Get DEV_CHANGE_READY at voc_state %d\n",
=======
			pr_info("Get DEV_CHANGE_READY at voc_state %d\n",
>>>>>>> upstream/4.3_primoc
				v->voc_state);
			if (v->voc_state == VOICE_CHANGE) {
				msm_snddev_enable_sidetone(v->dev_rx.dev_id, 1);
				/* send device info to modem */
				voice_cmd_device_info(v);
				/* update voice state */
				v->voc_state = VOICE_ACQUIRE;
<<<<<<< HEAD
				pr_aud_info("voc_state -> VOICE_ACQUIRE\n");
			} else {
				pr_aud_info("Get DEV_CHANGE_READY "
=======
				pr_info("voc_state -> VOICE_ACQUIRE\n");
                                broadcast_event(AUDDEV_EVT_VOICE_STATE_CHG,
                                        VOICE_STATE_INCALL, SESSION_IGNORE);
			} else {
				MM_ERR("Get DEV_CHANGE_READY "
>>>>>>> upstream/4.3_primoc
					"at the wrong voc_state %d\n", v->voc_state);
				voice_cmd_device_info(v);
			}

			break;
		default:
			break;
		}
		mutex_lock(&voice.lock);
		v->dev_event = 0;
		mutex_unlock(&voice.lock);
	}
	return 0;
}

void htc_7x30_register_voice_ops(struct q5v2voice_ops *ops)
{
	voice_ops = ops;
}

static int __init voice_init(void)
{
	int rc;
	struct voice_data *v = &voice;
<<<<<<< HEAD
	pr_aud_info("%s\n", __func__);
=======
	MM_INFO("%s\n", __func__);
>>>>>>> upstream/4.3_primoc

	mutex_init(&voice.lock);
	v->handle = NULL;
	v->cb_handle = NULL;

	/* set default value */
	v->default_mute_val = 1;  /* default is mute */
	v->default_vol_val = 100;
	v->default_sample_val = 8000;
	v->network = NETWORK_GSM;

	/* initialize dev_rx and dev_tx */
	memset(&v->dev_tx, 0, sizeof(struct device_data));
	memset(&v->dev_rx, 0, sizeof(struct device_data));
	v->dev_rx.volume = v->default_vol_val;
	v->dev_tx.mute = v->default_mute_val;

	v->dev_state = DEV_INIT;
<<<<<<< HEAD
	pr_aud_info("dev_state -> DEV_INIT\n");
	v->voc_state = VOICE_INIT;
	pr_aud_info("voc_state -> VOICE_INIT\n");
=======
	pr_info("dev_state -> DEV_INIT\n");
	v->voc_state = VOICE_INIT;
	pr_info("voc_state -> VOICE_INIT\n");
>>>>>>> upstream/4.3_primoc
	atomic_set(&v->rel_start_flag, 0);
	atomic_set(&v->acq_start_flag, 0);
	v->dev_event = 0;
	v->voc_event = 0;
	init_completion(&voice.complete);
	init_waitqueue_head(&v->dev_wait);
	init_waitqueue_head(&v->voc_wait);

	 /* get device handle */
	rc = daldevice_attach(VOICE_DALRPC_DEVICEID,
				VOICE_DALRPC_PORT_NAME,
				VOICE_DALRPC_CPU,
				&v->handle);
	if (rc) {
<<<<<<< HEAD
		pr_aud_err("%s: daldevice_attach failed, rc %d\n",
=======
		MM_ERR("%s: daldevice_attach failed, rc %d\n",
>>>>>>> upstream/4.3_primoc
			__func__, rc);
		goto done;
	}

	/* Allocate the callback handle */
	v->cb_handle = dalrpc_alloc_cb(v->handle, remote_cb_function, v);
	if (v->cb_handle == NULL) {
<<<<<<< HEAD
		pr_aud_err("%s: dalrpc_alloc_cb failed\n", __func__);
=======
		MM_ERR("%s: dalrpc_alloc_cb failed\n", __func__);
>>>>>>> upstream/4.3_primoc
		goto err;
	}

	/* setup the callback */
	rc = voice_cmd_init(v);
	if (rc)
		goto err1;

	v->device_events = AUDDEV_EVT_DEV_CHG_VOICE |
			AUDDEV_EVT_DEV_RDY |
			AUDDEV_EVT_REL_PENDING |
			AUDDEV_EVT_START_VOICE |
			AUDDEV_EVT_END_VOICE |
			AUDDEV_EVT_DEVICE_VOL_MUTE_CHG |
			AUDDEV_EVT_FREQ_CHG;

	/* register callback to auddev */
	auddev_register_evt_listner(v->device_events, AUDDEV_CLNT_VOC,
				0, voice_auddev_cb_function, v);

	/* create and start thread */
	v->task = kthread_run(voice_thread, v, "voice");
	if (IS_ERR(v->task)) {
		rc = PTR_ERR(v->task);
		v->task = NULL;
	} else
		goto done;

err1:   dalrpc_dealloc_cb(v->handle, v->cb_handle);
err:
	daldevice_detach(v->handle);
	v->handle = NULL;
done:
	return rc;
}

late_initcall(voice_init);
