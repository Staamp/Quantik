package org.example;

import org.jpl7.Query;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;

import java.util.Scanner;

/**
 * The class EngineIA make a connection with the player (file player.c)
 * This class receive and send the move realize by the player
 */
public class EngineIA {


    public static String intToStringPawn(int p) {
        switch(p) {
            case 0 : return "c";
            case 1 : return "p";
            case 2 : return "s";
            case 3 : return "t";
            default: return "";
        }
    }

    public static int stringToIntPawn(String p) {
        switch(p) {
            case "c" : return 0;
            case "p" : return 1;
            case "s" : return 2;
            case "t" : return 3;
            default: return -1;
        }
    }

    /**
     * This function realize a communication with player when he plays in second
     *
     * @param g the Quantik grid
     * @param i the number iteration
     * @param DIS the DataInputStream to receive move of the opponent player
     * @param DOS the DataOutputStream to send move of the opponent player
     * @return an error message if he are an error or 0 if all is normal
     * @throws Exception the exception to catch error with recv and send function
     */
    public static int playSecond(Grille g, int i, DataInputStream DIS, DataOutputStream DOS) throws Exception {
        Scanner sc = new Scanner(System.in);
        int xSend;
        int ySend;
        int pSend;
        int cSend;
        int xRecv;
        int yRecv;
        int pRecv;
        int cRecv;
        int continuerPartie = 0;

        System.out.println("-------------------------------------------------------------");
        System.out.println("\n\nPlaySecond Iteration " + i);

        g.printGrille();

        continuerPartie = DIS.readInt();
        if (continuerPartie != 0) {
            return -1;
        }

        xRecv = DIS.readInt();
        yRecv = DIS.readInt();
        pRecv = DIS.readInt();
        cRecv = DIS.readInt();
        if (cRecv != 0) {
            return -1;
        }

        g.addPawnInGrille(xRecv, yRecv, intToStringPawn(pRecv));

        System.out.println("The opponent played :");
        g.printGrille();

        System.out.println("Choisir x : (0,1,2,3)");
        xSend = Integer.parseInt(sc.next());
        System.out.println("Choisir y : (0,1,2,3)");
        ySend = Integer.parseInt(sc.next());
        System.out.println("Choisir p : (0,1,2,3)");
        pSend = Integer.parseInt(sc.next());
        System.out.println("Choisir c : (0,1,2,3)");
        cSend = Integer.parseInt(sc.next());

        DOS.writeInt(xSend); //Envoie de xSend
        DOS.writeInt(ySend); //Envoie de ySend
        DOS.writeInt(pSend); //Envoie de pSend
        DOS.writeInt(cSend); //Envoie de cSend

        g.addPawnInGrille(xSend, ySend, intToStringPawn(pSend));

        System.out.println("The grid after your move :");
        g.printGrille();

        return 0;
    }

    /**
     * This function realize a communication with player when he plays in second
     *
     * @param g the Quantik grid
     * @param i the number iteration
     * @param DIS the DataInputStream to receive move of the opponent player
     * @param DOS the DataOutputStream to send move of the opponent player
     * @return an error message if he are an error or 0 if all is normal
     * @throws Exception the exception to catch error with recv and send function
     */
    public static int playFirst(Grille g, int i, DataInputStream DIS, DataOutputStream DOS) throws Exception {

        Scanner sc = new Scanner(System.in);
        int xSend;
        int ySend;
        int pSend;
        int cSend;
        int xRecv;
        int yRecv;
        int pRecv;
        int cRecv;
        int continuerPartie = 0;

        System.out.println("-------------------------------------------------------------");
        System.out.println("\n\nPlayFirst Iteration " + i);

        g.printGrille();

        System.out.println("Choisir xSend : (0,1,2,3)");
        xSend = Integer.parseInt(sc.next());
        System.out.println("Choisir y : (0,1,2,3)");
        ySend = Integer.parseInt(sc.next());
        System.out.println("Choisir p : (0,1,2,3)");
        pSend = Integer.parseInt(sc.next());
        System.out.println("Choisir c : (0,1,2,3)");
        cSend = Integer.parseInt(sc.next());

        DOS.writeInt(xSend); //Envoie de xSend
        DOS.writeInt(ySend); //Envoie de y
        DOS.writeInt(pSend); //Envoie de p
        DOS.writeInt(cSend); //Envoie de c

        g.addPawnInGrille(xSend, ySend, intToStringPawn(pSend));

        System.out.println("The grid after your move :");
        g.printGrille();

        System.out.println("CONTINUER OU PAS ?");
        continuerPartie = DIS.readInt();
        if (continuerPartie != 0) {
            return -1;
        }

        xRecv = DIS.readInt();
        yRecv = DIS.readInt();
        pRecv = DIS.readInt();
        cRecv = DIS.readInt();
        if (cRecv != 0) {
            return -1;
        }

        g.addPawnInGrille(xRecv, yRecv, intToStringPawn(pRecv));

        System.out.println("The opponent played :");
        g.printGrille();
        return 0;
    }

    /**
     * Main function to communicate with the player and the prolog file
     * @param args the arguments array
     */
    public static void main (String[] args) {

        int port = 2567;
        String addr = "127.0.0.1";
        Scanner sc = new Scanner(System.in);
        Grille g = new Grille();
        Prolog p = new Prolog();
        g.printGrille();

        try{
            Socket sock = new Socket(addr,port);
            DataInputStream DIS = new DataInputStream(sock.getInputStream());
            DataOutputStream DOS = new DataOutputStream(sock.getOutputStream());

            int playFirst = DIS.readInt();
            int end = 0;
            int i = 0;

            do {
                i++;
                if (playFirst == 0) {
                    end = playFirst(g, i , DIS, DOS);
                    if (end != 0) {
                        break;
                    }
                }
                if (playFirst != 0) {
                    end = playSecond(g, i, DIS, DOS);
                    if (end != 0) {
                        break;
                    }
                }
            } while (end != 1) ;

            System.out.println("FIN DE LA PREMIERE PARTIE\nDebut de la revanche :\n");
            end = 0;
            i = 0;

            do {
                i++;
                if (playFirst == 0) {
                    end = playSecond(g, i, DIS, DOS);
                    if (end != 0) {
                        break;
                    }
                }
                if (playFirst != 0) {
                    end = playFirst(g, i , DIS, DOS);
                    if (end != 0) {
                        break;
                    }
                }
            } while (end != 1);

            DIS.close();
            DOS.close();
            sock.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
