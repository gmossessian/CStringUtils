#include "CStringUtils.h"

static string __bigIntAdd(string a, string b);
static string __bigIntSubtract(string a, string b);
static int __bigIntComp(string a, string b);
static string *__bigIntDivide(string a, string b);
static string *__findQuotient(string divisor, string dividend, string minq, string maxq);
static string __modExpOdd(string b, string e, string n);
static string __binaryModPow_modpoweroftwo(string base, string exp, uint64_t modpow);
string __modExpEven(string a, string e, string n);
string __modExpOdd(string a, string e, string n);
string __monPro(string abar, string bbar, string n, string nprime, int rpow);


string bigIntAdd(string a, string b){   //right-to-left addition
	string R;
	if(a.sign == 1 && b.sign == 1) R = __bigIntAdd(a,b);
	if(a.sign == -1 && b.sign == 1) R = __bigIntSubtract(b,a);
	if(a.sign == 1 && b.sign == -1) R = __bigIntSubtract(a,b);
	if(a.sign == -1 && b.sign == -1){
		R = __bigIntAdd(a,b);
		R.sign = -1;
	}
	return R;
}

string bigIntSubtract(string a, string b){
	string R;
	if(a.sign == 1 && b.sign == -1)	return __bigIntAdd(a,b);
	if(a.sign == -1 && b.sign == -1){
		 return __bigIntSubtract(b,a);
	 }
	if (a.sign == -1 && b.sign == 1){
		R = __bigIntAdd(a,b);
		R.sign = -1;
		return R;
	}
	return __bigIntSubtract(a,b);
}

string __bigIntAdd(string a, string b){ //assumes a, b positive
	/*
	 * Assume both a and b are positive. Use stringadd(string, string) to deal with signs.
	 * Adds two bigints stored as big endian raw bytes in a string object.
	 * Just adds one byte at a time and carries the 1.
	 */
	string r = NULLSTRING;
	int i,j, sum;
	unsigned char c;
	string shorter;
	string longer;

	shorter = LOCALSTRING((a.len<b.len ? a : b));
	longer = LOCALSTRING((a.len<b.len ? b : a));

	for(i=0; i<shorter.len; i++){

		sum = (int)((unsigned char)shorter.c[shorter.len - i - 1])+(int)((unsigned char)longer.c[longer.len - i - 1]);
		c = (unsigned char)(sum - (sum >= 256 ? 256 : 0));

		r = stringCat(charToS(c),r);
		j = longer.len - i - 1;
		while( sum >= 256 ) {
			if(--j == -1){
				longer = stringCat(charToS(0x01), longer);
				break;
			}
			else{
				sum = 1 + (int)((unsigned char)longer.c[j]);
				c = (unsigned char)(sum - (sum >= 256 ? 256 : 0));
				longer.c[j]=c;
			}
		}
	}
	for(j=longer.len-i-1; j>=0; j--){
		r = stringCat(charToS(longer.c[j]),r);
	}

	return r;
}

string __bigIntSubtract(string a, string b){ //assumes a,b positive
	if(__bigIntComp(a,b)==0) return charToS(0x00);

	string r=NULLSTRING;
	int i,j,diff;
	unsigned char c;

	string smaller = bigIntCopy(((__bigIntComp(a,b)==1) ? b : a));
	string bigger  = bigIntCopy(((__bigIntComp(a,b)==1) ? a : b));
	
	if(smaller.len==0) return bigger;

	for(i=0; i<smaller.len; i++){
		diff = (int)((unsigned char)bigger.c[bigger.len - i - 1]) - (int)((unsigned char)smaller.c[smaller.len - i - 1]);
		c = (unsigned char)(diff + (diff < 0 ? 256 : 0));
		r = stringCat(charToS(c),r);
		j = bigger.len - i - 1;
		while(diff < 0){
			if (--j==-1){//this shouldn't ever happen, since we're doing bigger - smaller
				fprintf(stderr, "Something went wrong with stringSubtract.\n");
				exit(1);
			}
			else{
				diff = (int)((unsigned char)bigger.c[j]) - 1;
				c = (unsigned char)(diff + (diff < 0 ? 256 : 0));
				bigger.c[j] = c;
			}
		}
	}
	for(j = bigger.len - i - 1; j>=0; j--){
		r = stringCat(charToS(bigger.c[j]), r);
	}
	r = stripLeadingZeroes(r);
	r.sign = __bigIntComp(a,b);
	return r;
}

