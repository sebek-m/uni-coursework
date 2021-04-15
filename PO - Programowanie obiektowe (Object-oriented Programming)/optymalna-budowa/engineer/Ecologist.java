package engineer;

import misc.Combination;

public class Ecologist extends Smart {
    /**
     * Dostęp package-private, ponieważ konstruktor wywoływany jest
     * tylko poprzez statyczną metodę newEngineer() w nadklasie engineer.Engineer.
     */
    Ecologist() {
    }

    /**
     * {@inheritDoc}
     */
    public boolean isPotentialMinimum(Combination combination, Combination minComb) {
        int wasteSum = combination.getLengthSum() - combination.getTargetLengthSum();

        return minComb.isEmpty() || wasteSum < minComb.getWasteSum();
    }
}
