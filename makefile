CC = clang

CFILESNOMAIN = CStringUtils.c CStringUtils_bigInt.c 
CFILES = $(CFILESNOMAIN) CStringUtils_test.c

HFILES = CStringUtils.h

build: $(CFILES) $(HFILES)
	$(CC) $(CFILES) $(HFILES)

compile: $(CFILESNOMAIN) $(HFILES)
	$(CC) -c $(CFILESNOMAIN) 
	rm libCStringUtils.a
	ar -cvq libCStringUtils.a $(patsubst %.c, %.o, $(CFILESNOMAIN))