string bigIntMultiply(string a, string b){
	/*
	 * Multiplies two bigint strings.
	 * It's just regular multiplication by hand, using the binary representation
	*/
	int pow = ispowerof2(a);
	if(pow!=-1){
		string r = stringLeftShift(b,pow);
		r.sign = a.sign*b.sign;
		return r;
	}
	pow = ispowerof2(b);
	if(pow!=-1){
		 string r = stringLeftShift(a,pow);
		 r.sign = a.sign*b.sign;
		 return r;
	 }
	
	string r = charToS(0x00);
	uint64_t l = a.len<<3;
	uint64_t i;
	for(i=0; i<l; i++){
		if(bigIntParity(stringRightShift(a,i))==1)
			r = bigIntAdd(r,stringLeftShift(b,i));
	}
	r.sign = a.sign * b.sign;
	return r;
}

string bigIntModExp(string base, string exp, string mod){
	if(bigIntParity(mod)==1){
		return __modExpOdd(base, exp, mod);
	}
	return __modExpEven(base, exp, mod);
}

int __bigIntComp(string a, string b){ // assumes a,b positive
	string A = stripLeadingZeroes(a);
	string B = stripLeadingZeroes(b);

	if(A.len<B.len) return -1;
	if(A.len>B.len) return 1;

	int i=0;
	while(i<A.len){
		//printf("comparing: %02x %02x: \n", A.c[i], B.c[i]);
		if((uint8_t)A.c[i] > (uint8_t)B.c[i]) return 1;
		if((uint8_t)A.c[i] < (uint8_t)B.c[i]) return -1;
		i++;
	}
	return 0;
}

int bigIntComp(string a, string b){ //compares a, b, as bigints, returning 1 if a>b, -1 if a<b, 0 if a==b.
	if (a.sign == b.sign){
		if(a.sign == 1){
			return __bigIntComp(a,b);
		}
		if(a.sign==-1){
			return __bigIntComp(b,a);
		}
	}
	return a.sign; //return 1 if a is positive, -1 if a is negative...
}

int bigIntParity(string a){ //returns the last bit of a.
	return a.c[a.len-1] & 0x1;
}

string stripLeadingZeroes(string a){
	int i=0;
	//printf("sLZ: ");printsint(a);printf(" len = %i, ",a.len);PRINTNL;
	if(a.len==0) return a;
	if(a.c[0]!=0) return a;
	while((unsigned char)a.c[i]==0 && i<a.len) i++;
	//printf("i=%i.\n",i);
	if(i==a.len) return NULLSTRING;
	return newBigInt(&a.c[i],a.len-i,a.sign);
}

int ispowerof2(string a){ //returns -1 if not, otherwise returns the power
	int p=-1;
	for(int i=0; i<a.len; i++){
		for(int j=0; j<8; j++){
			if(((a.c[i] >> j) & 1)){
				if(p!=-1) return -1;
				p = ((a.len-i-1)<<3)+j;
			}
		}
	}
	return p;
}

string *__bigIntDivide(string a, string b){ //assumes a, b positive
	string *qr = calloc(2, sizeof(string));
	if(__bigIntComp(b,charToS(0x00))==0){
		return NULL;
	}
	if(__bigIntComp(a,b)==-1){
		qr[0] = charToS(0x00);
		qr[1] = bigIntCopy(b);
		return qr;
	}
	if(__bigIntComp(a,b)==0){
		qr[0] = charToS(0x01);
		qr[1] = charToS(0x00);
		return qr;
	}
	int pow = ispowerof2(b);
	if(pow!=-1){
		qr[0] = stringRightShift(a,pow);
		qr[1] = getSignificantBits(a,pow);
		qr[0].sign=1;
		qr[1].sign=1;
		return qr;
	}
	//long division, in base-256
	string divisor = charToS(a.c[0]);
	string quotient = NULLSTRING;
	//string q;
	int i=1;
	do{
		
		while(__bigIntComp(divisor,b)==-1 && i<a.len){ 
			divisor = stringCat(divisor, charToS(a.c[i]));
			i++;
		}
		if(i == a.len && __bigIntComp(divisor, b) == -1){
			break;
		}
		qr = __findQuotient(divisor, b, charToS(0x00), charToS(0xFF));
		quotient = stringCat(quotient, qr[0]);
		divisor = qr[1];
	}while(i<a.len);
	qr[0] = bigIntCopy(quotient);
	qr[1] = bigIntCopy(divisor);
	return qr;
}

