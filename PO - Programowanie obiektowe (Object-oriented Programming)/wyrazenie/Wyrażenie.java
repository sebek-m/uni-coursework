public abstract class Wyrażenie {
    private float poleTrapezu(float a, float b) {
        float wysokość = b - a;
        float dół = policz(a);
        float góra = policz(b);

        return ((dół + góra) * wysokość) / 2;
    }

    public float całka(float a, float b) {
        float pole = 0;
        float odcinek = (b - a) / 1000;
        float dół = a;
        float góra = a + odcinek;

        for (int i = 0; i < 1000; i++) {
            pole += poleTrapezu(dół, góra);

            dół = góra;
            góra += odcinek;
        }

        return pole;
    }

    protected abstract float policz(float x);

    public abstract void zmieńNawiasy();
}