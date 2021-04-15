package shopping;

import engineer.Smart;
import rod.PricedRod;
import misc.Combination;
import misc.Project;

/**
 * Klasa reprezentująca cennik. Zawiera metody potrzebne do
 * znajdywania prętów o odpowiednich, oczekiwanych cechach.
 */
public class PriceList {
    /**
     * Właściwy cennik, wcześniej wczytany z wejścia
     */
    private PricedRod[] pricedRods;

    public PriceList(PricedRod[] pricedRods) {
        this.pricedRods = pricedRods;
    }

    /**
     * Zwraca najdłuższy wyceniony pręt z cennika
     *
     * @return najdłuższy pręt
     */
    public PricedRod findLongestRod() {
        return pricedRods[pricedRods.length - 1];
    }

    /**
     * Binsearch znajdujący indeks, pod którym w cenniku
     * znajduje się najkrótszy pręt, który wystarczy do otrzymania
     * odcinka o zadanej długości.
     *
     * @param targetLength oczekiwana długość odcinka
     * @return indeks, pod którym w cenniku znajduje się wystarczający pręt
     */
    private int findSufficientIndex(int targetLength) {
        int left = 0;
        int right = pricedRods.length - 1;

        while (left < right) {
            int mid = (left + right) / 2;

            if (pricedRods[mid].getLength() >= targetLength)
                right = mid;
            else
                left = mid + 1;
        }

        return left;
    }

    /**
     * Znajduje w cenniku najkrótszy pręt, który wystarczy do otrzymania
     * odcinka o zadanej długości.
     *
     * @param targetLength oczekiwana długość odcinka
     * @return wystarczający wyceniony pręt
     */
    public PricedRod findSufficientRod(int targetLength) {
        int index = findSufficientIndex(targetLength);

        return pricedRods[index];
    }

    /**
     * Podaje wyceniony pręt, który znajduje się w cenniku
     * pod zadanym indeksem.
     *
     * @param index indeks, pod którym znajduje się pręt
     * @return pręt z cennika pod zadanym indeksem
     */
    private PricedRod getRod(int index) {
        return pricedRods[index];
    }

    /**
     * Sprawdza, czy kombinacja, na której wykonywano próbne cięcia
     * rzeczywiście jest dotychczas najbardziej optymalnym rozwiązaniem,
     * czyli czy udało się z niej otrzymać wszystkie odcinki z projektu.
     * Jest to równoważne ze sprawdzeniem, czy przypisano minComb jakąś
     * kombinację prawidłowo pociętych prętów.
     * Dodatkowo sprawdzane jest, czy minimalna kombinacja była pełna
     * przed próbnymi cięciami, żeby przypadkiem nie przypisać pustej
     * minimalnej kombinacji informacji, że jest pelna.
     *
     * @param minComb aktualna optymalna kombinacja
     * @param wasEmpty informacja czy minimalna kombinacja była pusta
     *                 przed próbnymi cięciami
     * @return informacja, czy cięcia według projektu były możliwe w sprawdzanej
     * kombinacji
     */
    private boolean segmentsDontFit(Combination minComb, boolean wasEmpty) {
        return minComb.isEmpty() && !wasEmpty;
    }

    /**
     * Sprawdza, czy rozważana kombinacja nie ma już zbyt dużej sumarycznej długości,
     * czyli czy ta długość nie przekracza sumy długości odcinków z projektu z dodaną
     * długością najdłuższego pręta w sprzedaży.
     * Sumaryczna długość kombinacji nie może przekroczyć wspomnianej sumy,
     * ponieważ oznaczałoby to, że na pewno istnieje inna, krótsza,
     * prawidłowa kombinacja.
     *
     * @param combination sprawdzana kombinacja
     * @return informacja o tym, czy sumaryczna długość kombinacji nie jest za duża
     */
    private boolean notTooLong(Combination combination) {
        int maxRodLength = findLongestRod().getLength();
        return combination.getTargetLengthSum() + maxRodLength
                > combination.getLengthSum();
    }

    /**
     * Sprawdza, czy zadaną kombinację można pociąć na zadane w projekcie odcinki.
     * Jeśli tak, to oznacza to, że ta kombinacja jest nową aktualnie optymalną
     * kombinacją, więc przypisuje tę kombinację do minComb.
     *
     * @param segmentIndex indeks, pod ktorym w projekcie znajduje się odcinek,
     *                     który aktualnie próbujemy uciąć
     * @param project projekt odcinków
     * @param combination sprawdzana kombinacja
     * @param minComb aktualnie optymalna kombinacja
     */
    private void checkIfCutsPossible(int segmentIndex, Project project,
                            Combination combination, Combination minComb) {
        // Udało się odciąć wszystkie odcinki zadane w projekcie,
        // więc sprawdzana kombinacja naprawdę jest nową optymalną
        if (segmentIndex < 0) {
            minComb.transform(combination);
        }
        else {
            int nextSegmentLength = project.segmentLength(segmentIndex);

            // Pętla zakończy się, gdy spróbowano już odciąć aktualnie rozważany
            // odcinek we wszystkich prętach z kombinacji (dzieje się tak
            // gdy nie udało się pociąć tej kombinacji na zadane odcinki,
            // ale także, gdy udało się to dopiero po próbie odcięcia aktualnego
            // odcinka z ostatniego pręta w kombinacji) lub gdy w którymś momencie
            // wgłąb rekurencji udało się już odciąć wszystkie odcinki
            for (int i = 0; i < combination.size() && minComb.isEmpty(); i++) {
                // Cięcie się udało, więc próbujemy rekurencyjnie z kolejnym
                // odcinkiem z projektu
                if (combination.getUncut(i).cut(nextSegmentLength))
                    checkIfCutsPossible(segmentIndex - 1, project, combination, minComb);
            }
        }
    }

