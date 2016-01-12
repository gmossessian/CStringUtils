/*
 * CStringUtils.c
 *
 *  Created on: Jan 6, 2016
 *      Author: George Mossessian
 *
 *
 */

#include "CStringUtils.h"

string __bigIntAdd(string a, string b);
string __bigIntSubtract(string a, string b);
string *__bigIntDivide(string a, string b);
int __bigIntComp(string a, string b);


string newString(char * c, int t){
	string r;
	int i;
	if(c!=NULL){
		r.len = (t<=0)?strlen(c):t;
		r.c=(char *)malloc(sizeof(char)*(r.len+1));
		for(i=0; i<r.len; i++) r.c[i]=c[i];
		//strncpy(r.c, c,r.len); /* size_t maxes out on long strings like base64 encoded ciphers of vanilla ice lyrics. */
		r.c[r.len]='\0';
		return r;
	}
	r.len=t;
	r.c=(char *)malloc(sizeof(char)*(r.len+1));
	memset(r.c,(char)0,sizeof(char)*(t+1));
	r.sign = 1;
	return r;
}

string newBigInt(char *c, int t, char s){
	string r = newString(c,t);
	r.sign = s;
	return r;
}

string bigIntCopy(string s){
	return newBigInt(s.c, s.len, s.sign);
}

void prints(string s){
	int i;
	for(i=0; i<s.len; i++){
		printf("%c", s.c[i]);
		fflush(stdout);
	}
}

void printsprint(string s){
	for(int i=0; i<s.len; i++){
		if(isprint(s.c[i])) printf("%c",s.c[i]);
		else printf(" ");
		fflush(stdout);
	}
}

void printsint(string s){
	int i;
	if(s.sign == -1) printf("-");
	for(i=0; i<s.len; i++){
		printf("%02x", (unsigned char)s.c[i]);
	}
}

void printsbinary(string s){
	int i,j;
	for(i=0; i<s.len; i++){
		for(j=7; j>=0; j--){
			if((s.c[i] >> j) & 0x01) printf("1");
			else printf("0");
			fflush(stdout);
		}
	}
}

string readLine(void){
	unsigned int len=128;
	unsigned int i=0;
	string str;
	str.c = malloc(len*sizeof(char));
	int c;

	while((c=getchar())!='\n' && c!=EOF){
		str.c[i++]=(char)c;
		if(i==len){
			str.c=realloc(str.c,sizeof(char)*(len+i));
			len+=i;
		}
	}
	str.c=realloc(str.c, sizeof(char)*(i+1));
	str.c[i]='\0';
	str.len=i;
	return str;
}

string readInput(string end){
	unsigned int len=128;
	unsigned int i=0;
	int endlen=end.len;
	string str;
	str.c =malloc(len*sizeof(char));
	int c;
	if(!strcmp(end.c, "\n")) return readLine();
	do{
		c=getchar();
		str.c[i++]=c;
		if(i==len){
			str.c=realloc(str.c,sizeof(char)*(len+i));
			len+=i;
		}
		str.c[i]='\0';
	} while(strcmp(&str.c[MAX(0,i-endlen)], end.c));
	getchar();//if you don't do this, the last newline gets left in stdin...
	i-=endlen;
	str.c=realloc(str.c, sizeof(char)*(i+1));
	str.c[i]='\0';
	str.len=i;
	return str;
}

string readInputFromFile(FILE *fp){
	unsigned int len=128;
	unsigned int i=0;
	string str;
	str.c=malloc(len*sizeof(char));
	int c;
	while(!feof(fp)){
		c=fgetc(fp);
		str.c[i++]=c;
		if(i==len){
			str.c=realloc(str.c,sizeof(char)*(len+i));
			len+=i;
		}
	}
	str.c=realloc(str.c, sizeof(char)*(i+1));
	str.c[i]='\0';
	str.len=i;
	return str;
}

string base64Encode(string in){
	int i,j;
	int b;
	string out;

	out = newString(NULL, 4*((in.len+2)/3));

	//out.len = 4*in.len/3 + ((in.len%3)?1:0)+1;
	//out.c=malloc(sizeof(char)*(out.len));

	j=0;
	for(i=0;i<in.len;i+=3){
		b = (in.c[i] & 0xFC) >> 2;
		out.c[j++] = BASE64[b];
		b = (in.c[i]&0x03) << 4;
		if(i+1<in.len){
			b |= (in.c[i+1] & 0xF0) >> 4;
			out.c[j++]= BASE64[b];
			b= (in.c[i+1] & 0x0F) << 2;
			if (i+2 < in.len){
				b |= (in.c[i+2] & 0xC0) >> 6;
				out.c[j++]=BASE64[b];
				b = in.c[i+2] & 0x3F;
				out.c[j++]=BASE64[b];
			} else {
				out.c[j++] = BASE64[b];
				out.c[j++] = '=';
			}
		} else{
			out.c[j++]=BASE64[b];
			out.c[j++]='=';
			out.c[j++]='=';
		}
	}
	out.c[j]='\0';
	return out;
}

