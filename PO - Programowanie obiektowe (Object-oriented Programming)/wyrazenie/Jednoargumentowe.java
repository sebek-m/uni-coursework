public abstract class Jednoargumentowe extends Wyrażenie {
    protected Wyrażenie arg;

    public float policz(float x) {
        float warArg = arg.policz(x);
        return policz2(warArg);
    }

    protected abstract float policz2(float x);

    public void zmieńNawiasy() {
    }
}
