#include "fonctionTCP.h"

#define TAIL_BUF 20

void printError(int err, char* msg, int sock, int sockIA) {
	if (err <= 0) {
    perror(msg);
    shutdown(sock, SHUT_RDWR); close(sock);
    shutdown(sockIA, SHUT_RDWR); close(sockIA);
    exit(EXIT_FAILURE);
	}
}

int socketServeur(ushort nPort) {
	int sockConx;        /* descripteur socket connexion */
	int port;            /* numero de port */
  int sizeAddr;        /* taille de l'adresse d'une socket */
  int err;		         /* code d'erreur */
  
  char buffer[TAIL_BUF]; /* buffer de reception */
  
  struct sockaddr_in addServ;	/* adresse socket connex serveur */
  struct sockaddr_in addClient;	/* adresse de la socket client connectee */
  
  if (nPort < 1024) {
    printf ("usage : %d port > 1024\n", port);
    return -1;
  }
  
  port = nPort;
  
  sockConx = socket(AF_INET, SOCK_STREAM, 0);
  if (sockConx < 0) {
    perror("(serveurTCP) erreur on socket");
    return -2;
  }
  
  int iOption = 1;
  setsockopt(sockConx, SOL_SOCKET, SO_REUSEADDR, (char*)&iOption, sizeof(iOption));
  
  addServ.sin_family = AF_INET;
  addServ.sin_port = htons(port); // conversion en format réseau (big endian)
  addServ.sin_addr.s_addr = INADDR_ANY; 
  // INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
  bzero(addServ.sin_zero, 8);
  
  sizeAddr = sizeof(struct sockaddr_in);

  err = bind(sockConx, (struct sockaddr *)&addServ, sizeAddr);
  if (err < 0) {
    perror("(serveurTCP) erreur on bind");
    close(sockConx);
    return -3;
  }
  
  int enable = 1;
  if(setsockopt(sockConx, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
  	printf("ERROR : SETSOCKOPT");
  	return -5;
  }  
  
  err = listen(sockConx, 1);
  if (err < 0) {
    perror("(serveurTCP) erreur dans listen");
    close(sockConx);
    return -4;
  }
  
  return sockConx;
}


int socketClient(char* nomMachine, ushort nPort) {
  char chaine[TAIL_BUF];   				/* buffer */
  int sock;                				/* descripteur de la socket locale */
  int port;                				/* variables de lecture */
  int err;                 				/* code d'erreur */
  char* ipMachServ;        				/* pour solution inet_aton */
  char* nomMachServ;       				/* pour solution getaddrinfo */
  struct sockaddr_in addSockServ; /* adresse de la socket connexion du serveur */
  struct addrinfo hints;   				/* parametre pour getaddrinfo */
  struct addrinfo *result; 				/* les adresses obtenues par getaddrinfo */ 
  socklen_t sizeAdd;       				/* taille d'une structure pour l'adresse de socket */
  
  ipMachServ = nomMachine; 
  nomMachServ = nomMachine;
  port = nPort;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("(client) error during the creation of the socket\n");
    return -2;
  }
  
  addSockServ.sin_family = AF_INET;
  err = inet_aton(ipMachServ, &addSockServ.sin_addr);
  if (err == 0) { 
    perror("(client) error while obtaining the server IP\n");
    close(sock);
    return -3;
  }
  
  addSockServ.sin_port = htons(port);
  bzero(addSockServ.sin_zero, 8);
 	
  sizeAdd = sizeof(struct sockaddr_in);

  /* 
   *  initialisation de l'adresse de la socket - version getaddrinfo
   */
  /*
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; // AF_INET / AF_INET6 
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  
  
  // récupération de la liste des adresses corespondante au serveur
  
  err = getaddrinfo(nomMachServ, argv[2], &hints, &result);
  if (err != 0) {
    perror("(client) erreur sur getaddrinfo");
    close(sock);
    return -3;
  }
  
  addSockServ = *(struct sockaddr_in*) result->ai_addr;
  sizeAdd = result->ai_addrlen;
  */
	
  err = connect(sock, (struct sockaddr *)&addSockServ, sizeAdd); 
  if (err < 0) {
    perror("(client) socket connection error\n");
    close(sock);
    return -4;
  }
  
  int enable = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
  	printf("(client) eror while setsockopt\n");
  	return -5;
  }
	
  return sock;
}

int connectionIA(int port) {
  printf("CONNECTION IA");
  struct sockaddr_in addClient;
  int sockTrans;
  int	sizeAddr;
	int	err;

  int sockServ = socketServeur(port);
  if (sockServ < 0){
		printf("(connectionIA) Error on socket server portIA\n");
		return -2;
	}

  sizeAddr = sizeof(struct sockaddr_in);
	sockTrans = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTrans < 0) {
		perror("(connectionIA) Error on accept");
		return -3;
	}

  return sockTrans;
}

int sendIA(int ent, int sockIA) {
	int err = 0;
	ent = htonl(ent);
	err = send(sockIA, &ent, sizeof(int),0);
	if (err <= 0) {
		perror("(player) send error with the IA request\n");
		shutdown(sockIA, SHUT_RDWR); close(sockIA);
		return -1;
	}
  ent = ntohl(ent);
  return 0;
}

int recvIA(int sockIA) {
	 	int entres;
    int err = 0;
    while (err < 4) {
        err = recv(sockIA, &entres, sizeof(int),MSG_PEEK);
    }
    err = recv(sockIA, &entres, sizeof(int),0);
    if (err <= 0) {
        perror("(player) recv error with the IA response\n");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -1;
    }
    int res = ntohl(entres);
    return res;
}






























