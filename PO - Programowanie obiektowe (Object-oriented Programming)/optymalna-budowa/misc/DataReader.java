package misc;

import rod.PricedRod;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * Klasa obsługująca czytanie odpowiednia sformatowanych
 * danych o budowie ze standardowego wejścia.
 */
public class DataReader {
    /**
     * Jeden używany Scanner
     */
    private Scanner scan;

    public DataReader() {
        scan  = new Scanner(System.in);
    }

    /**
     * Czyta jedną pozycję, która ma znaleźć
     * się w cenniku, czyli długość pręta
     * i jego cenę
     *
     * @return instancja wycenionego pręta
     */
    private PricedRod readOnePricedRod() {
        int rodLength = scan.nextInt();
        int rodPrice = scan.nextInt();

        return new PricedRod(rodLength, rodPrice);
    }

    /**
     * Czyta informacje o cenniku.
     *
     * @return tablica z wycenionymi prętami,
     *         później użyta przy tworzeniu
     *         instancji cennika
     */
    public PricedRod[] readPriceList() {
        int priceListSize = scan.nextInt();
        PricedRod[] newPrices = new PricedRod[priceListSize];

        for (int i = 0; i < priceListSize; i++)
            newPrices[i] = readOnePricedRod();

        return newPrices;
    }

    /**
     * Czyta długości odcinków, które mają znaleźć
     * się w projekcie.
     *
     * @return ArrayList z długościami,
     *         później użyty przy tworzeniu
     *         instancji projektu
     */
    public ArrayList<Integer> readRodLengths() {
        int rodNumber = scan.nextInt();
        ArrayList<Integer> rodLengths = new ArrayList<>();

        for (int i = 0; i < rodNumber; i++)
            rodLengths.add(scan.nextInt());

        scan.nextLine();

        return rodLengths;
    }

    /**
     * Czyta nazwę strategii, która ma zostać
     * użyta w planowaniu.
     *
     * @return nazwa strategii
     */
    public String readStrategy() {
        return scan.nextLine();
    }
}
