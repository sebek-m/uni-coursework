package engineer;

import misc.Project;
import rod.UncutRod;

/**
 * Abstrakcyjna podklasa inżyniera, której podklasy reprezentują
 * strategie zachłanne.
 */
public abstract class Greedy extends Engineer {
    /**
     * Jeśli aktualnie cięty pręt ma nadal wystarczającą
     * długość, żeby uciąć odcinek z projektu, znajdujący się
     * aktualnie pod indeksem 'i', to wykonywane jest cięcie.
     * W przeciwnym wypadku funkcja nic nie robi.
     *
     * @param rod aktualnie cięty pręt
     * @param project projekt
     * @param i indeks, pod którym w projekcie znajduje się
     *          sprawdzana długość odcinka
     */
    private void maybeCut(UncutRod rod, Project project, int i) {
        int plannedSegment = project.segmentLength(i);

        if (rod.cut(plannedSegment)) {
            project.crossSegmentOut(i);
        }
    }

    /**
     * Tnie zadany pręt na odcinki z projektu w kolejności nierosnącej
     * tak długo, jak w oryginalnym pręcie starcza długości do wykonania
     * kolejnego cięcia.
     *
     * @param rod pręty, który ma być pocięty
     * @param project projekt
     */
    public void cutWhilePossible(UncutRod rod, Project project) {
        int i = project.segmentNumber() - 1;

        for (; i >= 0; i--)
            maybeCut(rod, project, i);

        // Wyjście z pętli oznacza, że nie ma już odcinków do ucięcia,
        // albo że każdy z nadal nieotrzymanych odcinków jest dłuższy
        // od pozostałej części ciętego pręta. W obu przypadkach
        // ta pozostała część staje się odpadem.
        this.wasteLength += rod.getLength();
    }
}