string *__findQuotient(string divisor, string dividend, string minq, string maxq){ //an O(log n) division routine, finds q by a binary search instead of linear
	string q = stringRightShift(bigIntAdd(minq,maxq),1);
	string diff = stringRightShift(bigIntSubtract(maxq,minq),1);
	string p = bigIntMultiply(dividend, q);
	string *qr=calloc(2,sizeof(string));

	while( __bigIntComp(diff, charToS(0x00)) == 1){
		fflush(stdout);
		if(__bigIntComp(divisor, p) == 1){ // if divisor > dividend *q, q is too small.
			q = bigIntAdd(q, diff);
		}
		if(__bigIntComp(divisor, p) == -1){ // if divisor < dividend * q, q is too big.
			q = bigIntSubtract(q, diff);
		}
		if(__bigIntComp(divisor, p)==0){
			qr[0] = q;
			qr[1] = charToS(0x00);
			return qr;
		}
		p = bigIntMultiply(dividend, q);
		diff = stringRightShift(diff,1);
	}
	while(__bigIntComp(divisor, p)==1){ // if a > b*q, q is too small, so increment it. Afterwards, it will be 1 too big.
		q = bigIntIncr(q);
		p = bigIntAdd(p, dividend);
	}
	while(__bigIntComp(divisor, p) == -1){ // while a < b*q, decrement q
		q = bigIntDecr(q);
		p = bigIntSubtract(p,dividend);
	}
	qr[0] = q;
	qr[1] = bigIntSubtract(divisor, p);
	return qr;
}

string *bigIntDivide(string a, string b){ //returns [quotient, remainder]
	string *qr = __bigIntDivide(a,b);
	
	if(a.sign == -1 && b.sign == -1){
		bigIntIncr(qr[0]);
		qr[1] = bigIntSubtract(b,qr[1]);
	}
	else if(a.sign == 1 && b.sign == -1){
		qr[0].sign = -qr[0].sign;
	}
	else if(a.sign == -1 && b.sign == 1){
		qr[0] = bigIntIncr(qr[0]);
		qr[0].sign = -1;
		qr[1] = bigIntSubtract(b,qr[1]);
	}
	return qr;
}

string bigIntIncr(string a){ //returns a++
	return bigIntAdd(a,charToS(0x01));
}

string bigIntDecr(string a){ //returns a--
	return bigIntSubtract(a,charToS(0x01));
}

string *extendedEuclidean(string A, string B, string C){
	string S,s,T,t,R,r,q,temp;
	
	S=charToS(0x00);
	s=charToS(0x01);
	T=charToS(0x01);
	t=charToS(0x00);
	R=bigIntCopy(B);
	r=bigIntCopy(A);
	int revflag = 0;
	if(bigIntComp(A,B)==-1){
		temp = R;
		R = r;
		r = temp;
		revflag = 1;
	}
	
	while(bigIntComp(R,charToS(0x00))!=0){
		q = bigIntDivide(r,R)[0];
		memcpy(&temp, &R, sizeof(string)); R = bigIntSubtract(r, bigIntMultiply(q, R)); memcpy(&r, &temp, sizeof(string));
		memcpy(&temp, &S, sizeof(string)); S = bigIntSubtract(s, bigIntMultiply(q, S)); memcpy(&s, &temp, sizeof(string));
		memcpy(&temp, &T, sizeof(string)); T = bigIntSubtract(t, bigIntMultiply(q, T)); memcpy(&t, &temp, sizeof(string));
	}
	//at this point, s*A + t*B = r = gcd(A,B), and T,S are quotients of A,B by the gcd.
	string *qr = bigIntDivide(C,r);
	string ratio = bigIntCopy(qr[0]);
	if(bigIntComp(qr[1],charToS(0x00))!=0) return NULL;
	if(revflag==1){
		temp = s;
		s = t;
		t = temp;
	}
	
	//normalise s, t so that 0 <= s < B
	if(s.sign==-1){
		qr = bigIntDivide(bigIntAbs(s),bigIntAbs(B));
		string q = (bigIntComp(qr[1],charToS(0x00))==0) ? qr[0] : bigIntIncr(qr[0]);
		s = bigIntAdd(s,bigIntMultiply(q,bigIntAbs(B)));
		t = bigIntSubtract(t, bigIntMultiply(q,bigIntAbs(A)));
	}
	//multiply to get the correct coefficients
	s = bigIntMultiply(s,ratio);
	t = bigIntMultiply(t,ratio);
	string *ret = calloc(2,sizeof(string));
	ret[0] = s;
	ret[1] = t;
	return ret;
}


