#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "validationUtils.h"


int isNumber(char *str)
{
	while (*str) {
		if (!isdigit(*str)) {
			return 0; // Return false - not a number
		}
		str++;
	}
	return 1; // Return true - this is a number
}


int ipValidate(char *ip) // Check if the IP is valid
{
	int num, dots = 0;
	char* test = ip;
	char *ptr;

	if (ip == NULL) {
		return 0; // Return false
	}

	// Check amt of dots
	int stringLength = strlen(ip);
	int overallDots = 0;
	for (int i = 0; i < stringLength; i++)
	{
		if (ip[i] == '.') {
			overallDots++;
		}
	}

	ptr = strtok(test, "."); // Slice the IP by dots
	if (ptr == NULL) {
		return 0; // Return false - there is no number after the dot
	}
	while (ptr) {
		if (!isNumber(ptr)) { // Check whether the sub string is holding only number or not
			return 0;
		}
		num = atoi(ptr); // Convert substring to number
		if (num >= 0 && num <= 255) {
			ptr = strtok(NULL, "."); // Go the next part of the IP
			if (ptr != NULL) {
				dots++;
			}
		}
		else {
			return 0;
		}
	}
	if (overallDots != 3) // If the number of dots are not 3, return false
		return 0;
	return 1;
}