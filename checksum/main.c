#include <stdio.h> //for printf
#include <string.h> //memset
#include <sys/socket.h>    //for socket ofcourse
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h> //for exit(0);
#include <errno.h> //For errno - the error number
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/ip.h>    //Provides declarations for ip header

#define PAXOS_LEN 102 
 
struct proposer_state {
    int rawsock;
    char datagram[1500];
    struct sockaddr_in *dest;
};

/*
    Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}
 

int create_rawsock() {
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(s == -1)
    {
        perror("Failed to create raw socket");
        exit(1);
    }
    return s;
}

void init_rawsock (struct proposer_state *ctx, struct sockaddr_in *mine, struct sockaddr_in *dest)
{     
    //zero out the packet buffer
    memset (ctx->datagram, 0, PAXOS_LEN);
    //IP header
    struct iphdr *iph = (struct iphdr *) ctx->datagram;
    //UDP header
    struct udphdr *udph = (struct udphdr *) (ctx->datagram + sizeof (struct ip));
    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = mine->sin_addr.s_addr;    //Spoof the source ip address
    iph->daddr = dest->sin_addr.s_addr;
    //UDP header
    udph->source = mine->sin_port;
    udph->dest = dest->sin_port;
    udph->check = 0; //leave checksum 0 now, filled later by pseudo header
}


int paxos_send (struct proposer_state *ctx, char *msg, int msglen) {
    char *data;
    //Data part
    data = ctx->datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    memcpy(data , msg, msglen);
    //Ip checksum
    struct iphdr *iph = (struct iphdr *) ctx->datagram;
    iph->check = csum ((unsigned short *) ctx->datagram, iph->tot_len);
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + msglen;
    //UDP header
    struct udphdr *udph = (struct udphdr *) (ctx->datagram + sizeof (struct ip));
    udph->len = htons(8 + msglen); //udp header size
    udph->check = 0; //leave checksum 0 now, filled later by pseudo header

    if (sendto (ctx->rawsock, ctx->datagram, iph->tot_len,  0, (struct sockaddr *) ctx->dest, sizeof (*ctx->dest)) < 0) {
        perror("sendto failed");
    }
    //Data send successfully
    else {
        printf ("Packet Send. Length : %d \n" , iph->tot_len);
    }
    return 0;
}

void free_proposer_state(struct proposer_state *ctx) {
    free(ctx->dest);
    free(ctx);
}

int main(int argc, char* argv[]) {
    char msg[] = "ABC123456789";
    char ip_src[] = "192.168.4.97";
    char ip_dst[] = "224.3.29.73";
    int tp_src = 12345;
    int tp_dst = 34952;

    struct sockaddr_in mine;
    mine.sin_family = AF_INET;
    mine.sin_port = htons(tp_src);
    mine.sin_addr.s_addr = inet_addr (ip_src);  

    struct sockaddr_in *dest = malloc(sizeof(struct sockaddr_in));
    dest->sin_family = AF_INET;
    dest->sin_port = htons(tp_dst);
    dest->sin_addr.s_addr = inet_addr (ip_dst);  

    struct proposer_state *ctx = malloc(sizeof(struct proposer_state));
    ctx->rawsock = create_rawsock();
    ctx->dest = dest;

    init_rawsock(ctx, &mine, dest);
    int msglen = strlen(msg);
    paxos_send(ctx, msg, msglen);

    free_proposer_state(ctx);

    return 0;
}
//Complete 