#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT            8000 
#define MESSAGE         "Yow!!! Are we having fun yet?!?"
#define SERVERHOST      "127.0.0.1"

void
write_to_server (int filedes)
{
  int nbytes;

  nbytes = write (filedes, MESSAGE, strlen (MESSAGE) + 1);
  if (nbytes < 0)
    {
      perror ("write");
      exit (EXIT_FAILURE);
    }
}

void
init_sockaddr (struct sockaddr_in *name,
               const char *hostname,
               uint16_t port)
{
  struct hostent *hostinfo;

  name->sin_family = AF_INET;
  name->sin_port = htons (port);
  hostinfo = gethostbyname (hostname);
  if (hostinfo == NULL)
    {
      fprintf (stderr, "Unknown host %s.\n", hostname);
      exit (EXIT_FAILURE);
    }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

int make_client_sock(const char * server_name,int port)
{
  int sock;
  struct sockaddr_in servername;

  /* Create the socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror ("socket (client)");
    exit (EXIT_FAILURE);
  }

  /* Connect to the server. */
  init_sockaddr (&servername, SERVERHOST, PORT);


  if (0 > connect (sock,
        (struct sockaddr *) &servername,
        sizeof (servername)))
  {
    return -1;
  }
  return sock;
}

int
main (void)
{
  int sock = make_client_sock("localhost",8000);
  if(sock < 0)
  {
    perror("create new client error ");
  }


  //  int sock;
  // struct sockaddr_in servername;

  // /* Create the socket. */
  // sock = socket (PF_INET, SOCK_STREAM, 0);
  // if (sock < 0)
  //   {
  //     perror ("socket (client)");
  //     exit (EXIT_FAILURE);
  //   }

  // /* Connect to the server. */
  // init_sockaddr (&servername, SERVERHOST, PORT);
  // if (0 > connect (sock,
  //                  (struct sockaddr *) &servername,
  //                  sizeof (servername)))
  //   {
  //     perror ("connect (client)");
  //     exit (EXIT_FAILURE);
  //   }
  write_to_server(sock);

}
