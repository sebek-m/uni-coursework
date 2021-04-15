import java.util.ArrayList;

public class Wektor<T extends Algebraiczna<T>> {
    private ArrayList<T> pola;
    private boolean pełny;

    static class RóżnaDługośćWektorów extends RuntimeException {
        public RóżnaDługośćWektorów(String message) {
            super(message);
        }
    }
    static class NiepełnyWektor extends RuntimeException {
        public NiepełnyWektor(String message) {
            super(message);
        }
    }
    static class ZaKrótkiWektor extends RuntimeException {}

    public Wektor(int długość) {
        pola = new ArrayList<>();
        while (pola.size() < długość)
            pola.add(null);

        pełny = false;
    }

    public int getDługość() {
        return pola.size();
    }

    public T getWartość(int nrPola) {
        return pola.get(nrPola);
    }

    public boolean isPełny() {
        return pełny;
    }

    private boolean sprawdźCzyPełny() {
        for (T element : pola) {
            if (element == null)
                return false;
        }

        return true;
    }

    public void ustaw(int nrPola, T wartość) throws ZaKrótkiWektor {
        if (nrPola > pola.size() - 1)
            throw new ZaKrótkiWektor();

        pola.set(nrPola, wartość);

        if (sprawdźCzyPełny())
            pełny = true;
    }

    private void checkRóżnaDługość(Wektor<T> drugi) throws RóżnaDługośćWektorów {
        if (pola.size() != drugi.getDługość())
            throw new RóżnaDługośćWektorów("Długość 1: " + pola.size() +
                    ", Długość 2: " + drugi.getDługość());
    }

    private void checkNiepełnyWektor(Wektor<T> drugi) throws NiepełnyWektor {
        if (!pełny || !drugi.isPełny()) {
            StringBuilder pozycje1 = new StringBuilder();
            StringBuilder pozycje2 = new StringBuilder();

            pozycje1.append("W wektorze 1 brakuje elementów na pozycjach: ");
            pozycje2.append("W wektorze 2 brakuje elementów na pozycjach: ");

            for (int i = 0; i < pola.size(); i++) {
                if (pola.get(i) == null)
                    pozycje1.append(i);
                    pozycje1.append(" ");
            }

            for (int i = 0; i < drugi.getDługość(); i++) {
                if (drugi.getWartość(i) == null)
                    pozycje2.append(i);
                pozycje2.append(" ");
            }

            throw new NiepełnyWektor(pozycje1.toString() + '\n' + pozycje2.toString());
        }
    }

    public Wektor<T> dodaj(Wektor<T> drugi) throws RóżnaDługośćWektorów, NiepełnyWektor {
        checkRóżnaDługość(drugi);
        checkNiepełnyWektor(drugi);

        Wektor<T> suma = new Wektor<>(pola.size());
        T wartość;

        for (int i = 0; i < pola.size(); i++) {
            wartość = pola.get(i).dodaj(drugi.getWartość(i));

            suma.ustaw(i, wartość);
        }

        return suma;
    }

    public Wektor<T> pomnóż(Wektor<T> drugi) throws RóżnaDługośćWektorów, NiepełnyWektor {
        checkRóżnaDługość(drugi);
        checkNiepełnyWektor(drugi);

        Wektor<T> iloczyn = new Wektor<>(pola.size());
        T wartość;

        for (int i = 0; i < pola.size(); i++) {
            wartość = pola.get(i).pomnóż(drugi.getWartość(i));

            iloczyn.ustaw(i, wartość);
        }

        return iloczyn;
    }

    public String toString() {
        StringBuilder ret = new StringBuilder();

        ret.append("{");

        for (int i = 0; i < pola.size() - 1; i++) {
            ret.append(pola.get(i).toString());
            ret.append(", ");
        }

        ret.append(pola.get(pola.size() - 1).toString());

        ret.append("}");

        return ret.toString();
    }
}
