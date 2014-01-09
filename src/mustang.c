#include <mustang.h>
#include <libusb-1.0/libusb.h>

libusb_device_handle *amp_hand;  // Handle for USB Communication

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