string base64Decode(string in){
	string out;
	int b[4];
	int i,j;

	if(in.len%4){
		fprintf(stderr, "Invalid Base64 input.\n");
		return newString(NULL,0);
	}
	i=BASE64VAL('=');
	out=newString(NULL, 4*(in.len+2)/3);
	j=0;
	for(i=0; i<in.len; i+=4){
		b[0]=BASE64VAL(in.c[i]);
		b[1]=BASE64VAL(in.c[i+1]);
		b[2]=BASE64VAL(in.c[i+2]);
		b[3]=BASE64VAL(in.c[i+3]);
		out.c[j++]=(char)((b[0] << 2) | (b[1] >> 4));
		if(b[2]<64){
			out.c[j++]=(char)((b[1]<<4 | b[2] >> 2));
			if(b[3]<64) out.c[j++]=(char)((b[2]<<6 | b[3]));
		}
	}
	out.c[j]='\0';
	out.len=j;
	return out;
}

int HammingDistance(string str1, string str2){
	int i,d=0;
	char c;
	for(i=0; i<MIN(str1.len,str2.len); i++){
		c=(str1.c[i]^str2.c[i]);
		do{d+=c&0x01; c=c>>1; }while(c);
	}
	return d;
}

string base16Encode(string in){
	string out=newString(NULL, in.len*2);
	int i,j;

	j=0;
	for(i=0; i<in.len; i++){
		out.c[j++]=BASE16[((in.c[i] & 0xF0)>>4)];
		out.c[j++]=BASE16[(in.c[i] & 0x0F)];
	}
	out.c[j]='\0';
	return out;
}

string base16Decode(string in){
	char b[2];
	int i,j;
	string out;

	out=newString(NULL,in.len/2+in.len%2);
	j=0;
	for(i=0; i<in.len; i++){
		b[i%2]=BASE16VAL(tolower(in.c[i]));
		if(i%2==1){
			out.c[j++]=(b[0]<<4)+b[1];
		}
	}
	if(i%2==1){
		out.c[j++]=b[0];
	}
	out.c[j]='\0';
	return out;
}

int isBase16(string str){
	int i;
	for(i=0; i<str.len; i++){
		if(isupper(str.c[i])) str.c[i]=tolower(str.c[i]);
		if(BASE16VAL(str.c[i])>16){
			return 0;
		}
	}
	return 1;
}

string *blockString(string str, int bs){
	int num = numBlocks(str, bs);
	string *blocks = (string *)malloc(sizeof(string)*(num+1));
	int i;

	for(i=0; i<num-1; i++){
		blocks[i]=newString(NULL, bs);
	}
	blocks[num-1]= newString(NULL, ((str.len % bs)? ( str.len % bs ) : bs ));

	blocks[num]=newString(NULL, 0);
	for(i=0; i<str.len; i++){
		blocks[i/bs].c[i%bs]=str.c[i];
	}
	return blocks;
}

int numBlocks(string str, int bs){
	return str.len/bs + (str.len%bs? 1: 0);
}

string *invertBlocks(string str, int bs){
	int numb=str.len/bs + ((str.len%bs)?1:0);
	string *res = malloc(sizeof(string)*(bs));
	int i,j;

	for(i=0; i<bs; i++){
		res[i]=newString(NULL, numb);
		for(j=0; j<numb; j++){
			if(j*bs+i<str.len){
				res[i].c[j]=str.c[j*bs+i];
			}
			else res[i].c[j]='\0';
		}
	}
	return res;
}

int blockRepeats(string str, int bs){
	string *blocks = blockString(str, bs);
	int i, j;
	int num=numBlocks(str, bs);

	for(i=0; i<num-1; i++){
		for(j=i+1; j<num; j++){
			if(!strncmp(blocks[i].c, blocks[j].c, blocks[i].len)){
				//printf("%i: ", i); printsint(blocks[i]);printf("\n%i: ", j); printsint(blocks[j]); printf("\n");
				return 1;
			}

		}
	}

	return 0;
}

string PKCS7PadString(string str, int bs){
	int d = bs - str.len%bs;
	int i;
	string pad;
	char finalblock[bs];

	if(d == bs){
		for(i=0; i<bs; i++) finalblock[i]=bs;
		return stringCat(str, newString(finalblock, bs));
	}

	pad = newString(str.c, str.len+d);
	for(i=str.len; i<pad.len; i++){
		pad.c[i]=(char)d;
	}
	return pad;
}

