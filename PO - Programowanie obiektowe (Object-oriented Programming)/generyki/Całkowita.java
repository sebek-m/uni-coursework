public class Całkowita implements Algebraiczna<Całkowita> {
    private int wartość;

    public Całkowita(int wartość) {
        this.wartość = wartość;
    }

    public int getWartość() {
        return wartość;
    }

    @Override
    public Całkowita dodaj(Całkowita arg) {
        return new Całkowita(this.wartość + arg.getWartość());
    }

    @Override
    public Całkowita pomnóż(Całkowita arg) {
        return new Całkowita(this.wartość * arg.getWartość());
    }

    @Override
    public String toString() {
        return String.valueOf(wartość);
    }
}
