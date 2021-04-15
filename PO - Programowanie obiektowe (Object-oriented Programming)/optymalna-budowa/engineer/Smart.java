package engineer;

import shopping.Salesman;
import misc.Combination;
import misc.Project;

/**
 * Abstrakcyjna podklasa inżyniera, której podklasy reprezentują
 * strategie wymagające rozwiązania problemu plecakowego.
 */
public abstract class Smart extends Engineer {
    /**
     * Sprawdza, czy podana kombinacja spełnia warunek aktywnej
     * strategii na bycie potencjalnym minimum.
     *
     * @param combination sprawdzana kombinacja
     * @param minComb najbardziej optymalna kombinacja
     *                z dotychczas sprawdzanych
     * @return czy kombinacja jest potencjalnym minimum
     */
    public abstract boolean isPotentialMinimum(Combination combination,
                                                  Combination minComb);

    /**
     * Dokonuje zakupu zbioru pociętych prętów, który jest optymalnym
     * rozwiązaniem według strategii ekonomicznej/ekologicznej
     *
     * @param salesman sprzedawca
     * @param project projekt odcinków
     */
    private void buyBestCombination(Salesman salesman, Project project) {
        // Wykluczamy edge case z pustym projektem lub projektem z samymi
        // odcinkami o długości 0
        if (project.segmentsLeft() && project.minSegmentLength() > 0)
            this.purchasedRods = salesman.sellBestCombination(this, project);
    }

    /**
     * Zapisuje informację o sumie długości odpadów powstałych przy cięciu prętów
     *
     * @param waste suma długości powstałych odpadów
     */
    public void acceptWaste(int waste) {
        wasteLength += waste;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void plan(Salesman salesman, Project project) {
        buyBestCombination(salesman, project);

        report();
    }
}