/*string *extendedEuclidean(string A, string B, string C){
	printf("taking egcd(0x");printsint(A);printf(", 0x");printsint(B);printf(", 0x");printsint(C);printf(")\n");fflush(stdout);
	string a = bigIntCopy(A);
	string b = bigIntCopy(B);
	string c = bigIntCopy(C);
	int revflag = 0;
	if(bigIntComp(a,b)==-1){
		string temp = a;
		a = b;
		b = temp;
		revflag = 1;
		printf("Swapped: ");printsint(a); printf(" ");printsint(b);PRINTNL;fflush(stdout);
	}
	string *ret = calloc(2,sizeof(string));
	string *qr = bigIntDivide(a,b);
	string r[3] = {bigIntCopy(a),bigIntCopy(b),bigIntCopy(qr[1])};
	string q = qr[0];
	string s[3] = {charToS(0x01),charToS(0x00),charToS(0x01)};
	string t[3] = {charToS(0x00),charToS(0x01),q};
	t[2].sign = -t[2].sign;
	string tmpArr[3];
	if(bigIntComp(r[2],charToS(0x00))==0){
		qr = bigIntDivide(c,b);
		if(bigIntComp(qr[1],charToS(0x00))==0){
			if(revflag == 0){
				ret[0] = charToS(0x00);
				ret[1] = bigIntCopy(qr[0]);
				return ret;
			}
			ret[0] = bigIntCopy(qr[0]);
			ret[1] = charToS(0x00);
			return ret;
		}
		return NULL;
	}
	while(bigIntComp(r[2],charToS(0x00))==1){
		qr = bigIntDivide(r[1],r[2]);
		tmpArr[0]=r[1]; tmpArr[1]=r[2]; tmpArr[2]=qr[1]; memcpy(r, tmpArr, 3*sizeof(string));
		tmpArr[0]=s[1]; tmpArr[1]=s[2]; tmpArr[2]=bigIntSubtract(s[1],bigIntMultiply(qr[0],s[2])); memcpy(s, tmpArr, 3*sizeof(string));
		tmpArr[0]=t[1]; tmpArr[1]=t[2]; tmpArr[2]=bigIntSubtract(t[1],bigIntMultiply(qr[0],t[2])); memcpy(t, tmpArr, 3*sizeof(string));
	}
	qr = bigIntDivide(c,r[1]);

	if (bigIntComp(qr[1], charToS(0x00)) != 0){
		return NULL;
	}

	if (revflag == 1){
		memcpy(tmpArr, s, 3*sizeof(string));
		memcpy(s, t, 3*sizeof(string));
		memcpy(t, tmpArr, 3*sizeof(string));
	}

	ret[0] = bigIntMultiply(qr[0],s[1]);
	ret[1] = bigIntMultiply(qr[0],t[1]);

	while(ret[0].sign == -1){
		printf("adjusting egcd output.\n");
		printsint(ret[0]);printf(" ");printsint(ret[1]);PRINTNL;fflush(stdout);
		ret[0] = bigIntAdd(ret[0], bigIntAbs(b));
		ret[1] = bigIntSubtract(ret[1],bigIntAbs(a));
		sleep(1);
	}
	printf("\t");printsint(ret[0]);printf(" ");printsint(ret[1]);PRINTNL;fflush(stdout);

	return ret;
}*/

string bigIntAbs(string a){
	string A = bigIntCopy(a);
	A.sign = 1;
	return A;
}	

string __modExpOdd(string a, string e, string n){
	string r = charToS(0x01);
	int rpow=0;
	int ebits  = countSignificantBits(e);
	while(__bigIntComp(r,n)==-1){
		r = stringLeftShift(r,1);
		rpow++;
	}
	string nprime = extendedEuclidean(r,n,charToS(0x01))[1];
	nprime.sign = 1;
	string abar = bigIntDivide(bigIntMultiply(a,r),n)[1];
	string xbar = bigIntDivide(r,n)[1];
	for(int i = ebits-1; i>=0; i--){
		xbar = __monPro(xbar, xbar, n, nprime, rpow);
		if(bigIntParity(stringRightShift(e,i))){
			xbar = __monPro(abar, xbar, n, nprime, rpow);
		}
	}
	string ret =  __monPro(xbar, charToS(0x01), n, nprime, rpow);
	string x = __monPro(xbar, charToS(0x01), n, nprime, rpow);
	return x;
}

