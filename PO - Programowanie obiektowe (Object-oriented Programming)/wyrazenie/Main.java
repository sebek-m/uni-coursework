public class Main {

    public static void main(String[] args) {
        Sin sin = new Sin(new Potęga(new Zmienna(), 7));
        Potęga pot = new Potęga(new Plus(new Zmienna(),
                new Potęga(new Zmienna(), 2.5f)), 5);
        Razy razy = new Razy(sin, pot);

        System.out.println(pot.toString());
        System.out.println(pot.policz(2));
        System.out.println(pot.całka(1, 3));

        System.out.println(razy.toString());
        System.out.println(razy.policz(2));
        System.out.println(razy.całka(0.72f, 3));

        Log log = new Log(new Plus(new Zmienna(), new Cos(new Stała(12))));

        System.out.println(log.toString());
    }
}
