#include "random.h"

#include <time.h>
#include <stdlib.h>

namespace Random
{
	void setup()
	{
		srand(time(NULL));
	}

	double next()
	{
		return (double)rand()/(double)RAND_MAX - 0.5;
	}
}