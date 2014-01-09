#include <mustang.h>
#include <libusb-1.0/libusb.h>

libusb_device_handle *amp_hand;  	// Handle for USB Communication
unsigned char execute[LENGTH];		// "apply" command sent after each instruction
unsigned char prev_array[4][LENGTH];	// array used to clear the effect


void mustang_initDriver()
{
	amp_hand = NULL;

	// "apply effect" command
	memset(execute, 0x00, LENGTH);
	execute[0]	= 0x1c;
	execute[1]	= 0x03;

	memset(prev_array, 0x00, LENGTH*4);
	for (int i = 0; i < 4; i++)
	{
		prev_array[i][0]	= 0x1c;
		prev_array[i][1]	= 0x03;
		prev_array[i][6]	= prev_array[i][7] = prev_array[i][21] = 0x01;
		prev_array[i][20]	= 0x08;
		prev_array[i][FXSLOT]	= 0xff;
	}
}

int mustang_startAmp(char list[][32], char *name, struct amp_settings *amp_set, struct fx_pedal_settings *effects_set)
{
	int ret, recieved;
	unsigned char array[LENGTH];
	unsigned char recieved_data[296][LENGTH], data[7][LENGTH];
	memset(recieved_data, 0x00, 296*LENGTH);

	if (amp_hand == NULL)
	{
		ret = libusb_init(NULL);
		if (ret) return ret;

		// get the handle for the amp
		if ((amp_hand = libusb_open_device_with_vid_pid(NULL, USB_VID, OLD_USB_PID)) == NULL)
			if ((amp_hand = libusb_open_device_with_vid_pid(NULL, USB_VID, NEW_USB_PID)) == NULL)
			{
				libusb_exit(NULL);
				return -100;
			}

		// detach kernel driver
		ret = libusb_kernel_driver_active(amp_hand, 0);
		if (ret)
		{
			ret = libusb_detach_kernel_driver(amp_hand, 0);
			if (ret)
			{
				mustang_stopAmp();
				return ret;
			}
		}

		// claim our baby
		ret = libusb_claim_interface(amp_hand, 0);
		if (ret)
		{
			mustang_stopAmp();
			return ret;
		}
	}

	// Initialization which is required for the amp
	// to send replies in the future
	memset(array, 0x00, LENGTH);
	array[1] = 0xc3;
	libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);
	libusb_interrupt_transfer(amp_hand, 0x81, array, LENGTH, &recieved, TMOUT);

	memset(array, 0x00, LENGTH);
	array[0]	= 0x1a;
	array[1]	= 0x03;
	libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);
	libusb_interrupt_transfer(amp_hand, 0x81, array, LENGTH, &recieved, TMOUT);

	if (list != NULL || name != NULL || amp_set != NULL || effects_set != NULL)
	{
		int i = 0, j = 0;
		memset(array, 0x00, LENGTH);
		array[0]	= 0xff;
		array[1]	= 0xc1;
		libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);

		for (i = 0; recieved; i++)
		{
			libusb_interrupt_transfer(amp_hand, 0x81, array, LENGTH, &recieved, TMOUT);
			memcpy(recieved_data[i], array, LENGTH);
		}

		int max_to_receive;
		if (i > 143)
		{
			max_to_receive = 200;
		}
		else
		{
			max_to_receive = 48;
		}



		if (list != NULL)
			for (i = 0, j = 0; i < max_to_receive; i+=2, j++)
				memcpy(list[j], recieved_data[i]+16, 32);

		if (name != NULL || amp_set != NULL || effects_set != NULL)
		{
			for (j = 0; j < 7; i++, j++)
				memcpy(data[j], recieved_data[i], LENGTH);

			//mustang_decodeData(data, name, amp_set, effects_set);
		}
	}

	return 0;
}


int mustang_stopAmp()
{
	int ret;

	if (amp_hand != NULL)
	{
		// release claimed interface
		ret = libusb_release_interface(amp_hand, 0);

		if (ret && (ret != LIBUSB_ERROR_NO_DEVICE))
			return ret;


		if (ret != LIBUSB_ERROR_NO_DEVICE)
		{
			// re-attach kernel driver
			ret = libusb_attach_kernel_driver(amp_hand, 0);
			if (ret) return ret;
		}

		// close opened interface
		libusb_close(amp_hand);
		amp_hand = NULL;

		// stop using libusb
		libusb_exit(NULL);
	}

	return 0;
}


