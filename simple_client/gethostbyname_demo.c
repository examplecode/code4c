#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


int main() 
{
	test_case1();
}

int test_caste2() 
{
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	inet_pton(AF_INET, "www.sina.com", &sa.sin_addr);

	char node[NI_MAXHOST];
	int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), node, sizeof(node), NULL, 0, 0);
	if (res)
	{
		printf("&#37;s\n", gai_strerror(res));
		exit(1);
	}
	printf("%s\n", node);
}

int test_case1()
{
	struct addrinfo* ailist;
	struct addrinfo* aip;
	struct addrinfo hint;
	struct sockaddr_in* sinp;
	const char* addr;
	char abuf[512];

	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = 0;
	hint.ai_socktype = 0;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	const char* host = "www.sina.com";
	const char* serv = "ftp";

	if (getaddrinfo(host, NULL, &hint, &ailist))
	{
		printf("error: getaddrinfo()\n");
		exit(1);
	}

	// char buf[512];
	// if (getnameinfo(aip->ai_addr, sizeof(struct sockaddr), buf, 512, NULL, 0, 0) == 0)
	// printf("%s\n", buf);

	for (aip = ailist; aip != NULL; aip = aip->ai_next)
	{

		printf ("=======================\n");
		char buf[512];

		if (aip->ai_family == AF_INET) 
		{
			if (getnameinfo(aip->ai_addr, sizeof(struct sockaddr), buf, 512, NULL, 0, 0) == 0)
				printf("%s\n", buf);
		}



	}
}
