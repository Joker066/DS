#include <stdio.h>
#include "fraction.c"

int main() {
    int cmd, x, y, z, w;
    Frac a, b, c;
    scanf("%d", &cmd);
    scanf("%d %d %d %d", &x, &y, &z, &w);
    a = normal(x, y), b = normal(z, w);
    if (cmd == 1){
        c = add(a, b);
    }
    else if (cmd == 2){
        c = sub(a, b);
    }
    else if (cmd == 3){
        c = mul(a, b);
    }
    else if (cmd == 4){
        c = div(a, b);
    }
    printf("%lld / %lld\n", c.a, c.b);
    return 0;
}
