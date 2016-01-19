/*
 * CStringUtils.h
 *
 *  Created on: Jan 6, 2016
 *      Author: George Mossessian
 */

#ifndef __CSTRINGUTILS_H__
#define __CSTRINGUTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

#define ABS(X) 	 ((X)<0?-(X):X)
#define MIN(X,Y) (((X)>(Y))?(Y):(X))
#define MAX(X,Y) (((X)>(Y))?(X):(Y))
#define BASE64VAL(X) (strchr(BASE64,(X))-BASE64)    //base64 value of a char '0-9a-zA-Z+/'
#define BASE16VAL(X) (strchr(BASE16,(X))-BASE16)	//hex value of a char '0-9a-f'
#define PRINTNL printf("\n")						//print newline
#define NULLSTRING (newString(NULL,0))				//empty string
#define LOCALSTRING(S) (newString(S.c,S.len))		//make local copy of string
#define BIGINTCOPY(B) (newBigInt(B.c, B.len, B.sign))// make a local copy of the bigint

static const char *BASE16 = "0123456789abcdef=";
static const char *BASE64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

typedef struct string{
	char *c;
	int len;
	char sign;
}string;



string newString(char *, int);
string newBigInt(char *, int, char);
string bigIntCopy(string s);

string 	base64Encode(string);
int 	isBase16(string);
string 	base16Decode(string);
string 	base16Encode(string);
string 	base64Decode(string);


int HammingDistance(string str1, string str2);

string *blockString(string str, int bs);
int numBlocks(string str, int bs);
string *invertBlocks(string str, int bs);
int blockRepeats(string str, int bs);
string bigIntCopy(string s);

string PKCS7PadString(string, int);
string stringCat(string first, string second);

uint32_t stringToUint32(string s);
string uint32ToString(uint32_t);

int stringComp(string, string);													
int stringCompN(string, string, int);											

int validatePKCS7Padding(string str);											
string stripPKCS7Padding(string str);											

string stripChars(string in, string chars);										

string randString(int len);														

string stringLeftRotate(string word, int bits);									
string stringLeftShift(string word, int bits);
string stringRightShift(string word, int bits);
string bigIntAdd(string a, string b);
string bigIntIncr(string a);
string bigIntDecr(string a);
string bigIntSubtract(string a, string b);
string bigIntMultiply(string a, string b);
string *bigIntDivide(string a, string b);
string *extendedEuclidean(string a, string b, string n);
string stripLeadingZeroes(string a);
int bigIntComp(string a, string b);
int bigIntParity(string a);
string bigIntAbs(string a);
string charToS(char c);
int ispowerof2(string a); //returns -1 if not, otherwise returns the power

string getSignificantBits(string a, uint64_t nbits);
uint32_t countSignificantBits(string a);
string bigIntModExp(string base, string exp, string mod);
string stringXOR(string, string);												

void prints(string);
void printsprint(string s);
void printsint(string);
void printsbinary(string s);
string readLine(void);
string readInput(string end);

string readInputFromFile(FILE *fp);

#endif
