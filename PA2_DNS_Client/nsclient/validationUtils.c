#define _CRT_SECURE_NO_WARNINGS

/*
	Functions to validate user input for correctness before constructing the DNS packets.
	If the user inputs garbage, we don't want to send this garbage out because we know
	that the reply will also be garbage.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "validationUtils.h"


int isNumber(char *str)
{
	// Check if a character is a number
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
	// Validate given DNS server IP address, 
	// kill program if the IP address is invalid.

	int num, dots = 0;
	char *ptr;

	if (ip == NULL) {
		return 0; // Return false
	}

	char* strToTest = (char*)malloc(sizeof(char) * strlen(ip));
	strcpy(strToTest, ip);

	// Check amt of dots
	int stringLength = strlen(strToTest);
	int overallDots = 0;
	for (int i = 0; i < stringLength; i++)
	{
		if (strToTest[i] == '.') {
			overallDots++;
		}
	}

	ptr = strtok(strToTest, "."); // Slice the IP by dots
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


int checkDomain(char* domain) {
	// Check URL validity based on RFC https://www.rfc-editor.org/rfc/rfc3986#section-1.1.1
	//	1. Check atleast 1 dot and no more than 1 dot in a row.
	//	2. Check that the domain name is not longer than 255 characters - This is automatically true becuase of our
	//	   input buffer size, which is 255


	int retval = 1;
	int stringLength = strlen(domain);

	int numDots = 0;
	int currentSeenDot = 50;	// Magic numbers
	int prevSeenDot = 100;
	for (int i = 0; i < stringLength; i++)
	{
		if (domain[i] == '.') {
			numDots++;
			currentSeenDot = i;
		}

		if (currentSeenDot - prevSeenDot == 1) {
			// If two dots are seen one after another - URL is invalid.
			return 0;
		}
		prevSeenDot = currentSeenDot;
	}

	if (numDots < 1) {
		retval = 0;
	}
	
	return retval;
}