string stringCat(string first, string second){
	string str=newString(NULL, first.len+second.len);
	int i;

	for(i=0; i<first.len; i++){
		str.c[i]=first.c[i];
	}
	for(i=first.len; i<str.len; i++){
		str.c[i]=second.c[i-first.len];
	}
	return str;
}

int stringComp(string s1, string s2){
	int l=MIN(s1.len, s2.len);
	int i;
	for(i=0; i<l; i++){
		if(s1.c[i]!=s2.c[i]) return 0;
	}
	return 1;
}

int stringCompN(string s1, string s2, int l){
	int i;
	if(s1.len<l || s2.len <l) return 0;
	for(i=0; i<l; i++) if(s1.c[i]!=s2.c[i]) return 0;
	return 1;
}

int validatePKCS7Padding(string str){
	unsigned char final=(unsigned char)str.c[str.len-1];
	int i;
	if(final >= str.len || final == 0) return 0;
	for(i=str.len-1; i>=str.len-final; i--){
		if(str.c[i]!=final) return 0;
	}
	return 1;
}

string stripPKCS7Padding(string str){
	if(validatePKCS7Padding(str)==0) return str;
	return newString(str.c, str.len-str.c[str.len-1]);
}

string stripChars(string in, string chars){
	string res=newString(NULL,in.len);
	int i,j,k;
	int flag;

	j=0;
	for(i=0; i<in.len; i++){
		flag=1;
		for(k=0; k<chars.len; k++){
			if(in.c[i]==chars.c[k]){
				flag=0;
				break;
			}
		}
		if(flag){
			res.c[j++]=in.c[i];
		}
	}
	return newString(res.c,j);
}

string randString(int len){
	string r = newString(NULL,len);
	int i;
	for(i=0; i<len; i++) r.c[i]=(unsigned char)rand()%256;
	return r;
}

string stringLeftRotate(string word, int bits){
	string w=newString(NULL, word.len);
	int bitOffset = bits%8;
	int byteOffset = bits/8;
	unsigned char rmask = (unsigned char)((1<<(8-bitOffset))-1); //in binary, it's 1's for the rightmost 8-$bitOffset bits
	unsigned char lmask = ~rmask;
	int i;
	unsigned char l,r;

	for(i=0; i<w.len; i++){
		l=word.c[(i+byteOffset)%word.len];
		r=word.c[(i+byteOffset+1)%word.len];
		w.c[i] = ((l & rmask) << bitOffset) | ((r&lmask) >> (8-bitOffset));
	}
	return w;
}

string stringLeftShift(string word, int bits){
	string w;
	if(bits<0) return stringRightShift(word, -bits);
	w=newString(NULL, word.len+bits/8+1);
	int bitOffset = bits%8;
	unsigned char rmask = (unsigned char)((1<<(8-bitOffset))-1); //in binary, it's 1's for the rightmost 8-$bitOffset bits
	unsigned char lmask = ~rmask;
	int i;
	for(i=0; i<=word.len; i++){
		w.c[i]   |= (word.c[i] & lmask) >> (8-bitOffset);
		w.c[i+1] |= (word.c[i] & rmask) << (bitOffset);
	}
	if(w.c[0]==0){
		w = newString(&w.c[1],w.len-1);
	}
	return w;
}

string stringRightShift(string word, int bits){
	string w;
	if(bits<0) return stringLeftShift(word, -bits);
	w=newString(NULL, MAX(word.len,0));
	int bitOffset = bits%8;
	int byteOffset = bits/8;
	unsigned char rmask = (unsigned char)((1<<bitOffset)-1);
	unsigned char lmask = ~rmask;
	int i;
	for(i=0; i<w.len-byteOffset; i++){
		if(i>0)
			w.c[i+byteOffset] |= (word.c[i-1] & rmask) << (8-bitOffset);
		w.c[i+byteOffset] |= (word.c[i] & lmask) >> (bitOffset);
	}
	while(w.c[0]==0 && w.len>0){
		w = newString(&w.c[1],w.len-1);
	}
	return w;
}

string stringXOR(string a, string b){	//left-to-right XOR, repeating the shorter string

	string r;
	int i;
	r=newString(NULL, MAX(a.len,b.len));
	for(i=0; i<r.len; i++){
		r.c[i]=a.c[i%a.len]^b.c[i%b.len];
	}
	r.c[i]='\0';
	return r;
}

