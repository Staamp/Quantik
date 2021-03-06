package org.example;


import org.jpl7.Atom;
import org.jpl7.Query;
import org.jpl7.Term;
import org.jpl7.Variable;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

public class PrologTest {

    Prolog p = new Prolog();
    Grille g = new Grille();

    @Test
    public void testConsult() {
        Query q1 = p.consult();
        assertTrue(q1.hasSolution());
        q1.close();
    }

    @Test
    public void testRandom() {
        int r = -1;
        for (int i = 0; i < 100; i++) {
            r = p.random();
            assertTrue(r > -1 && r < 4);
        }
    }

    @Test
    public void testJouerCoupRandomSurCaseVide() {
        int[] r;
        for (int i = 0; i < 10; i++) {
            r = p.jouerCoupRandomSurCaseVide(g);
            assertTrue(r[0] > -1 && r[0] < 4);
            assertTrue(r[1] > -1 && r[1] < 4);
            assertTrue(r[2] > -1 && r[2] < 4);
        }
    }

    @Test
    public void testPionToInt() {
        assertEquals(0,p.pionToInt("c"));
        assertEquals(1,p.pionToInt("p"));
        assertEquals(2,p.pionToInt("s"));
        assertEquals(3,p.pionToInt("t"));
        assertEquals(-1,p.pionToInt("x"));
    }

    @Test
    public void testJouerCoup() {
        int[] r;
        for (int i = 0; i < 7; i++) {
            r = p.jouerCoup(g);
            assertTrue(r[0] > -1 && r[0] < 4);
            assertTrue(r[1] > -1 && r[1] < 4);
            assertTrue(r[2] > -1 && r[2] < 4);
        }
    }

    @Test
    public void testPartie() {
        g.printGrille();
        g.addPawnInGrille(0,0,"p");
        g.printGrille();
    }

    @Test
    public void testHeuristique1() {
        int[] r = p.jouerCoupHeuristique(g);
        assertEquals(r[0],3);
        assertEquals(r[1],3);
        assertTrue(r[2] > -1 && r[2] < 4);
    }

    @Test
    public void testHeuristique2() {
        g.addPawnInGrille(0,0,"c");
        g.addPawnInGrille(0,1,"p");
        g.addPawnInGrille(0,2,"s");

        int[] r = p.jouerCoupHeuristique(g);
        assertEquals(0,r[0]);
        assertEquals(3,r[1]);
        assertEquals(3,r[2]);
    }

    @Test
    public void testHeuristique3() {
        g.addPawnInGrille(0,0,"c");
        g.addPawnInGrille(1,0,"p");
        g.addPawnInGrille(2,0,"s");

        int[] r = p.jouerCoupHeuristique(g);
        assertEquals(3,r[0]);
        assertEquals(0,r[1]);
        assertEquals(3,r[2]);
    }

    @Test
    public void testHeuristique4() {
        g.addPawnInGrille(0,0,"c");
        g.addPawnInGrille(0,1,"p");
        g.addPawnInGrille(1,0,"s");

        int[] r = p.jouerCoupHeuristique(g);
        assertEquals(1,r[0]);
        assertEquals(1,r[1]);
        assertEquals(3,r[2]);
    }

    @Test
    public void testHeuristique5() {
        g.addPawnInGrille(0,0,"c");
        g.addPawnInGrille(0,1,"p");
        g.addPawnInGrille(1,2,"s");
        g.addPawnInGrille(1,3,"t");

        g.addPawnInGrille(2,0,"s");
        g.addPawnInGrille(2,1,"t");
        g.addPawnInGrille(2,2,"c");
        g.addPawnInGrille(2,3,"p");

        try {
            int[] r = p.jouerCoupHeuristique(g);
            assertEquals(1,r[0]);
            assertEquals(1,r[1]);
            assertEquals(3,r[2]);
        } catch (NullPointerException e) {
            Assert.assertNull(e.getMessage());
        }

    }

}
