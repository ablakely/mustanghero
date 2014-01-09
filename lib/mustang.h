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

enum amps
{
	FENDER_57_DELUXE,
	FENDER_59_BASSMAN,
	FENDER_57_CHAMP,
	FENDER_65_DELUXE_REVERB,
	FENDER_65_PRINCETON,
	FENDER_65_TWIN_REVERB,
	FENDER_SUPER_SONIC,
	BRITISH_60S,
	BRITISH_70S,
	BRITISH_80S,
	AMERICAN_90S,
	METAL_2000
};

enum effects
{
	EMPTY,
	OVERDRIVE,
	WAH,
	TOUCH_WAH,
	FUZZ,
	FUZZ_TOUCH_WAH,
	SIMPLE_COMP,
	COMPRESSOR,

	SINE_CHORUS,
	TRIANGLE_CHORUS,
	SINE_FLANGER,
	TRIANGLE_FLANGER,
	VIBRATONE,
	VINTAGE_TREMOLO,
	SINE_TREMOLO,
	RING_MODULATOR,
	STEP_FILTER,
	PHASER,
	PITCH_SHIFTER,

	MONO_DELAY,
	MONO_ECHO_FILTER,
	STEREO_ECHO_FILTER,
	MULTITAP_DELAY,
	PING_PONG_DELAY,
	DUCKING_DELAY,
	REVERSE_DELAY,
	TAPE_DELAY,
	STEREO_TAPE_DELAY,

	SMALL_HALL_REVERB,
	LARGE_HALL_REVERB,
	SMALL_ROOM_REVERB,
	LARGE_ROOM_REVERB,
	SMALL_PLATE_REVERB,
	LARGE_PLATE_REVERB,
	AMBIENT_REVERB,
	ARENA_REVERB,
	FENDER_63_SPRING_REVERB,
	FENDER_65_SPRING_REVERB
};

enum cabinets
{
	OFF,
	cab57DLX,
	cabBSSMN,
	cab65DLX,
	cab65PRN,
	cabCHAMP,
	cab4x12M,
	cab2x12C,
	cab4x12G,
	cab65TWM,
	cab4x12V,
	cabSS212,
	cabSS112
};

// Amp's VID and PID
#define USB_VID		0x1ed8
#define OLD_USB_PID	0x0004	// For the Mustang I and II
#define NEW_USB_PID	0x0005  // Mustang III, IV, and V

// For USB communication
#define TMOUT	500
#define LENGTH	64

// Effect array fields
#define DSP	2
#define EFFECT	16
#define FXSLOT	18
#define KNOB1	32
#define KNOB2	33
#define KNOB3	34
#define KNOB4	35
#define KNOB5	36
#define KNOB6	37

// Amplifier array fields
#define AMPLIFIER	16
#define VOLUME		32
#define GAIN		33
#define TREBLE		36
#define MIDDLE		37
#define BASS		38
#define CABINET		49
#define NOISE_GATE	47
#define THRESHOLD	48
#define MASTER_VOL	35
#define GAIN2		34
#define PRESENCE	39
#define DEPTH		41
#define BIAS		42
#define SAG		51
#define BRIGHTNESS	52

void mustang_initDriver();
int  mustang_startAmp(char list[][32], char *name, struct amp_settings *amp_set, struct fx_pedal_settings *effects_set);
int  mustang_stopAmp();
int  mustang_setEffect(struct fx_pedal_settings);

#endif
