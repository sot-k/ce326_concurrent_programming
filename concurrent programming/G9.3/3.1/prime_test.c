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
	int i;
	if (p < 2){
		return 0;
	}

	for ( i = 2; i < p; i++) {
		if (p % i == 0) {
			return 0;
		}
	}

	return 1;
}
