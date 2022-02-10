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

void main (int argc, char **argv)
{
	int c;
	int protocol = 0;
	extern char *optarg;
	extern int optind;
	int lg_mesg_emis =30;
	int port = atoi(argv[argc-1]);
	port = htons(port);
	char adr = (argv[argc-2]);
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	while ((c = getopt(argc, argv, "pn:su")) != -1) {
		switch (c) {
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
            protocol = 1;
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
		printf("on est dans le source\n");
	else
		printf("on est dans le puits\n");

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
	if (protocol=1) {
	    if (source == 1) {
	    	struct hostent *hp ;
			struct sockaddr_in adr_distant;
			/* main */
			...
			/* affectation domaine et n° de port */
			/* reset de la structure */
			memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
			adr_distant.sin_family = AF_INET ; /* domaine Internet*/
			adr_distant.sin_port = port ; /* n° de port */
			/* affectation @_IP */
			if ((hp = gethostbyname(adr)) = = NULL)
			{ 
				printf("erreur gethostbyname\n") ;
				exit(1) ; 
			}
			memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);


			/* association @ socket représentation interne */
		

			for (int nb_mess=0;nb_mess<nb_message;nb_mess++)
			{
				char * message;
				message = malloc (lg);
				char motif = (nb_mess+1)%26;
				construire_message(message,motif,lg,nb_mess);
			}

			if (sendto(sock,message,lg,0, (struct sockaddr *)&adr_local, lg_adr_local) ==-1)
			{
				printf("échec d'envoi du message\n") ;
				exit(1) ; 
			}


	    	if (close(sock) == -1)
			{ 
				printf("échec de destruction du socket\n") ;
				exit(1) ; 
			}
	    }
	    else if (source == 0) {

			int sock ; /* représentation interne du socket local */
			struct sockaddr_in adr_local ; /* adresse du socket local */
			int lg_adr_local = sizeof(adr_local) ;

			/* création du socket */
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			{ 
				printf("échec de création du socket\n") ;
				exit(1) ; 
			}
			/* construction de l’@ du socket avec n° de port = port et */
			/* @ IP = celle de la machine sur laquelle s'exécute le proc */
			memset((char*)&adr_local, 0, sizeof(adr_local)) ; /* reset */
			adr_local.sin_family = AF_INET ;
			adr_local.sin_port = port;
			adr_local.sin_addr.s_addr = adr ;

			if (bind (sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
			{ 
				printf("échec du bind\n") ;
				exit(1) ;
			}
			if recvfrom
	    }
	}
	void construire_message(char *message, char motif, int lg, int nb_mess) {
		
        for (int i=nb_mess-1;i<lg;i++) 
        {
            message[i] = motif;
        }
		n=nb_mess;
		for (int i=4;i>=0;i--)
		{
			if (n!=0)
			{
				message[i]=n%nb_digit(n);
				n=n/10^(nb_digit(n));
			}
			else 
			{
				message[i]=-;
			}
		} 
		
    }
    void afficher_message(char *message, int lg) {
        int i;
        printf("message construit : ");
        for (i=0;i<lg;i++) {

            printf("%c", message[i]); printf("\n");
        }
    }
	int nb_digit(int n)
	{
		int count=0;
		do {
			n /= 10;
			++count;
		}while (n!=0);
		return count-1;
	}

}

