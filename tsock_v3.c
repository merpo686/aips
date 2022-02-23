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


/////////////////////////////// VERSION FINALE AVEC UDP ET TCP/////////////

////////////////////CONSTRUCTION DU MESSAGE AU FORMAT REQUIS ////////////
void construire_message(char *message, char motif, int lg, int nb_message) 
{
	for (int i=0;i<lg;i++) 
	{
		message[i] = motif;
	}
}

void afficher_message(char *message, int lg,int nb_mess) 
{
	int i;
    int fact = 10000;
	char nbs[10]="0123456789";
	printf("[");
    int nb=nb_mess;
	for (i=0;i<lg+5;i++) 
	{
        if (i>4)
		{
			printf("%c", message[i]); 
		}
		else 
		{
			if ((nb/fact)==0 && nb!=0)
			{
				printf("-");
			}
			else 
			{
                printf("%c",nbs[nb/fact]);
				nb=nb%fact;
			}
			fact=fact/10;
		}
	}
	printf("]");
	printf("\n");
}

////////////////////////////////////CREATION DE SOCKET LOCAL//////////////
//////////////////////////RENVOIE LE SOCKET/////////////

int create_local_socket(int port,int protocol){
    int sock;
    ///////////////SI UDP ///////////////////
    if (protocol==1)
    {
        if ((sock=socket(AF_INET, SOCK_DGRAM, 0))== -1)
        {
            printf("échec de création du socket\n") ;
            exit(1);
        }
    }
    else ////////TCP ///////////
    {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("échec de création du socket\n") ;
            exit(1);
        }
    }
    //////////////////ADDRESSAGE DU SOCKET
    struct sockaddr_in adr_local ; /* adresse du socket local */
    int lg_adr_local = sizeof(adr_local) ;
    memset((char*)&adr_local, 0, sizeof(adr_local)) ; 
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

void fc_source(int lg,int port,char* adr, int protocol, int nb_message)
{
	printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%d, dest=%s\n",
    lg,port,nb_message,protocol,adr);
    //////////PREPARATION DES ADRESSES
	int sock;
    char* message= malloc(sizeof(char)*lg);
    char motif_tab[26]="abcdefghijklmnopqrstuvwxyz";
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
    memcpy( (char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length );
    printf("Socket ok. Preparation de l'envoi des messages.\n");
	if (protocol==1)
	{
        ///////CREATION DU SOCKET ET ENVOI DES MESSAGES
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            printf("échec de création du socket\n") ;
            exit(1);
        }
		for (int nb_mess=0;nb_mess<nb_message;nb_mess++)
		{
			char motif = motif_tab[(nb_mess)%26];
			construire_message(message,motif,lg,nb_mess+1);
			printf("SOURCE : Envoi n°%d (%d) ",nb_mess+1,lg);
			afficher_message(message,lg,nb_mess+1);
			if ((sendto(sock,message,lg,0,(struct sockaddr*)&adr_distant,
            sizeof(adr_distant)))==-1)
			{	
				printf("Echec envoi message \n");
			}
		}
        ////FERMETURE
		if (close(sock)==-1)
		{
			printf("Echec fermeture\n");
		}
	}
    else 
    {
        ///////CREATION DU SOCKET ET ENVOI DES MESSAGES
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("échec de création du socket\n") ;
            exit(1);
        }
        int connection = connect(sock,(struct sockaddr *)&adr_distant, 
        sizeof(adr_distant));
        if (connection==-1)
	    {
	        printf("Echec de connexion \n");
	        exit(1);
	    }
        //Phase d'envoi du message 
        for (int nb_mess=0;nb_mess<nb_message;nb_mess++) 
        {  
	        char motif = motif_tab[(nb_mess)%26];
			construire_message(message,motif,lg,nb_mess+1);
			printf("SOURCE : Envoi n°%d (%d) ",nb_mess+1,lg);
			afficher_message(message,lg,nb_mess+1);
	        if(write(sock, message, lg)==-1)
	        {
	            printf("Erreur lors de l'envoi du message \n");
	        }
        }
        ////FERMETURE
        if(shutdown(sock,1)==-1)
        {
	        printf("Echec de fermeture. \n");
        }
    }
}
void fc_puits(int lg,int port, int protocol,int nb_message){
    printf("PUITS : lg_mesg_lu=%d, port=%d, nb_reception= infini, TP=%d\n",
    lg,port,protocol);
    //////ADRESSAGE
    int sock;
	char* message= malloc(sizeof(char)*30);
	sock = create_local_socket(port,protocol);
	struct sockaddr_in src;
	unsigned int size = sizeof( src );
	int lg_mess_recep;
    /////UDP
    if (protocol==1)
    {
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
                printf("PUITS : Reception n°%d (%d) ",i+1,lg_mess_recep);
                afficher_message(message, lg_mess_recep,i+1);
            }
        }
        if (close(sock)==-1)
        {
            printf("echec fermeture\n");
        }
    }
    //////TCP 
    else 
    {
        int sock_src;
        listen(sock,5);
        if ((sock_src=accept(sock, (struct sockaddr *)&src, &size))==-1)
        { 
            printf("Echec du accept \n");
            exit(1);
        }
        for (int i=0; i<nb_message; i++)
        {
            if((lg_mess_recep=read(sock_src, message, lg))<0)
            {
                printf("Echec du read \n");
                exit(1);
            }
            printf("PUITS : Reception n°%d (%d) ",i+1,lg_mess_recep);
            afficher_message(message, lg_mess_recep,i+1);
        }
        if(shutdown(sock,0)==-1)
        {
            printf("Echec de fermeture. \n");
        }
    }
}

void main (int argc, char **argv)
{
	int c;
	int protocol=0;
	extern char *optarg;
	extern int optind;
	int lg = -1;
	int port = atoi(argv[argc-1]);
	char *adr = (argv[argc-2]);
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	while ((c = getopt(argc, argv, "pn:sul:")) != -1) 
    {
		switch (c)
        {
		case 'p':
			if (source == 1) 
            {
				printf("usage: cmd [-p|-s][-n ##][-l ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) 
            {
				printf("usage: cmd [-p|-s][-n ##][-l ##]\n");
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
        
        case 'l':
            lg = atoi(optarg);
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}
	if (source == -1) 
    {
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
	if (nb_message != -1) 
    {
		if (source == 1)
        {
			printf("nb de tampons à envoyer : %d\n", nb_message);
        }
		else
        {
			printf("nb de tampons à recevoir : %d\n", nb_message);
        }
	} 
    else 
    {
		if (source == 1) 
        {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} 
        else
        {
            nb_message = 10 ;
		    printf("nb de tampons à recevoir = infini\n");
        }
	}
    if (lg != -1) 
    {
		if (source == 1)
        {
			printf("nb de caractères à envoyer : %d\n", lg);
        }
		else
        {
			printf("nb de caractères à recevoir : %d\n", lg);
        }
	} 
    else 
    {
		if (source == 1) 
        {
			lg = 30 ;
			printf("nb de caractères à envoyer = 30 par défaut\n");
		} 
        else
        {
            lg = 30 ;
		    printf("nb de caractères à recevoir = 30\n");
        }

	}
	if (source==1)
	{
		fc_source(lg,port,adr,protocol,nb_message);
	}
	else
    {
		fc_puits(lg,port,protocol,nb_message);
	}
}


