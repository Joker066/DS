static long long gcd(long long a, long long b) {
		return b ? gcd(b, a % b) : a;
}

typedef struct Fraction Frac;
Frac normal(long long, long long);
Frac add(Frac, Frac);
Frac sub(Frac, Frac);
Frac mul(Frac, Frac);
Frac div(Frac, Frac);

struct Fraction {
		long long a, b;
};

Frac normal(long long a, long long b) {
		Frac c;
		long long d = gcd(a, b);
		c.a = a / d, c.b = b / d;
		return c;
}

Frac add(Frac x, Frac y) {
		Frac c = normal(x.a * y.b + x.b * y.a, x.b * y.b);
		return c;
}

Frac sub(Frac x, Frac y) {
		Frac c = normal(x.a * y.b - x.b * y.a, x.b * y.b);
		return c;
}

Frac mul(Frac x, Frac y) {
		Frac c = normal(x.a * y.a, x.b * y.b);
		return c;
}

Frac div(Frac x, Frac y) {
		Frac c = normal(x.a * y.b, x.b * y.a);
		return c;
}
