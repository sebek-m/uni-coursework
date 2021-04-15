package engineer;

import misc.Combination;

public class Economist extends Smart {
    /**
     * Dostęp package-private, ponieważ konstruktor wywoływany jest
     * tylko poprzez statyczną metodę newEngineer() w nadklasie engineer.Engineer.
     */
    Economist(){
    }

    /**
     * {@inheritDoc}
     */
    public boolean isPotentialMinimum(Combination combination, Combination minComb) {
        return minComb.isEmpty() || combination.getPriceSum() < minComb.getPriceSum();
    }
}
