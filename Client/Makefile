# make for ASN03

CC=gcc
CFLAGS=-Wall -ggdb

c4981_chat_client: c4981_asn3_client.o client.o
	$(CC) $(CFLAGS) c4981_asn3_client.o client.o -o c4981_chat_client -lpthread

clean:
	rm -f *.o *.bak ex

c4981_asn3_client.o: c4981_asn3_client.c
	$(CC) $(CFLAGS) -O -c c4981_asn3_client.c

client.o: client.c
	$(CC) $(CFLAGS) -O -c client.c

