/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

void construire_message(char *message, char motif, int lg, int nb_message) 
{
	int i;
	int nb=nb_message;
	int fact = 10000;
	char nbs[10]="0123456789";
	for (i=0;i<lg;i++) 
	{
		if (i>4)
		{
			message[i] = motif;
		}
		else 
		{
			if ((nb/fact)==0 && nb!=0)
			{
				message[i]='-';

			}
			else 
			{
				message[i]=nbs[nb/fact];
				nb=nb%fact;
			}
			fact=fact/10;
		}
		
	}
}

void afficher_message(char *message, int lg) 
{
	int i;
	printf("[");
	for (i=0;i<lg;i++) 
	{
		printf("%c", message[i]); 
	}
	printf("]");
	printf("\n");
}

int create_local_socket(int port,int protocol){
	int sock;
	if (protocol==1)
	{
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			printf("échec de création du socket\n") ;
			exit(1);
		}
		struct sockaddr_in adr_local ; /* adresse du socket local */
		int lg_adr_local = sizeof(adr_local) ;
		memset((char*)&adr_local, 0, sizeof(adr_local)) ; /* reset */
		/*if (getsockname(sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
		{
			printf("échec du getsockname\n") ;
			exit(1) ; 
		}*/
		adr_local.sin_family = AF_INET ;
		adr_local.sin_port = port ;
		adr_local.sin_addr.s_addr = INADDR_ANY ;
		if (bind (sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
		{ 
			printf("échec du bind\n") ;
			exit(1) ; 
		}
		return sock;
	}
	else
	{

	}
}

void fc_source(int lg,int port,char* adr, int protocol, int nb_message){

	printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%d, dest=%s\n",lg,port,30,protocol,adr);
	int sock;
	if (protocol==1)
	{
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			printf("échec de création du socket\n") ;
			exit(1);
		}
		struct hostent *hp ;
		struct sockaddr_in adr_distant;
		/* affectation domaine et n° de port */
		/* reset de la structure */
		memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
		adr_distant.sin_family = AF_INET ; /* domaine Internet*/
		adr_distant.sin_port = port ; /* n° de port */
		/* affectation @_IP */
		if ((hp = gethostbyname(adr)) == NULL)
		{ 
			printf("erreur gethostbyname\n") ;
			exit(1) ; 
		}
		memcpy( (char*)&(adr_distant.sin_addr.s_addr),
		hp->h_addr,
		hp->h_length ) ;
		printf("Socket ok. Preparation de l'envoi des messages.\n");
		char* message= malloc(sizeof(char)*lg);
		char motif_tab[26]="abcdefghijklmnopqrstuvwxyz";
		for (int nb_mess=0;nb_mess<nb_message;nb_mess++)
		{
			char motif = motif_tab[(nb_mess)%26];
			construire_message(message,motif,lg,nb_mess+1);
			printf("Envoi n°%d (%d) ",nb_mess+1,lg);
			afficher_message(message,lg);
			if ((sendto(sock,message,lg,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant)))==-1)
			{	
				printf("Erreur ça va pas du tout là arrêteeeeeeeeeeeeeee.\n");
			}
		}
		if (close(sock)==-1)
		{
			printf("echec fermeture\n");
		}
	}

}
void fc_puits(int lg,int port, int protocol,int nb_message){
	printf("PUITS: lg_mesg_lu=%d, port=%d, nb_reception= infini, TP=%d\n",lg,port,protocol);
	int sock;
	char* message= malloc(sizeof(char)*30);
	sock = create_local_socket(port,protocol);
	struct sockaddr_in src;
	unsigned int size = sizeof( src );
	int lg_mess_recep;
	for (int i=0; i<nb_message;i++)
	{
		
		lg_mess_recep=recvfrom(sock,message,lg,0,(struct sockaddr*)&src, &size );
		if (lg_mess_recep==-1)
		{
			printf("echec de l'écoute\n");
			exit(1);
		}
		else
		{
			printf("Reception n°%d (%d) ",i+1,lg_mess_recep);
			afficher_message(message, lg_mess_recep);
		}
	}
	if (close(sock)==-1)
	{
		printf("echec fermeture\n");
	}
}

void main (int argc, char **argv)
{
	int c;
	int protocol=0;
	extern char *optarg;
	extern int optind;
	int lg = 30;
	int port = atoi(argv[argc-1]);
	port = htons(port);
	char *adr = (argv[argc-2]);
	int nb_message = 10; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	while ((c = getopt(argc, argv, "pn:su")) != -1) {
		switch (c){
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'u':
			protocol=1;
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

	if (source == 1) 
	{
		printf("on est dans le source\n");
	}
	else
	{
		printf("on est dans le puits\n");
		
	}
	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}
	if (source==1)
	{
		fc_source(lg,port,adr,protocol,nb_message);
	}
	else{
		fc_puits(lg,port,protocol,nb_message);
	}
}