/*string __modExpOdd(string a, string e, string n){
	if(bigIntComp(e, charToS(0x00))==0) return charToS(0x01);
	if(bigIntComp(n, charToS(0x01))==0) return charToS(0x00);
	string E = bigIntCopy(e);
	string A = bigIntCopy(a);
	while(bigIntComp(E,charToS(0x01))==1){
		A = bigIntMultiply(A,A);
		if(bigIntParity(E)==1){
			A = bigIntMultiply(A,a);
		}
		A = bigIntDivide(A,n)[1];
		E = stringRightShift(E,1);
	}
	return A;
}*/

string __modExpEven(string a, string e, string n){
	string q = bigIntCopy(n);
	string j = charToS(0x01);
	uint64_t jpow = 0;
	while(bigIntParity(q)==0){
		q = stringRightShift(q,1);
		j = stringLeftShift(j,1);
		jpow++;
	}
	//printf("q = ");printsint(q);PRINTNL;
	//printf("jpow = %lu, power = %i\n",jpow, ispowerof2(j));
	//printf("j = ");printsbinary(j);PRINTNL;
	string A = bigIntDivide(a,q)[1];
	string x1 = __modExpOdd(A,e,q);
	//printf("x1 = ");printsint(x1);PRINTNL;
	A = getSignificantBits(a,jpow);
	string E = getSignificantBits(e, jpow-1);
	string x2 = __binaryModPow_modpoweroftwo(A,E,jpow);
	//printf("x2 = ");printsint(x2);PRINTNL;
	string qinv = extendedEuclidean(q,j,charToS(0x01))[0];
	//printf("qinv = ");printsint(qinv);PRINTNL;fflush(stdout);
	string y = bigIntDivide(bigIntSubtract(x2,x1),j)[1];
	y = getSignificantBits(bigIntMultiply(y,qinv),jpow);
	//if(y.sign==-1) y = bigIntAdd(y,j);
	//printf("y = ");printsint(y);PRINTNL;fflush(stdout);
	string x = bigIntAdd(x1,bigIntMultiply(q,y));
	//printf("x = ");printsint(x);PRINTNL;
	return x;
}

string __monPro(string abar, string bbar, string n, string nprime, int rpow){
	string t = bigIntMultiply(abar, bbar);
	string m = getSignificantBits(bigIntMultiply(t,nprime),rpow);
	string u = stringRightShift(bigIntAdd(t,bigIntMultiply(m,n)),rpow);
	if(bigIntComp(u,n)>=0) return bigIntSubtract(u,n);
	return u;
}

string __binaryModPow_modpoweroftwo(string base, string exp, uint64_t modpow){
	if(modpow == 0) return charToS(0x00);
	string res = charToS(0x01);
	string B = getSignificantBits(base, modpow);//bigIntDivide(base, mod)[1];
	string E = bigIntCopy(exp);
	while(bigIntComp(E,charToS(0x00))==1){
		if(bigIntParity(E)==1) res = getSignificantBits(bigIntMultiply(res, B), modpow);
		E = stringRightShift(E,1);
		B = getSignificantBits(bigIntMultiply(B,B),modpow);
	}
	return res;
}

uint32_t countSignificantBits(string a){
	string A = stripLeadingZeroes(a);
	if(A.len==0) return 0;
	uint32_t nbits = (A.len-1)<<3;
	uint32_t i;
	for(i=7; i>0; i--){
		if((A.c[0] >> i) & 0x01) break;
	}
	nbits += (i+1);
	return nbits;
}

string getSignificantBits(string a,  uint64_t nbits){
	if ((a.len<<3)<= nbits){
		return a;
	}
	if (nbits==0){
		return charToS(0x00);
	}
	uint64_t nbytes = (nbits >> 3)+1;
	uint8_t bitmask = 0x01;
	bitmask = (bitmask << (nbits%8))-1;
	string A = newBigInt(&a.c[a.len-nbytes],nbytes,1);
	A.c[0] &= bitmask;
	return A;
}

