package shopping;

import engineer.*;
import rod.PricedRod;
import rod.UncutRod;
import misc.Combination;
import misc.Project;

import java.util.ArrayList;

/**
 * Klasa reprezentująca sprzedawcę, który we współpracy
 * ze swoim cennikiem (shopping.PriceList) przekazuje inżynierowi
 * odpowiednie pręty, dodatkowo pobierając za nie opłatę.
 */
public class Salesman {
    /**
     * Cennik sprzedawcy wcześniej wczytany z wejścia
     */
    private PriceList priceList;

    public Salesman(PriceList priceList) {
        this.priceList = priceList;
    }

    /**
     * "Prosi" inżyniera o zapłatę podanej kwoty.
     *
     * @param price kwota do zapłaty
     * @param customer inżynier, który dokonuje zakupu
     */
    private void askForMoney(int price, Engineer customer) {
        customer.pay(price);
    }

    /**
     * Pomocnicza metoda do poinformowania inżyniera o odpadach,
     * które zostały z cięcia prętów. Używana tylko przy obsłudze
     * strategii ekonomicznej i ekologicznej.
     *
     * @param waste sumaryczna długość odpadów
     * @param customer inżynier ze strategią ekonomiczną lub ekologiczną,
     *                 który dokonuje zakupu
     */
    private void informAboutWaste(int waste, Smart customer) {
        customer.acceptWaste(waste);
    }

    /**
     * Sprzedaje inżynierowi najkrótszy pręt z cennika,
     * który ma wystarczającą długość, żeby moc uciąć
     * z niego odcinek zadanej długości. Metoda używana
     * przy obsłudze strategii minimalistycznej.
     *
     * @param targetLength długość oczekiwanego odcinka
     * @param customer inżynier, który dokonuje zakupu
     * @return pręt, który wystarczy na ucięcie oczekiwanego odcinka
     */
    public UncutRod sellShortestSufficient(int targetLength, Engineer customer) {
        PricedRod targetRod = priceList.findSufficientRod(targetLength);

        askForMoney(targetRod.getPrice(), customer);

        return targetRod.prepareForSale();
    }

    /**
     * Sprzedaje inżynierowi najdłuższy pręt z cennika.
     * Metoda używana przy obsłudze strategii maksymalistycznej.
     *
     * @param customer inżynier, który dokonuje zakupu
     * @return najdłuższy pręt z cennika
     */
    public UncutRod sellLongest(Engineer customer) {
        PricedRod targetRod = priceList.findLongestRod();

        askForMoney(targetRod.getPrice(), customer);

        return targetRod.prepareForSale();
    }

    /**
     * Sprzedaje inżynierowi ze strategią ekonomiczną lub ekologiczną
     * zbiór już pociętych prętów, które są optymalnym, zgodnym z odpowiednią
     * strategią rozwiązaniem projektu. Dodatkowo informuje go o powstałych
     * w tym procesie odpadach.
     *
     * @param customer inżynier, który dokonuje zakupu
     * @param project projekt
     * @return zbiór prętów, który jest optymalnym rozwiązaniem
     */
    public ArrayList<UncutRod> sellBestCombination(Smart customer, Project project) {
        Combination bestComb = priceList.findBestCombination(customer, project);

        askForMoney(bestComb.getPriceSum(), customer);
        informAboutWaste(bestComb.getWasteSum(), customer);

        return bestComb.getCutCombination();
    }
}
