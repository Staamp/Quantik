#include "validation.h"
#include "fonctionTCP.h"
#include "protocolQuantik.h"
#include "conversion.h"
#include "printMessageEnum.h"

void printHelp(char* name) {
    printf("usage : %s IPserver port name color(W/B) portIA\n", name);
}


int main (int argc, char** argv) {

    if (argc != 6) {
        printHelp(argv[0]);
        return -1;
    }

    char* ipServeur = argv[1];
    char* playerName = argv[3];
    char* color = argv[4];
    int port = atoi(argv[2]);
    int sock = socketClient(ipServeur, port);
    //msg erreur TO DO
    int err;
    int portIA = atoi(argv[5]);
    TPartieReq gameRequest;
    TPartieRep gameResponse;

    if(!strcmp(color,"W")) {
        gameRequest.coulPion = BLANC;
    } else if(!strcmp(color,"B")) {
        gameRequest.coulPion = NOIR;
    } else {
        printHelp(argv[0]);
        return -2;
    }
    
    gameRequest.idReq = PARTIE;
    strcpy(gameRequest.nomJoueur, playerName);

		printf("-----------------------------------------------------------------------------\n");
		printf("Welcome to Quantik !\n");


    err = send(sock, &gameRequest, sizeof(TPartieReq),0);
    printError(err, "(player) send error with the initialize game request\n", sock);

    printf("Waiting for player...\n");
    err = recv(sock, &gameResponse, sizeof(TPartieRep),0);
    printError(err, "(player) recv error with the initialize game response\n", sock);


    switch(gameResponse.err) {
        case ERR_OK : 
            printf("You play against : %s\n", gameResponse.nomAdvers);
            break;
        case ERR_PARTIE :
            printf("Couldn't log into the game, invalid game request\n");
            break;
        case ERR_TYP :
            printf("Couldn't log into the game, invalid type request\n");
            break;
        default :
            printf("default\n");
    } 

    if (gameResponse.validCoulPion) {
        if (gameRequest.coulPion == BLANC) {
            gameRequest.coulPion = NOIR;
            printf("Color not available, you will play with black (%d).\n",gameRequest.coulPion);
        } else {
            gameRequest.coulPion = BLANC;
        		printf("Color not available, you will play with white (%d).\n",gameRequest.coulPion);
        }
    } else {
       if (gameRequest.coulPion == BLANC) {
            printf("You will play with white and you start(%d).\n",gameRequest.coulPion);
        } else {
            printf("You will play with black and white start(%d).\n",gameRequest.coulPion);
        }
    }


   /* printf("The game is starded :\n");
    int sockIA  = connectionIA(portIA); // Connexion a l'IA
      
    // Envoie au java
    int ent = 12;
    err = sendIA(ent, sockIA);
    if (err == -1) {
    	return -1;
    }
    
    // Reception du java
    int recu = recvIA(sockIA);
    if (recu == -1) {
    	return -1;
    }  */
    
    
    int wonMatch = 0;										// The number of match won for the player 
    int drawMatch = 0;									// The number of match drew for the player
    int lostMatch = 0;									// The number of match lost for the player
    int wonMatchOpponent = 0;						// The number of match won for the opponent player
    int drawMatchOpponent = 0;					// The number of match drew for the opponent player
    int lostMatchOpponent = 0; 					// The number of match lost for the opponent player
    
    int end = 0; 												//if the game is ended
    int i = 0; 													//iteration of the loop
    int x; 															//coordinate in line choose
    int y; 															//coordinate in column choose
    int p; 															//pawn choose 
    int c; 															//if the move is winner, looser...
		TCoupReq coupRepAdversaire;					//opponent request
		TCoupReq coupReq;										//us request
		TCoupRep coupResponse;							//us response
		TCoupRep coupResponseAdversaire;		//opponent response
		
		do {
		
			i++;
			
			if (gameRequest.coulPion == BLANC) {
				
				printf("-----------------------------------------------------------------------------\n");
				printf ("\n\nIteration %d\n", i);
				printf("\nChoisir x (0,1,2,3) :\n");
				scanf("%d", &x);
				printf("Choisir y (0,1,2,3) :\n");
				scanf("%d", &y);
				printf("Choisir p (0,1,2,3) :\n");
				scanf("%d", &p);
				printf("Propirete coup (0,1,2,3) : \n");
				scanf("%d", &c);
				printf("\n\n");
				
				TPion tpion = pionToTPion(p, gameRequest.coulPion);
				TCase tcase = positionToTCase(x, y);
				TPropCoup propCoup = intToProprieteCoup(c);
				bool estBloque = false;
				
				coupReq = requeteCoup(COUP, 1, estBloque, tpion, tcase, propCoup);
				
				//printf("SEND BLANC\n");
				err = 0;
				err = send(sock, &coupReq, sizeof(TCoupReq),0);
				printError(err, "(player) send error with the game request of one move\n", sock);
				
			 	//printf("RECV REPONSE VALIDE BLANC\n");
				err = 0;
				err = recv(sock, &coupResponse, sizeof(TCoupRep),0);
				printError(err, "(player) recv error with the game response of one move\n", sock);
				
				end = responseError(coupResponse.err);
				if (end != 0) {
					lostMatch++;
					wonMatchOpponent++;
					break;
				}
				end = responseValidCoup(coupResponse.validCoup, gameRequest.nomJoueur);
				if (end != 0) {
					lostMatch++;
					wonMatchOpponent++;
					break;
				}
				end = responseContinuerAJouer(coupResponse.propCoup, gameRequest.nomJoueur);
				if (end == 1) {
					wonMatch++;
					lostMatchOpponent++;
					break;
				}
				if (end == 2) {
					drawMatch++;
					drawMatchOpponent++;
					break;
				}
				if (end == 3) {
					lostMatch++;
					wonMatchOpponent++;
					break;
				}
				
				err = 0;
				printf("Waiting the player to play...\n");
				//printf("RECV COUV VALIDE ADVERSAIRE BLANC\n");
				err = recv(sock, &coupResponseAdversaire, sizeof(TCoupRep),0);
				printError(err, "(player) recv error with the opponent move response\n", sock);
				
				end = responseAdversaireError(coupResponseAdversaire.err);
				if (end != 0) {
					lostMatchOpponent++;
					wonMatch++;
					break;
				}
				end = responseAdversaireValidCoup(coupResponseAdversaire.validCoup, gameResponse.nomAdvers);
				if (end != 0) {
					lostMatchOpponent++;
					wonMatch++;
					break;
				}
				end = responseAdversairePropCoup(coupResponseAdversaire.propCoup, gameResponse.nomAdvers);
				if (end == 1) {
					wonMatchOpponent++;
					lostMatch++;
					break;
				}
				if (end == 2) {
					drawMatchOpponent++;
					drawMatch++;
					break;
				}
				if (end == 3) {
					lostMatchOpponent++;
					wonMatch++;
					break;
				}
				
				//printf("RECV COUP ADV BLANC\n");
				err = recv(sock, &coupRepAdversaire, sizeof(TCoupReq),0);
				printError(err, "(player) recv error with the opponent move\n", sock);

				int xa = ligneToInt(coupRepAdversaire.posPion.l);
				int ya = colonneToInt(coupRepAdversaire.posPion.c);
				int pa = pionToInt(coupRepAdversaire.pion.typePion);
				int ca = coupRepAdversaire.propCoup;
				
				printf("[%d,%d,%d,%d]\n", xa, ya, pa, ca);
			
			}
			
			
			
			
			if (gameRequest.coulPion == NOIR) {
				
				printf("-----------------------------------------------------------------------------\n");
				printf("\n\nIteration %d\n", i);
				
				//printf("RECV COUV VALIDE ADVERSAIRE NOIR\n");
				err = 0;
				err = recv(sock, &coupResponseAdversaire, sizeof(TCoupRep),0);
				printError(err, "(player) recv error with the opponent move response\n", sock);
				
				end = responseAdversaireError(coupResponseAdversaire.err);
				if (end != 0) {
					lostMatchOpponent++;
					wonMatch++;
					break;
				}
				end = responseAdversaireValidCoup(coupResponseAdversaire.validCoup, gameResponse.nomAdvers);
				if (end != 0) {
					lostMatchOpponent++;
					wonMatch++;
					break;
				}
				end = responseAdversairePropCoup(coupResponseAdversaire.propCoup, gameResponse.nomAdvers);
				if (end == 1) {
					wonMatchOpponent++;
					lostMatch++;
					break;
				}
				if (end == 2) {
					drawMatchOpponent++;
					drawMatch++;
					break;
				}
				if (end == 3) {
					lostMatchOpponent++;
					wonMatch++;
					break;
				}
				
				//printf("RECV COUP ADV NOIR \n");
				err = 0;
				err = recv(sock, &coupRepAdversaire, sizeof(TCoupReq),0);
				printError(err, "(player) recv error with the opponent move\n", sock);
				
				int xa = ligneToInt(coupRepAdversaire.posPion.l);
				int ya = colonneToInt(coupRepAdversaire.posPion.c);
				int pa = pionToInt(coupRepAdversaire.pion.typePion);
				int ca = coupRepAdversaire.propCoup;
				
				printf("[%d,%d,%d,%d]\n", xa, ya, pa, ca);
				
				printf("Choisir x (0,1,2,3) :\n");
				scanf("%d", &x);
				printf("Choisir y (0,1,2,3) :\n");
				scanf("%d", &y);
				printf("Choisir p (0,1,2,3) :\n");
				scanf("%d", &p);
				printf("Propirete coup (0,1,2,3) : \n");
				scanf("%d", &c);
				
				
				TPion tpion = pionToTPion(p, gameRequest.coulPion);
				TCase tcase = positionToTCase(x, y);
				TPropCoup propCoup = intToProprieteCoup(c);
				bool estBloque = false;
				coupReq = requeteCoup(COUP, 1, estBloque, tpion, tcase, propCoup);
				
				//printf("SEND NOIR\n");
				err = 0;
				err = send(sock, &coupReq, sizeof(TCoupReq),0);
				printError(err, "(player) send error with the game request of one move\n", sock);
				
			 	printf("Waiting the player to play...\n");
				//printf("RECV REPONSE COUP NOIR\n");
				err = 0;
				err = recv(sock, &coupResponse, sizeof(TCoupRep),0);
				printError(err, "(player) recv error with the game response of one move\n", sock);
				
				end = responseError(coupResponse.err);
				if (end != 0) {
					lostMatch++;
					wonMatchOpponent++;
					break;
				}
				end = responseValidCoup(coupResponse.validCoup, gameRequest.nomJoueur);
				if (end != 0) {
					lostMatch++;
					wonMatchOpponent++;
					break;
				}
				end = responseContinuerAJouer(coupResponse.propCoup, gameRequest.nomJoueur);
				if (end == 1) {
					wonMatch++;
					lostMatchOpponent++;
					break;
				}
				if (end == 2) {
					drawMatch++;
					drawMatchOpponent++;
					break;
				}
				if (end == 3) {
					lostMatch++;
					wonMatchOpponent++;
					break;
				}
						
			}
		
		} while (end != 1);
		
		
		printf("\n-----------------------------------------------------------------------------\n");
		printf("The first game is ended\n");
		printf("-Player %s : won match : %d / drew match : %d / lost match : %d\n",
			 gameRequest.nomJoueur, wonMatch, drawMatch, lostMatch);
		printf("-Player %s : won match : %d / drew match : %d / lost match : %d\n",
			 gameResponse.nomAdvers, wonMatchOpponent, drawMatchOpponent, lostMatchOpponent);
			 
		printf("Now we start revenge, good luck !\n");
		

    shutdown(sock, SHUT_RDWR); close(sock);
    return 0;
}







