    /**
     * Przygotowuje aktualnie optymalną kombinację do ewentualnego procesu
     * decyzji, czy zadana kombinacja jest nową optymalną. Następnie wywołuje
     * sprawdzanie, czy odcinki zadane w projekcie rzeczywiście można otrzymać
     * w zadanej kombinacji. Jeśli można, to po zakończeniu działania funkcji
     * checkIfCutsPossible minComb ma już przypisaną nową optymalną kombinację,
     * a w przeciwnym wypadku trzeba przywrócić jej status optymalnej.
     *
     * @param combination sprawdzana kombinacja
     * @param minComb aktualnie optymalna kombinacja
     * @param project projekt odcinków
     */
    private void checkIfNewOptimal(Combination combination, Combination minComb, Project project) {
        // Najpierw w sprawdzanej kombinacji trzeba przekształcić
        // przechowywane wycenione pręty w te do cięcia
        combination.prepareForCutting();

        // Zapamiętujemy, czy znaleziono już jakąś "optymalną" kombinację
        boolean wasEmpty = minComb.isEmpty();
        // Jest to potrzebne, ponieważ pętla w checkIfCutsPossible zależy od tego,
        // czy optymalna kombinacja jest pusta
        if (!wasEmpty)
            minComb.makeFakeEmpty();

        checkIfCutsPossible(project.segmentNumber() - 1,
                project, combination, minComb);

        if (segmentsDontFit(minComb, wasEmpty))
            minComb.restoreRealNotEmpty();
    }

    /**
     * Do danej kombinacji dodaje jeden pręt z cennika i wywołuje sprawdzanie
     * (checkThisComb) powstałej kombinacji. Na końcu próby usuwa dodany pręt
     * z kombinacji, by móc przetestować pierwotną kombinację
     * z, dodanym w następnym obrocie pętli, kolejnym prętem z cennika.
     *
     * @param combination kombinacje, której rozszerzenia będą sprawdzane
     * @param minComb aktualnie optymalna kombinacja
     * @param customer inżynier ze strategią ekonomiczną/ekologiczną
     * @param project projekt cięć odcinków
     */
    private void checkAllNext(Combination combination, Combination minComb,
                              Smart customer, Project project) {
        // Rozważamy tylko te pręty z cennika, które są dłuższe od najkrótszego
        // odcinka z projektu.
        for (int i = combination.getStartIndex(); i < pricedRods.length; i++) {
            PricedRod nextRod = getRod(i);
            combination.addRod(nextRod);

            checkThisComb(combination, minComb, customer, project);

            combination.removeRod();
        }
    }

    /**
     * Rekurencyjna (rekurencyjne wywołanie ukryte w checkAllNext) metoda,
     * która sprawdza czy dana kombinacja potencjalnie jest lub, po dodaniu
     * kolejnych prętów, ma szansę być nową optymalną kombinacją i podejmuje
     * dalsze kroki, czyli próbne cięcia kombinacji lub sprawdzanie kolejnych
     * kombinacji, w zależności od wyniku tego sprawdzenia. Jeśli kombinacja
     * nie ma szans na bycie optymalną, funkcja nic nie robi.
     *
     * @param combination sprawdzana kombinacja
     * @param minComb aktualnie optymalna kombinacja
     * @param customer inżynier ze strategią ekonomiczną/ekologiczną,
     *                 który potrzebuje optymalnej kombinacji
     * @param project projekt cięcia odcinków
     */
    private void checkThisComb(Combination combination, Combination minComb,
                               Smart customer, Project project) {
        // Kombinacja jest odpowiedniej długości i spełnia warunek
        // danej strategii
        if (combination.sufficientLength() &&
                customer.isPotentialMinimum(combination, minComb)) {
            checkIfNewOptimal(combination, minComb, project);
        }
        // Kombinacja jest za krótka lub nie spełnia warunku,
        // ale na pewno nie jest za długa
        else if (notTooLong(combination)){
            checkAllNext(combination, minComb, customer, project);
        }
    }

    /**
     * Znajduje optymalną, według strategii inżyniera customer (ekonomicznej/
     * ekologicznej), kombinację prętów z cennika.
     *
     * @param customer obsługiwany inżynier ze strategią ekonomiczną lub ekologiczną
     * @param project projekt cięcia prętów
     * @return optymalna kombinacja z już pociętymi prętami
     */
    public Combination findBestCombination(Smart customer, Project project) {
        // Jednym z podstawowych kryteriów w szukaniu optymalnej kombinacji
        // jest porównywanie jej sumarycznej długości z sumą długości odcinków
        // z projektu
        int segLengthsSum = project.lengthSum();
        // Nie chcemy rozważać prętów z cennika, które są krótsze od każdego
        // odcinka z projektu
        int startIndex = findSufficientIndex(project.minSegmentLength());

        Combination bestComb = new Combination(segLengthsSum, startIndex);
        Combination activeComb = new Combination(segLengthsSum, startIndex);

        checkAllNext(activeComb, bestComb, customer, project);

        return bestComb;
    }
}