string bigIntAdd(string a, string b){   //right-to-left addition
	string R;
	if(a.sign == -1 && b.sign == 1)	return __bigIntSubtract(b,a);
	if(a.sign == 1 && b.sign == -1) return __bigIntSubtract(a,b);
	if(a.sign == -1 && b.sign == -1){
		R = __bigIntAdd(a,b);
		R.sign = -1;
		return R;
	}
	return __bigIntAdd(a,b);
}

string bigIntSubtract(string a, string b){ //right-to-left subtraction
	string R;
	if(a.sign == 1 && b.sign == -1)	return __bigIntAdd(a,b);
	if(a.sign == -1 && b.sign == -1) return __bigIntSubtract(b,a);
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
	 * Just adds one byte at a time and carries the one if it needs to be carried.
	 * The typecasting is a bit unfortunate but I don't think there's a way around it...
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
	string r=NULLSTRING;
	int i,j,diff;
	unsigned char c;

	string smaller;
	string bigger;
	if(__bigIntComp(a,b)==0) return charToS(0x00);

	smaller = LOCALSTRING(((bigIntComp(a,b)==-1) ? a : b));
	bigger  = LOCALSTRING(((bigIntComp(a,b)==-1) ? b : a));

	for(i=0; i<smaller.len; i++){
		diff = (int)((unsigned char)bigger.c[bigger.len - i - 1]) - (int)((unsigned char)smaller.c[smaller.len - i - 1]);
		c = (unsigned char)(diff + (diff < 0 ? 256 : 0));
		r = stringCat(charToS(c),r);
		j = bigger.len - i -1;
		while(diff < 0){
			if (--j==-1){//this shouldn't ever happen, since we're doing bigger - smaller
				fprintf(stderr, "Something went wrong with stringSubtract.");
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
	r.sign = bigIntComp(a,b);
	return r;
}

string bigIntMultiply(string a, string b){
	/*
	 * Multiples two bigint strings.
	 * It's just regular multiplication by hand, using the binary representation
	*/
	string r = charToS(0x00);
	long int l = a.len*8;
	long int i;
	for(i=0; i<l; i++){
		if(bigIntParity(stringRightShift(a,i))==1)
			r = bigIntAdd(r,stringLeftShift(b,i));
	}
	r.sign = a.sign * b.sign;
	return r;
}

int __bigIntComp(string a, string b){ // assumes a,b positive
	int i;
	string A = stripLeadingZeroes(a);
	string B = stripLeadingZeroes(b);

	if(A.len<B.len) return -1;
	if(A.len>B.len) return 1;

	for(i=0; i<a.len; i++){
		if((unsigned char)a.c[i] > (unsigned char)b.c[i]) return 1;
		if((unsigned char)a.c[i] < (unsigned char)b.c[i]) return -1;
	}
	return 0;
}

int    bigIntComp(string a, string b){ //compares a, b, as bigints, returning 1 if a>b, -1 if a<b, 0 if a==b.
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

string charToS(char c){
	return newBigInt(&c, 1, c>=0?1:-1);
}

int bigIntParity(string a){
	//Is the bigint string odd or even?
	return a.c[a.len-1] & 0x1;
}

string stripLeadingZeroes(string a){
	int i;
	if(a.len==0) return a;
	if(a.c[0]!=0) return a;
	while(a.c[i++]==0 && i<a.len);
	if(i==a.len) return NULLSTRING;
	return newBigInt(&a.c[i],a.len-i,a.sign);
}

string *__bigIntDivide(string a, string b){ // assumes both are positive
	string *qr = calloc(2,sizeof(string));
	if(bigIntComp(a,b)==1){
		qr[0] = charToS(0x00); qr[1] = LOCALSTRING(a);
		return qr;
	}
	if(bigIntComp(a,b)==0){
		qr[0] = charToS(0x01); qr[1] = charToS(0x00);
		return qr;
	}
	if(bigIntComp(b,charToS(0x02))==0){
		qr[0] = stringRightShift(a,1);
		qr[1] = charToS(bigIntParity(a)&0xFF);
		return qr;
	}
	qr = __bigIntDivide(bigIntSubtract(a,b),b);
	qr[0] = bigIntIncr(qr[0]);
	return qr;
}

string *bigIntDivide(string a, string b){
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

string bigIntIncr(string a){
	return bigIntAdd(a,charToS(0x01));
}

string uint32ToString(uint32_t l){
	string s = newString(NULL,4);
	int i;
	for(i=0; i<4; i++){
		s.c[i] = (l >> (8*(3-i))) & 0xFF;
	}
	return s;
}

uint32_t stringToUint32(string s){
	uint32_t l;
	int i;
	l=0;
	for(i=0; i<4; i++){
		l = l|(((uint32_t)((unsigned char)s.c[i]))<<(8*(3-i)));
	}
	return l;
}
