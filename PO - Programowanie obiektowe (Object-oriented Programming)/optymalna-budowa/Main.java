import engineer.Engineer;
import shopping.PriceList;
import shopping.Salesman;
import misc.DataReader;
import misc.Project;

/**
 * Programowanie obiektowe - Projekt 1
 * Optymalna budowa
 *
 * Program wyznacza optymalny sposób pocięcia prętów dostępnych do kupienia
 * na odcinki konkretnych długości według jednej z czterech strategii:
 * minimalistycznej, maksymalistycznej, ekonomicznej lub ekologicznej.
 *
 * @author Sebastian Miller
 * Data ostatniej wersji: 25.04.2019
 */
public class Main {

    public static void main(String[] args) {
        DataReader reader = new DataReader();

        PriceList priceList = new PriceList(reader.readPriceList());
        Salesman salesman = new Salesman(priceList);

        Project project = new Project(reader.readRodLengths());
        Engineer engineer = Engineer.newEngineer(reader.readStrategy());

        engineer.plan(salesman, project);
    }
}
