package engineer;

import misc.Project;
import rod.UncutRod;
import shopping.Salesman;

import java.util.ArrayList;

/**
 * Abstrakcyjna klasa inżyniera, który obsługuje
 * planowanie optymalnej budowy. Jej podklasy
 * odpowiadają dostępnym strategiom.
 */
public abstract class Engineer {
    /**
     * Pręty ostatecznie wybrane do budowy. W momencie
     * wypisywania raportu każdy z nich jest już pocięty.
     */
    protected ArrayList<UncutRod> purchasedRods = new ArrayList<>();
    /**
     * Suma wydatków
     */
    protected int expenditure = 0;
    /**
     * Suma długości odpadów
     */
    protected int wasteLength = 0;

    /**
     * Tworzy nową instancję inżyniera z odpowiednią strategią.
     *
     * @param strategy nazwa strategii
     *
     * @return instancja odpowiedniej podklasy
     */
    public static Engineer newEngineer(String strategy) {
        Engineer newEngineer;

        switch(strategy) {
            case "minimalistyczna":
                newEngineer = new Minimalist();
                break;
            case "maksymalistyczna":
                newEngineer = new Maximalist();
                break;
            case "ekonomiczna":
                newEngineer = new Economist();
                break;
            case "ekologiczna":
                newEngineer = new Ecologist();
                break;
            default:
                newEngineer = null;
                break;
        }

        return newEngineer;
    }

    /**
     * Wypisuje na standardowe wyjście informacje
     * o optymalnym wyborze prętów.
     */
    protected void report() {
        System.out.println(expenditure);
        System.out.println(wasteLength);

        for (UncutRod rod : purchasedRods) {
            System.out.println(rod.toString());
        }
    }

    /**
     * Dodaje podaną kwotę do sumy wydatków.
     *
     * @param price kwota do zapłacenia
     */
    public void pay(int price) {
        expenditure += price;
    }

    /**
     * Wywołuje odpowiednie funkcje niezbędne do znalezienia
     * optymalnego wyboru prętów według wybranej strategii.
     * Na koniec wywołuje metodę report(), by wypisać informacje o rozwiązaniu.
     *
     * @param salesman sprzedawca prętów
     * @param project projekt do realizacji
     */
    public abstract void plan(Salesman salesman, Project project);
}
