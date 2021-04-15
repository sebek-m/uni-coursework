import java.util.Random;

public abstract class Warzywo {
    protected int cenaZasadzenia;
    protected final long czasZasadzenia = System.currentTimeMillis();
    protected int wartośćPrzyZebraniu;
    public static final Random generator = new Random();

    private static final int liczbaDostępnychWarzyw = 5;
    private static final int ziemniak = 0;
    private static final int pomidor = 1;
    private static final int rzodkiewka = 2;
    private static final int ogórek = 3;
    private static final int burak = 4;

    protected abstract String odmienionaNazwa();

    protected abstract int obliczWartość();

    public abstract Warzywo noweToSamo();

    public long getCzasZasadzenia() {
        return this.czasZasadzenia;
    }

    public static float millisToSec(long millis) {
        float millisAsFloat = millis;
        return (millisAsFloat / 1000) % 60;
    }

    public static int millisToSecInt(long millis) {
        int sec = Math.toIntExact(millis / 1000) % 60;
        return sec;
    }

    public int getCenaZasadzenia() {
        return this.cenaZasadzenia;
    }

    public int getWartośćPrzyZebraniu() {
        return this.wartośćPrzyZebraniu;
    }

    public void setWartośćPrzyZebraniu(int wartość) {
        this.wartośćPrzyZebraniu = wartość;
    }

    protected long ileCzasuNaPolu() {
        return System.currentTimeMillis() - this.czasZasadzenia;
    }

    public static Warzywo losoweWarzywo() {
        int random = generator.nextInt(liczbaDostępnychWarzyw);
        Warzywo noweWarzywo;

        switch (random) {
            case ziemniak:
                noweWarzywo = new Ziemniak();
                break;
            case pomidor:
                noweWarzywo = new Pomidor();
                break;
            case rzodkiewka:
                noweWarzywo = new Rzodkiewka();
                break;
            case ogórek:
                noweWarzywo = new Ogórek();
                break;
            case burak:
                noweWarzywo = new Burak();
                break;
            default:
                noweWarzywo = null;
                break;
        }

        return noweWarzywo;
    }

    public String wartośćString() {
        return "Wartość: " + this.wartośćPrzyZebraniu + " PLN";
    }

    public String toString() {
        return this.getClass().getSimpleName() + ", " + this.wartośćString();
    }
}

class Ziemniak extends Warzywo {
    public Ziemniak() {
        this.cenaZasadzenia = 1;
    }

    public String odmienionaNazwa() {
        return "ziemniaka";
    }

    public int obliczWartość() {
        long czasDojrzewania = ileCzasuNaPolu();

        if (czasDojrzewania <= 10000)
            return 0;
        else
            return 5;
    }

    public Warzywo noweToSamo() {
        return new Ziemniak();
    }
}

class Pomidor extends Warzywo {
    public Pomidor() {
        this.cenaZasadzenia = 6;
    }

    public String odmienionaNazwa() {
        return "pomidora";
    }

    public int obliczWartość() {
        float czasDojrzewania = millisToSec(ileCzasuNaPolu());
        int krok = 2;

        if (czasDojrzewania <= 10)
            return 0;
        else if (czasDojrzewania <= 15)
            return Math.round((czasDojrzewania - 10) * krok);
        else {
            float strata = (czasDojrzewania - 15) * krok;
            if (strata < 10)
                return Math.round(10 - strata);
            else
                return 0;
        }
    }

    public Warzywo noweToSamo() {
        return new Pomidor();
    }
}

class Rzodkiewka extends Warzywo {
    public Rzodkiewka() {
        this.cenaZasadzenia = 2;
    }

    public String odmienionaNazwa() {
        return "rzodkiewkę";
    }

    public int obliczWartość() {
        float czasDojrzewania = millisToSec(ileCzasuNaPolu());

        if (czasDojrzewania <= 20)
            return Math.round(czasDojrzewania);
        return 0;
    }

    public Warzywo noweToSamo() {
        return new Rzodkiewka();
    }
}

class Ogórek extends Warzywo {
    public Ogórek() {
        this.cenaZasadzenia = 4;
    }

    public String odmienionaNazwa() {
        return "ogórka";
    }

    public int obliczWartość() {
        float czasDojrzewania = millisToSec(ileCzasuNaPolu());
        int krok = 2;

        if (czasDojrzewania <= 5)
            return 10;
        else if (czasDojrzewania <= 10)
            return Math.round((10 - ((czasDojrzewania - 5) * krok)) % 10);
        else if (czasDojrzewania <= 15)
            return Math.round((czasDojrzewania - 10) * krok);
        else
            return 0;
    }

    public Warzywo noweToSamo() {
        return new Ogórek();
    }
}

class Burak extends Warzywo {
    public Burak() {
        this.cenaZasadzenia = 3;
    }

    public String odmienionaNazwa() {
        return "buraka";
    }

    public int obliczWartość() {
        float czasDojrzewania = millisToSec(ileCzasuNaPolu());
        int krok = 3;

        if (czasDojrzewania <= 5) {
            return 0;
        }
        else if (czasDojrzewania <= 10) {
            return 7;
        }
        else if (czasDojrzewania <= 15) {
            return 30;
        }
        else {
            float strata = (czasDojrzewania - 15) * krok;
            if (strata < 30)
                return Math.round(30 - strata);
            else
                return 0;
        }
    }

    public Warzywo noweToSamo() {
        return new Burak();
    }
}