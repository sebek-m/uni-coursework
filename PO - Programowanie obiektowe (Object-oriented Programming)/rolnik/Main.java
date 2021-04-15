public class Main {

    public static void main(String[] args) {
        Ogród ogród1 = new Ogród(4);
        PracownikPGR wiesiu = new PracownikPGR("Wiesiu");

        wiesiu.simulate(ogród1, 30);
        System.out.println();

        Ogród ogród2 = new Ogród(5);
        Gospodarz marian = new Gospodarz("Marian", ogród2.getLiczbaMiejsc());

        marian.simulate(ogród2, 30);
        System.out.println();

        Ogród ogród3 = new Ogród(5);
        Szczęściarz stachu = new Szczęściarz("Stachu");

        stachu.simulate(ogród3, 60);
    }
}