package engineer;

import misc.Project;
import rod.UncutRod;
import shopping.Salesman;

/**
 * Klasa reprezentująca strategię maksymalistyczną, czyli taką,
 * w której zawsze kupowany jest najdłuższy dostępny pręt,
 * po czym cięty jest tak długo, jak to możliwe.
 */
public class Maximalist extends Greedy {
    /**
     * Dostęp package-private, ponieważ konstruktor wywoływany jest
     * tylko poprzez statyczną metodę newEngineer() w nadklasie engineer.Engineer.
     */
    Maximalist() {
    }

    /**
     * Dokonuje zakupu najdłuższego pręta dostępnego u sprzedawcy.
     *
     * @param salesman sprzedawca
     * @return zakupiony najdłuższy pręt
     */
    private UncutRod buyLongest(Salesman salesman) {
        UncutRod newRod = salesman.sellLongest(this);
        purchasedRods.add(newRod);

        return newRod;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void plan(Salesman salesman, Project project) {
        while (project.segmentsLeft()) {
            UncutRod newLongest = buyLongest(salesman);

            cutWhilePossible(newLongest, project);
        }

        report();
    }
}
