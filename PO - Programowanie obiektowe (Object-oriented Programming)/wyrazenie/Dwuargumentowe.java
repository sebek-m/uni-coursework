public abstract class Dwuargumentowe extends Wyrażenie {
    protected Wyrażenie arg1;
    protected Wyrażenie arg2;
    private boolean nawiasy = false;

    protected abstract char symbol();

    public float policz(float x) {
        float warArg1 = arg1.policz(x);
        float warArg2 = arg2.policz(x);

        return policz2(warArg1, warArg2);
    }

    protected abstract float policz2(float arg1, float arg2);

    public void zmieńNawiasy() {
        this.nawiasy = true;
    }

    public String toString() {
        arg1.zmieńNawiasy();
        arg2.zmieńNawiasy();

        String pierwszy = arg1.toString();
        String drugi = arg2.toString();

        arg1.zmieńNawiasy();
        arg2.zmieńNawiasy();

        if (this.nawiasy)
            return "(" + pierwszy + " " + symbol() + " " + drugi + ")";
        else
            return pierwszy + " " + symbol() + " " + drugi;
    }
}