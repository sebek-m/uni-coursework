package misc;

import java.util.ArrayList;

/**
 * Klasa odpowiadająca za przechowywanie i operowanie
 * na projekcie odcinków, które mają być otrzymane
 * z kupowanych w prętów
 */
public class Project {
    /**
     * Długości oczekiwanych odcinków
     */
    private ArrayList<Integer> rodLengths;

    public Project(ArrayList<Integer> rodLengths) {
        this.rodLengths = rodLengths;
    }

    /**
     * @return liczba odcinków w projekcie
     */
    public int segmentNumber() {
        return rodLengths.size();
    }

    /**
     * Podaje długość odcinka, która znajduje się
     * pod zadanym indeksem
     *
     * @param position indeks, pod którym znajduje się
     *                 oczekiwana długość odcinka
     * @return długość oczekiwanego odcinka
     */
    public int segmentLength(int position) {
        return rodLengths.get(position);
    }

    /**
     * Wykreśla odcinek znajdujący się pod zadanym indeksem
     * z projektu
     *
     * @param position indeks, pod którym znajduje się
     *                 długość odcinka, która ma zniknąć
     */
    public void crossSegmentOut(int position) {
        rodLengths.remove(position);
    }

    /**
     * @return długość najdłuższego odcinka w projekcie
     */
    public int maxSegmentLength() {
        return rodLengths.get(rodLengths.size() - 1);
    }

    /**
     * @return długość najkrótszego odcinka w projekcie
     */
    public int minSegmentLength() {
        return rodLengths.get(0);
    }

    /**
     * @return czy są jeszcze jakieś odcinki w projekcie
     */
    public boolean segmentsLeft() {
        return rodLengths.size() > 0;
    }

    /**
     * @return suma odcinków w projekcie
     */
    public int lengthSum() {
        int sum = 0;

        for (Integer length : rodLengths)
            sum += length;

        return sum;
    }
}
