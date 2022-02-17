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
	int protocol=0;
	extern char *optarg;
	extern int optind;
	int lg = 30;
	int port = atoi(argv[argc-1]);
	port = htons(port);
	char *adr = (argv[argc-2]);
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
	void construire_message(char *message, char motif, int lg, int nb_message) 
	{
		int i;
		int nb=nb_message;
		int fact = 10000;
		for (i=0;i<lg;i++) 
		{
			if (i>4)
			{
				message[i] = motif;
			}
			else 
			{
				if (nb/fact==0)
				{
					message[i]='-';
				}
				else
				{
					message[i]=nb/fact;
					nb=nb%fact;
				}
				fact=fact/10;
			}
		}
	}
	void afficher_message(char *message, int lg) 
	{
		int i;
		printf("message construit : ");
		for (i=0;i<lg;i++) 
		{
			printf("%c", message[i]); printf("\n");
		}
	}

}