int mustang_setEffect(struct fx_pedal_settings value)
{
	int ret, recieved;
	unsigned char slot;
	unsigned char temp[LENGTH], array[LENGTH] = {
	        0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	// Clear effect on previous DSP befor setting a new one
	int k = 0;
	for (int i = 0; i < 4; i++)
	{
		if (prev_array[i][FXSLOT] == value.fx_slot || prev_array[i][FXSLOT] == (value.fx_slot+4))
		{
			memcpy(array, prev_array[i], LENGTH);
			prev_array[i][FXSLOT] = 0xff;
			k++;
			break;
		}
	}

	array[EFFECT]		= 0x00;
	array[KNOB1]		= 0x00;
	array[KNOB2]		= 0x00;
	array[KNOB3]		= 0x00;
	array[KNOB4]		= 0x00;
	array[KNOB5]		= 0x00;
	array[KNOB6]		= 0x00;

	ret = libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);
	libusb_interrupt_transfer(amp_hand, 0x81, temp, LENGTH, &recieved, TMOUT);
	ret = libusb_interrupt_transfer(amp_hand, 0x01, execute, LENGTH, &recieved, TMOUT);
	libusb_interrupt_transfer(amp_hand, 0x81, temp, LENGTH, &recieved, TMOUT);

	if (value.effect_num == EMPTY)
		return ret;

	if (value.put_post_amp)		// put effect in a slot after amplifer
	{
		slot	= value.fx_slot + 4;
	}
	else
	{
		slot	= value.fx_slot;
	}

	// Fill the form with data
	array[FXSLOT]		= slot;
	array[KNOB1]		= value.knob1;
	array[KNOB2]		= value.knob2;
	array[KNOB3]		= value.knob3;
	array[KNOB4]		= value.knob4;
	array[KNOB5]		= value.knob5;

	// Some effects have more knobs/paramaters
	if (	value.effect_num		== MONO_ECHO_FILTER 	||
		value.effect_num		== STEREO_ECHO_FILTER	||
		value.effect_num		== TAPE_DELAY		||
		value.effect_num		== STEREO_TAPE_DELAY)
	{
		array[KNOB6] = value.knob6;
	}

	// Fill the form with missing data
	switch (value.effect_num)
	{
		case OVERDRIVE:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x3c;
		break;

		case WAH:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x49;
			array[19]	= 0x01;
		break;

		case TOUCH_WAH:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x4a;
			array[19]	= 0x01;
		break;

		case FUZZ:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x1a;
		break;

		case FUZZ_TOUCH_WAH:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x1c;
		break;

		case SIMPLE_COMP:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x88;
			array[19]	= 0x08;

			if (array[KNOB1] > 0x03)
			{
				array[KNOB1] = 0x03;
			}

			array[KNOB2]	= 0x00;
			array[KNOB3]	= 0x00;
			array[KNOB4]	= 0x00;
			array[KNOB5]	= 0x00;
		break;

		case COMPRESSOR:
			array[DSP]	= 0x06;
			array[EFFECT]	= 0x07;
		break;

		case SINE_CHORUS:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x12;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case TRIANGLE_CHORUS:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x13;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case SINE_FLANGER:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x18;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case TRIANGLE_FLANGER:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x19;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case VIBRATONE:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x2d;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case VINTAGE_TREMOLO:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x40;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case SINE_TREMOLO:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x41;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case RING_MODULATOR:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x22;
			array[19]	= 0x01;

			if (array[KNOB4] > 0x01)
			{
				array[KNOB4] = 0x01;
			}
		break;

		case STEP_FILTER:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x29;
			array[19]	= 0x01;
			array[20]	= 0x01;
		break;

		case PHASER:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x4f;
			array[19]	= 0x01;
			array[20]	= 0x01;

			if (array[KNOB5] > 0x01)
			{
				array[KNOB5] = 0x01;
			}
		break;

		case PITCH_SHIFTER:
			array[DSP]	= 0x07;
			array[EFFECT]	= 0x1f;
			array[19]	= 0x01;
		break;

		case MONO_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x16;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case MONO_ECHO_FILTER:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x43;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case STEREO_ECHO_FILTER:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x48;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case MULTITAP_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x44;
			array[19]	= 0x02;
			array[20]	= 0x01;

			if (array[KNOB5] > 0x03)
			{
				array[KNOB5] = 0x03;
			}
		break;

		case PING_PONG_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x45;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case DUCKING_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x45;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case REVERSE_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x46;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case TAPE_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT] 	= 0x2b;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case STEREO_TAPE_DELAY:
			array[DSP]	= 0x08;
			array[EFFECT]	= 0x2a;
			array[19]	= 0x02;
			array[20]	= 0x01;
		break;

		case SMALL_HALL_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x24;
		break;

		case LARGE_HALL_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x3a;
		break;

		case SMALL_ROOM_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x26;
		break;

		case LARGE_ROOM_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x3b;
		break;

		case SMALL_PLATE_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x4e;
		break;

		case LARGE_PLATE_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x4b;
		break;

		case AMBIENT_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x4c;
		break;

		case ARENA_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x4d;
		break;

		case FENDER_63_SPRING_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x21;
		break;

		case FENDER_65_SPRING_REVERB:
			array[DSP]	= 0x09;
			array[EFFECT]	= 0x0b;
		break;
	}

	// Send packet to the amplifier.
	ret = libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);
	libusb_interrupt_transfer(amp_hand, 0x81, temp, LENGTH, &recieved, TMOUT);
	ret = libusb_interrupt_transfer(amp_hand, 0x01, execute, LENGTH, &recieved, TMOUT);
	libusb_interrupt_transfer(amp_hand, 0x81, temp, LENGTH, &recieved, TMOUT);

	// Save the current settings
	memcpy(prev_array[array[DSP]-6], array, LENGTH);

	return ret;
}


