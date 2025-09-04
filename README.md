# ds — simple data structures in C

This repo is set up to be used **without building a library**.

## How to use

1. Copy this repo into your project (e.g., `vendor/ds/`), or keep it at the root.
2. In your code, include the umbrella header:
   ```c
   #include "vendor/ds/include/ds.h"   // adjust path if different
Compile your app + the unity file ds_all.c, and add the header path.

## GCC / Clang
If ds is at project root:

```bash
Copy code
cc -std=c11 -O2 -Iinclude ds_all.c your_app.c -o your_app
If ds is vendored under vendor/ds/:

bash
Copy code
cc -std=c11 -O2 -Ivendor/ds/include vendor/ds/ds_all.c your_app.c -o your_app
MSVC (Windows)
If ds is vendored under vendor\ds\:

bat
Copy code
cl /std:c17 /O2 /I vendor\ds\include vendor\ds\ds_all.c your_app.c /Fe:your_app.exe
Important
Compile exactly one unity file: ds_all.c.
Do not also compile individual files in src/ (that will cause duplicate symbols).

Keep the include path correct (use -Iinclude or -Ivendor/ds/include).

All modules return status codes (e.g., QUEUE_OK, STACK_ERR_EMPTY). Debug functions are the only ones that print.

Minimal example
c
Copy code
// main.c
#include "vendor/ds/include/ds.h"
#include <stdio.h>

int main(void) {
    queue *q = queue_new();
    queue_push(q, 42);

    int x;
    if (queue_pop(q, &x) == QUEUE_OK) {
        printf("%d\n", x);
    }
    queue_free(q);
    return 0;
}
Build (vendored):

bash
Copy code
cc -std=c11 -O2 -Ivendor/ds/include vendor/ds/ds_all.c main.c -o demo
./demo
Using only some structures
Edit ds_all.c to include only what you need, e.g.:

c
Copy code
#include "src/queue.c"
#include "src/stack.c"
#include "src/rb_tree.c"