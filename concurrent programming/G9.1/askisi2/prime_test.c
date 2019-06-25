//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prime_test.h"

// #define ITERATION 5000
// // Prime test using Miller-Rabin Primality test, ITERATION signifies the accuracy
//
// /*
//
//  * calculates (a * b) % c taking into account that a * b might overflow
//
//  */
//
// long mulmod(long a, long b, long mod){
//
// 	long x = 0,y = a % mod;
//
// 	while (b > 0){
//
// 		if (b % 2 == 1){
// 			x = (x + y) % mod;
// 		}
//
// 		y = (y * 2) % mod;
// 		b /= 2;
// 	}
// 	return x % mod;
//
// }
//
// /*
//
//  * modular exponentiation
//
//  */
//
// long modulo(long base, long exponent, long mod){
//
// 	long x = 1;
//
// 	long y = base;
//
// 	while (exponent > 0){
//
// 		if (exponent % 2 == 1){
// 			x = (x * y) % mod;
// 		}
//
// 		y = (y * y) % mod;
// 		exponent = exponent / 2;
// 	}
// 	return x % mod;
//
// }
//
//
//
// /*
//
//  * Miller-Rabin Primality test, ITERATION signifies the accuracy
//
//  */
//
// int prime_test( int p){
// 	int i;
// 	long s;
//
// 	if (p < 2){
// 		return 0;
// 	}
//
// 	if (p != 2 && p % 2==0){
// 		return 0;
//
// 	}
//
// 	 s = p - 1;
//
// 	while (s % 2 == 0){
// 		s /= 2;
//
// 	}
//
// 	for (i = 0; i < ITERATION; i++){
//
// 		long a = rand() % (p - 1) + 1, temp = s;
//
// 		long mod = modulo(a, temp, p);
//
// 		while (temp != p - 1 && mod != 1 && mod != p - 1){
// 			mod = mulmod(mod, mod, p);
// 			temp *= 2;
// 		}
//
// 		if (mod != p - 1 && temp % 2 == 0){
// 			return 0;
// 		}
//
// 	}
//
// 	return 1;
// }

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
