#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#ifdef _USE_INET6
#define FAMILY PF_INET6
#else
#define FAMILY PF_INET
#endif

usage(char *str){
 printf ("Usage: %s <hostname> <IPport> [<UDP>]\n",str);
 exit(-1);
}

int main(int argc,char **argv){
#ifdef _USE_INET6
     struct sockaddr_in6 sa;
#else
     struct sockaddr_in sa;
#endif
     char node[NI_MAXHOST];
     char serv[NI_MAXSERV];
     int status,flags =0;
     size_t nodelen;
     struct addrinfo hints,*result;

     if (argc < 3) usage(argv[0]);
     if ((argc == 4) && (!strcmp(argv[3],"UDP")))
         flags=NI_DGRAM;
     /*
      * fill in the sa structure (either sockaddr_in or sockaddr_in6)
      * using getaddrinfo. When done free the resultant structure using
      * freeaddrinfo.
      */
     memset(&sa,0,sizeof(sa));
     memset(&hints,0,sizeof(hints));
     hints.ai_flags=AI_DEFAULT;
     hints.ai_family=FAMILY;
     status=getaddrinfo(argv[1],NULL,&hints,&result);
     if(status){
          printf ("no such host %s.\ngetaddrinfo error: %s\n",argv[1],
                  gai_strerror(status));
          exit(-1);
     }
     memcpy(&sa,result->ai_addr,sizeof(sa));
#ifdef _USE_INET6
     sa.sin6_port=htons(atoi(argv[2]));
#else
     sa.sin_port=htons(atoi(argv[2]));
#endif
     freeaddrinfo(result);
     /*
      * Call getnameinfo with the sa structure as input with node and
      * serv as output parameters. If the flag is set to NI_DGRAM AND
      * the service can use either TCP or UDP, then this will return
      * the UDP service name in the serv variable. Otherwise it will
      * just return the port number.
      */
     status = getnameinfo((struct sockaddr *)&sa,sizeof(sa),
                           node,NI_MAXHOST,serv,NI_MAXSERV,flags);
     if(status){
         printf("getnameinfo error=%1d\n",status);
         exit(-1);
     }
     printf("node = %s, service = %s\n",node,serv);
     return 0;
}