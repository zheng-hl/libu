/* $Id: main.c,v 1.5 2009/12/26 14:16:35 tho Exp $ */

#include <stdlib.h>
#include <u/libu.h>

int facility = LOG_LOCAL0;

int main (int argc, char *argv[])
{
    u_net_addr_t *a = NULL;
    int sd = -1, asd = -1, at;
    struct sockaddr_storage sa;
    socklen_t sa_len = sizeof sa;
    char s[1024];
    ssize_t rb;

    con_err_ifm (argc != 2, "usage: %s <bind uri>", argv[0]);

    con_err_if (u_net_uri2addr(argv[1], &a));
    con_err_sif ((sd = u_net_sock_by_addr(a, U_NET_SSOCK)) == -1);

    /* only STREAM/SEQPACKET sockets need to call accept(2) */
    asd = ((at = u_net_addr_get_type(a)) != U_NET_UDP4 && at != U_NET_UDP6) ?
        u_accept(sd, (struct sockaddr *) &sa, &sa_len) : sd;
    con_err_sif (asd == -1);

    /* read data */
    con_err_sif ((rb = recvfrom(asd, s, sizeof s, 0, 
                    (struct sockaddr *) &sa, &sa_len)) == -1);
    con("read: %s", s);

    /* dtors */
    u_net_addr_free(a);
    (void) close(sd);
    (void) close(asd);
    
    return EXIT_SUCCESS;
err:
    if (a)
        u_net_addr_free(a);
    U_CLOSE(sd);
    U_CLOSE(asd);
    return EXIT_FAILURE;
}
