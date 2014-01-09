#ifndef MUSTANG_H
#define MUSTANG_H

/*
 * mustanghero: Mustang Amplifer USB Driver
 *
 * Most of this code has been rewritten from the PLUG project.
 * http://code.piorekf.org/plug/wiki/Home
 */

#include <mustanghero.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

struct amp_settings
{
	unsigned char amp_num;
	unsigned char gain;
	unsigned char volume;
	unsigned char treble;
	unsigned char middle;
	unsigned char bass;
	unsigned char cabinet;
	unsigned char noise_gate;
	unsigned char master_vol;
	unsigned char gain2;
	unsigned char presence;
	unsigned char threshold;
	unsigned char depth;
	unsigned char bias;
	unsigned char sag;
	bool brightness;
	unsigned char usb_gain;
};

struct fx_pedal_settings
{
	unsigned char fx_slot;
	unsigned char effect_num;
	unsigned char knob1;
	unsigned char knob2;
	unsigned char knob3;
	unsigned char knob4;
	unsigned char knob5;
	unsigned char knob6;
	bool put_post_amp;
};

// Amp's VID and PID
#define USB_VID		0x1ed8
#define OLD_USB_PID	0x0004	// For the Mustang I and II
#define NEW_USB_PID	0x0005  // Mustang III, IV, and V

// For USB communication
#define TMOUT	500
#define LENGTH	64

int mustang_startAmp(char list[][32], char *name, struct amp_settings *amp_set, struct fx_pedal_settings *effects_set);
int mustang_stopAmp();

#endif
