
#ifndef REQUEST_H
#define REQUEST_H
#include <sys/types.h>
#include "tcp.h"

/*
A request object represents a HTTP request that has been
received but not yet serviced.  It contains the filename
of the requested file, and a pointer to a TCP object that
it should be sent to.  You may add items to this structure
if necessary.
*/

struct request {
	char *filename;
	struct tcp *conn;
	struct request *link;
	off_t fsize;
};

typedef struct request request;
/*
Create a new request object by reading from the given
TCP connection, and determining what file is needed.
*/

struct request * request_create( struct tcp *conn );

/*
Handle the request by opening the local file and sending
it to the necessary TCP connection.
*/

void request_handle( struct request *r );

/*
Free the request object and close the connection.
*/

void request_delete( struct request *r );

#endif
