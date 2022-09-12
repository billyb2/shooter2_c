#ifdef __WIN32__
#define socklen_t int
#define WORD unsigned short
#define BYTE unsigned char

#define NO_ERROR 0
#define AF_INET 2
#define SOCK_DGRAM 2
#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN 128

#define s_addr	S_un.S_addr
#define s_host	S_un.S_un_b.s_b2
#define s_net	S_un.S_un_b.s_b1
#define s_imp	S_un.S_un_w.s_w2
#define s_impno	S_un.S_un_b.s_b4
#define s_lh	S_un.S_un_b.s_b3
#define SOCKET_ERROR (-1)

#define SHUT_RDWR 2

typedef struct WSAData {
        WORD            wVersion;
        WORD            wHighVersion;
#ifdef _WIN64
        unsigned short  iMaxSockets;
        unsigned short  iMaxUdpDg;
        char            *lpVendorInfo;
        char            szDescription[WSADESCRIPTION_LEN+1];
        char            szSystemStatus[WSASYS_STATUS_LEN+1];
#else
        char            szDescription[WSADESCRIPTION_LEN+1];
        char            szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short  iMaxSockets;
        unsigned short  iMaxUdpDg;
        char            *lpVendorInfo;
#endif
} WSADATA, *LPWSADATA;

typedef struct in_addr {
  union {
    struct { unsigned char  s_b1, s_b2, s_b3, s_b4; } S_un_b;
    struct { unsigned short s_w1, s_w2; } S_un_w;
    unsigned long S_addr;
  } S_un;
} IN_ADDR, *PIN_ADDR, *LPIN_ADDR;

#define MAKEWORD(low,high) ((WORD)(((BYTE)(low)) | ((WORD)((BYTE)(high))) << 8))

#define IOCPARM_MASK 0x7
#define IOC_IN 0x80000000
#define _IOW(x,y,t) (IOC_IN|(((__LONG32)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define FIONBIO _IOW('f',126,unsigned long)
#define IPPROTO_TCP 6 
#define IPPROTO_UDP 17

struct sockaddr_in {
	short   sin_family;
	unsigned short sin_port;
	struct in_addr  sin_addr;
	char    sin_zero[8];
};

#define SOCKET unsigned int
#define INVALID_SOCKET (SOCKET)(~0)
#define WSAEWOULDBLOCK 10035
#define WSAEMSGSIZE 10040
#define MSG_PEEK 0x2

ssize_t recv(SOCKET sockfd, void* buf, size_t len, int flags);
size_t recvfrom(SOCKET sockfd, void *restrict buf, size_t len, int flags, struct sockaddr *restrict src_addr, socklen_t *restrict addrlen);

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

int bind(SOCKET s,const struct sockaddr *name,int namelen);
int closesocket(SOCKET s);
int connect(SOCKET s,const struct sockaddr *name,int namelen);
int ioctlsocket(SOCKET s,__LONG32 cmd,unsigned long *argp);
int getpeername(SOCKET s,struct sockaddr *name,int *namelen);
int getsockname(SOCKET s,struct sockaddr *name,int *namelen);
int getsockopt(SOCKET s,int level,int optname,char *optval,int *optlen);
unsigned long htonl(unsigned long hostlong);
unsigned short htons(unsigned short hostshort);
unsigned __LONG32 inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);
int listen(SOCKET s,int backlog);
int setsockopt(SOCKET s,int level,int optname,const char *optval,int optlen);
int shutdown(SOCKET s,int how);
SOCKET socket(int af,int type,int protocol);
struct hostent *gethostbyaddr(const char *addr,int len,int type);
struct hostent *gethostbyname(const char *name);
int gethostname(char *name,int namelen);
struct servent *getservbyport(int port,const char *proto);
struct servent *getservbyname(const char *name,const char *proto);
struct protoent *getprotobynumber(int number);
struct protoent *getprotobyname(const char *name);
int WSAStartup(WORD wVersionRequested,LPWSADATA lpWSAData);
int WSACleanup(void);
int WSAGetLastError();

#endif

