#include <string.h>
#include <amplifier.h>
#include <mustanghero.h>
#include <mustang.h>

void start_amp()
{
	memset(names, 0x00, 100*32);

	int x;
	struct amp_settings amplifier_set;
	struct fx_pedal_settings effects_set[4];
	char name[32];

	x = mustang_startAmp(names, name, &amplifier_set, effects_set);

	if (x != 0)
	{
		if (x == -100)
		{
			printf("[Mustang Driver] Sutible device not found!\n");
		}
		else
		{
			printf("[Mustang Driver] Error: %d\n", x);
		}

		return;
	}
	else
	{
		printf("[Mustang Driver] Connected to amplifier!\n");
	}
}
