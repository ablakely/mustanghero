#include <mustanghero.h>
#include <amplifier.h>
#include <mustang.h>

int main(int argc, char *argv[])
{
	start_amp();

	struct amp_settings punk;
	punk.amp_num		= METAL_2000;
	punk.gain		= 250;
	punk.volume		= 250;
	punk.treble		= 100;
	punk.middle		= 100;
	punk.bass		= 100;
	punk.cabinet		= cabCHAMP;
	punk.noise_gate		= 50;
	punk.master_vol		= 250;
	punk.gain2		= 100;
	punk.presence		= 50;
	punk.threshold		= 60;
	punk.depth		= 100;
	punk.bias		= 100;
	punk.sag		= 50;
	punk.brightness		= 1;
	punk.usb_gain		= 100;

	mustang_setAmplifier(punk);
	mustang_stopAmp();
}
