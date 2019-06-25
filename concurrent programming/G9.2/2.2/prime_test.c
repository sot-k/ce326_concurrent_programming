//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prime_test.h"


int prime_test( int p){

	if (p < 2){
		return 0;
	}

	for (size_t i = 2; i < p; i++) {
		if (p % i == 0) {
			return 0;
		}
	}

	return 1;
}
