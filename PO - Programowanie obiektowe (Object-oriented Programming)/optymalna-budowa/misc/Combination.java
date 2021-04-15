package misc;

import rod.PricedRod;
import rod.UncutRod;

import java.util.ArrayList;

/**
 * Klasa przydatna przy znajdywaniu optymalnych rozwiązań
 * według strategii ekonomicznej/ekologicznej. Zawiera ważne
 * informacje, sam zbiór prętów wybranych z cennika, oraz metody
 * do manipulowania tym zbiorem.
 */
public class Combination {
    private int priceSum;
    private int lengthSum;
    private int wasteSum;
    private ArrayList<PricedRod> rods;
    private ArrayList<UncutRod> cutCombination;
    private boolean empty;
    private int targetLengthSum;
    private int startIndex;

    public Combination(int targetLengthSum, int startIndex) {
        this.priceSum = 0;
        this.lengthSum = 0;
        this.wasteSum = 0;
        this.rods = new ArrayList<>();
        // cutCombination i tak jest otrzymywana w odpowiednim
        // momencie na podstawie rods, więc na początku
        // jej nie potrzebujemy
        this.cutCombination = null;
        this.empty = true;
        this.targetLengthSum = targetLengthSum;
        this.startIndex = startIndex;
    }

    /**
     * Dodaje zadany pręt do zbioru
     *
     * @param rod wyceniony pręt do dodania
     */
    public void addRod(PricedRod rod) {
        rods.add(rod);
        priceSum += rod.getPrice();
        lengthSum += rod.getLength();
    }

    /**
     * Usuwa ostatnio dodany pręt ze zbioru
     */
    public void removeRod() {
        int lastIndex = rods.size() - 1;
        PricedRod target = rods.get(lastIndex);

        priceSum -= target.getPrice();
        lengthSum -= target.getLength();
        rods.remove(lastIndex);
    }

    /**
     * @return suma cen prętów w zbiorze
     */
    public int getPriceSum() {
        return priceSum;
    }

    /**
     * @return suma długości prętów w zbiorze
     */
    public int getLengthSum() {
        return lengthSum;
    }

    /**
     * @return suma długości odpadów powstałych
     * z cięć wykonanych na zbiorze
     */
    public int getWasteSum() {
        return wasteSum;
    }

    /**
     * @return pręty z tego zbioru w formie przygotowanych
     * do cięcia/pociętych
     */
    public ArrayList<UncutRod> getCutCombination() {
        return cutCombination;
    }

    /**
     * @return czy zbiór prętów jest pusty
     */
    public boolean isEmpty() {
        return this.empty;
    }

    /**
     * @return suma długości odcinków, dla których
     * szukamy optymalnego rozwiązania
     */
    public int getTargetLengthSum() {
        return targetLengthSum;
    }

    /**
     * @return indeks, pod którym w cenniku
     * znajduje się najkrótszy pręt, który będzie
     * wystarczający dla zadanych odcinków
     */
    public int getStartIndex() {
        return startIndex;
    }

    /**
     * "Przekształca" tę kombinację prętów w zadaną
     *
     * @param combination kombinacja, w którą chcemy
     *                    przekształcić tę kombinację
     */
    public void transform(Combination combination) {
        this.priceSum = combination.getPriceSum();
        this.lengthSum = combination.getLengthSum();
        this.cutCombination = combination.getCutCombination();
        this.empty = false;
        addWaste();
    }

    /**
     * Przypisuje status bycia pustą tej kombinacji,
     * nawet jeśli nie jest pusta.
     */
    public void makeFakeEmpty(){
        this.empty = true;
    }

    /**
     * Przywraca status bycia pełną tej kombinacji
     */
    public void restoreRealNotEmpty() {
        this.empty = false;
    }

    /**
     * Tworzy "kopię" zbioru wycenionych prętów w tej kombinacji, która zawiera
     * pręty tych samych długości, tylko w wersji do cięcia
     *
     * @return zbiór prętów przygotowanych do cięcia
     */
    private ArrayList<UncutRod> pricedIntoUncut() {
        ArrayList<UncutRod> forSale = new ArrayList<>();

        for (PricedRod pricedRod : rods)
            forSale.add(new UncutRod(pricedRod.getLength()));

        return forSale;
    }

    /**
     * Podaje pręt z tej kombinacji w wersji do cięcia,
     * który znajduje się pod zadanym indeksem.
     *
     * @param index indeks, pod którym znajduje się
     *              żądany pręt
     * @return żądany pręt
     */
    public UncutRod getUncut(int index) {
        return cutCombination.get(index);
    }

    /**
     * Przygotowuje kombinację do cięcia, czyli wywołuje
     * metodę tworzącą "kopię" zbioru prętów w wersji do cięcia
     */
    public void prepareForCutting() {
        this.cutCombination = this.pricedIntoUncut();
        this.empty = false;
    }

    /**
     * @return liczba prętów w kombinacji
     */
    public int size() {
        return rods.size();
    }

    /**
     * Oblicza sumę długości odpadów i przypisuje tę wartość
     * odpowiedniemu atrybutowi
     */
    private void addWaste() {
        int waste = 0;

        for (UncutRod rod : cutCombination)
            waste += rod.getLength();

        this.wasteSum = waste;
    }

    /**
     * Sprawdza, czy kombinacja prętów ma wystarczającą sumaryczną
     * długość, żeby potencjalnie dało się otrzymać z niej oczekiwane
     * odcinki.
     *
     * @return informacja, czy sumaryczna długość jest wystarczająca
     */
    public boolean sufficientLength() {
        return this.getLengthSum() >= this.getTargetLengthSum();
    }
}
