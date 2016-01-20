#include "CStringUtils.h"

int main(int argc, char **argv){
	//test addition, subtraction
	string a = charToS(0x0f);
	string b = charToS(0x01);
	for(int i=0; i<a.len; i++) a.c[i]=0xFF;
	string r = bigIntAdd(a,b);
	//printsint(a);printf(" + ");printsint(b);printf(" = ");printsint(r);PRINTNL;
	
	//a=newBigInt(r.c, r.len, -1);
	//printsint(a);printf(" + ");printsint(b);printf(" = ");printsint(bigIntAdd(a,b));PRINTNL;
	
	/*a = randString(3);
	a.sign = -1;
	b=charToS(0xa5);
	r = bigIntAdd(a,b);
	printsint(a);printf(" + ");printsint(b);printf(" = ");printsint(r);
	r = bigIntAdd(b,a);
	printf(" = ");printsint(r);PRINTNL;*/
	
	
	srand(time(NULL));
	a = randString(4);
	b = randString(2);
	//printsint(a);printf(" / \n");
	//printsint(b);printf(" = \n");
	string *qr = bigIntDivide(a, b);
	//printsint(qr[0]);printf(" R: "); printsint(qr[1]);PRINTNL;
	if(bigIntComp(a,bigIntAdd(bigIntMultiply(b,qr[0]),qr[1]))==0){
		printf("Division checks out!\n");
	}
	
	string n;
	if(1){
		string *gcd;
		do{
			a = randString(5);
			b = randString(5);	
			n = randString(3);
			gcd = extendedEuclidean(a,b,n);
		}while(gcd==NULL);
		printsint(a);printf("*");printsint(gcd[0]);printf("+");printsint(b);printf("*");printsint(gcd[1]);printf("=");printsint(n);PRINTNL;
		if(bigIntComp(bigIntAdd(bigIntMultiply(a,gcd[0]),bigIntMultiply(b,gcd[1])),n)==0){
			printf("EGCD working.\n");
		}
	
		else printf("EGCD failed!\n");
		fflush(stdout);
	}
	
	//testing Montgometry reduction product
	/*a = charToS(0x02);
	n = charToS(0x05);
	r = charToS(0x01);
	int rpow=0;
	while(bigIntComp(r,n)==-1){
		r = stringLeftShift(r,1);
		rpow++;
	}
	//printf("r = ");printsint(r);PRINTNL;
	string nprime = extendedEuclidean(r,n,charToS(0x01))[1];
	nprime.sign = 1;
	//printf("nprime = ");printsint(nprime);PRINTNL;fflush(stdout);
	string abar = bigIntDivide(bigIntMultiply(a,r),n)[1];
	//printf("abar = "); printsint(abar);PRINTNL;fflush(stdout);
	printsint(a);printf(" ** 2 mod ");printsint(n);printf(" = ...");PRINTNL;
	string rbar = __monPro(abar, abar, n, nprime, rpow);
	r = __monPro(rbar, charToS(0x01), n, nprime, rpow);
	printf("...");printsint(r);PRINTNL;*/
	
	//testing modexp;
	a = base16Decode(newString("0177",0));
	b = base16Decode(newString("f9",0));
	n = base16Decode(newString("0184",0));
	printsint(a);printf(" ** ");printsint(b);printf(" mod ");printsint(n);printf(" = \n");
	printsint(bigIntModExp(a,b,n));PRINTNL;
	
	
}
