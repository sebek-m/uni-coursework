class Log extends Jednoargumentowe {
    public Log(Wyrażenie arg) {
        this.arg = arg;
    }

    protected float policz2(float x) {
        return (float) Math.log(x);
    }

    public String toString() {
        return "ln(" + arg.toString() + ")";
    }
}
