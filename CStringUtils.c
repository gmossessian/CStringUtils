/*
 * CStringUtils.c
 *
 *  Created on: Jan 6, 2016
 *      Author: George Mossessian
 *
 *
 */

#include "CStringUtils.h"

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

void printsprint(string s){ //print characters if they are printable, otherwise print a space.
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

string readInput(string end){ //read input from console, including newlines, until the delimeter is reached.
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

int HammingDistance(string str1, string str2){
	int i,d=0;
	char c;
	for(i=0; i<MIN(str1.len,str2.len); i++){
		c=(str1.c[i]^str2.c[i]);
		do{d+=c&0x01; c=c>>1; }while(c);
	}
	return d;
}

string *blockString(string str, int bs){ //break str into blocks of size bs.
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

int numBlocks(string str, int bs){ //compute number of blocks for blockString()
	return str.len/bs + (str.len%bs? 1: 0);
}

string *invertBlocks(string str, int bs){ //transpose of blockString()
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

int blockRepeats(string str, int bs){ //is there a repeating block? 1 if yes, 0 if no. 
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
	r.sign = 1;
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
	w.sign = word.sign;
	return w;
}

string stringRightShift(string word, int bits){
	string w;
	if(bits<0) return stringLeftShift(word, -bits);
	if((bits >> 3) > word.len){
		return charToS(0x00);
	}	
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
	w.sign = word.sign;
	if(bigIntComp(w,charToS(0x00))==0){
		w = charToS(0x00);
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

string charToS(char c){ //turns char into string with len=1.
	return newBigInt(&c, 1, 1);
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

string uint32ToString(uint32_t l){
	string s = newString(NULL,4);
	int i;
	for(i=0; i<4; i++){
		s.c[i] = (l >> (8*(3-i))) & 0xFF;
	}
	return s;
}

