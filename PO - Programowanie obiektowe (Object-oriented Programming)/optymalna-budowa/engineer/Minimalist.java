package engineer;

import misc.Project;
import rod.UncutRod;
import shopping.Salesman;

/**
 * Klasa reprezentująca strategię minimalistyczną, czyli taką,
 * w której w danym momencie kupowany jest najkrótszy możliwy pręt,
 * który wystarczy do otrzymania najdłuższego z pozostałych odcinków
 * w projekcie, po czym następuje cięcie tego pręta na odcinki z projektu
 * tak długo, jak to możliwe.
 */
public class Minimalist extends Greedy {
    /**
     * Dostęp package-private, ponieważ konstruktor wywoływany jest
     * tylko poprzez statyczną metodę newEngineer() w nadklasie engineer.Engineer.
     */
    Minimalist() {
    }

    /**
     * Dokonuje zakupu najkrótszego pręta ze sklepu, który jest dłuższy
     * lub ma tę samą długość, co aktualnie najdłuższy odcinek z projektu.
     *
     * @param salesman sprzedawca
     * @param project projekt
     * @return zakupiony wystarczający pręt
     */
    private UncutRod buyShortestSufficient(Salesman salesman, Project project) {
        int maxSegmentLength = project.maxSegmentLength();

        UncutRod newRod = salesman.sellShortestSufficient(maxSegmentLength, this);
        purchasedRods.add(newRod);

        return newRod;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void plan(Salesman salesman, Project project) {
        while (project.segmentsLeft()) {
            UncutRod newLongest = buyShortestSufficient(salesman, project);

            cutWhilePossible(newLongest, project);
        }

        report();
    }
}
