package rod;

import java.util.ArrayList;

/**
 * Specjalny rodzaj pręta, który służy do bycia ciętym.
 * Nazwa tej klasy odnosi się oczywiście do początkowego stanu
 * pręta, bo docelowo będzie on pocięty, a nie "niepocięty".
 */
public class UncutRod extends Rod {
    /**
     * Początkowa długość pręta tuż po zakupie
     */
    private int originalLength;
    /**
     * Odcinki, na które pocięto pręt
     */
    private ArrayList<Rod> plannedCuts;

    public UncutRod(int length) {
        super(length);
        originalLength = length;
        plannedCuts = new ArrayList<>();
    }

    /**
     * @return oryginalna długość pręta i, po spacji,
     * długości wszystkich odcinków, na które pocięto pręt
     */
    public String toString() {
        StringBuilder ret = new StringBuilder();

        for (Rod seg : plannedCuts)
            ret.append(" ").append(seg.getLength());

        return this.originalLength + ret.toString();
    }

    /**
     * Ucina z pręta odcinek o zadanej długości,
     * jeśli to możliwe.
     *
     * @param segmentLength długość odcinka, który chcemy
     *                      uciąć
     * @return informacja czy udało się uciąć odcinek
     */
    public boolean cut(int segmentLength) {
        if (segmentLength > this.length) {
            return false;
        }

        this.length -= segmentLength;

        Rod newSegment = new Rod(segmentLength);
        plannedCuts.add(newSegment);

        return true;
    }
}
