#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdint.h>
#include <assert.h>

/* need to be serilaize if on deiffernt arch or endiness */
struct response {
	int rc; 	/* retrun code */
	char res[1024];   /* res_ pointer */
};

#endif /* _SERVER_H_ */
