public class Potęga extends Jednoargumentowe {
    private float wykładnik;

    public Potęga(Wyrażenie arg, float wykładnik) {
        this.arg = arg;
        this.wykładnik = wykładnik;
    }

    protected float policz2(float x) {
        return (float) Math.pow(x, wykładnik);
    }

    public String toString() {
        arg.zmieńNawiasy();

        String argString = arg.toString();

        arg.zmieńNawiasy();

        return argString + "^" + wykładnik;
    }
}
