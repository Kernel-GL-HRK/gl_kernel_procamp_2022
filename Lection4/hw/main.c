#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "helpers.h"



// All available clocks.
static clockid_t clock_ids [] = {
	CLOCK_REALTIME
  , CLOCK_REALTIME_COARSE
  , CLOCK_MONOTONIC
  , CLOCK_MONOTONIC_COARSE
  , CLOCK_MONOTONIC_RAW
  , CLOCK_BOOTTIME
  , CLOCK_PROCESS_CPUTIME_ID
  , CLOCK_THREAD_CPUTIME_ID
};

// Clocks names.
static const char * clock_names [] = {
	"CLOCK_REALTIME"
  , "CLOCK_REALTIME_COARSE"
  , "CLOCK_MONOTONIC"
  , "CLOCK_MONOTONIC_COARSE"
  , "CLOCK_MONOTONIC_RAW"
  , "CLOCK_BOOTTIME"
  , "CLOCK_PROCESS_CPUTIME_ID"
  , "CLOCK_THREAD_CPUTIME_ID"
};

// Helper variables used for pretty output.
static const char * clock_names_align [] = {
	" ........... "
  , " .... "
  , " .......... "
  , " ... "
  , " ...... "
  , " ........... "
  , " . "
  , " .. "
};



int main ()
{
	int clock_count = sizeof (clock_ids) / sizeof (clock_ids [0]);
	int i;					// A common counter.
	struct timespec time;	// A common variable for obtaining results.
	const int nano = 9;		// Simple constant (try to avoid a magic number).
	struct prefix_t prefix;	// A helper structure for forming pretty fraction output.


	memset (& time, sizeof (time), 0x00);

	if (init_prefix (& prefix, nano, malloc) ) {
		return EXIT_FAILURE;
	}


	for (i = 0; i < clock_count; ++i) {
		if (!clock_gettime (clock_ids [i], & time) ) {
			printf ("Time for %s is%s%ld.%s%ld\n", clock_names [i], clock_names_align [i]
										, time.tv_sec, update_prefix (& prefix, time.tv_nsec)
										, normalize_fraction (time.tv_nsec) );
		} else {
			printf ("Can't get time for %s, due to: %s.\n", clock_names [i], strerror (errno) );
			continue;
		}
	}


	for (i = 0; i < clock_count; ++i) {
		if (!clock_getres (clock_ids [i], & time) ) {
			printf ("Resolution for %s is%s%ld.%s%ld\n", clock_names [i], clock_names_align [i]
										, time.tv_sec, update_prefix (& prefix, time.tv_nsec)
										, normalize_fraction (time.tv_nsec) );
		} else {
			printf ("Can't get resolution for %s, due to: %s.\n", clock_names [i], strerror (errno) );
			continue;
		}
	}


	// Make GCC happy (`free` takes a `void*` but we need `const void *`).
	release_prefix (& prefix, (void (*) (const void *) ) free);


	return EXIT_SUCCESS;
}
