# ds — simple data structures in C

This repo is set up to be used **without building a library**.

## How to use

1. Copy this repo into your project (e.g., `vendor/ds/`) **or** place it at the project root.
2. Include the umbrella header in your code:

   ```c
   #include "vendor/ds/include/ds.h"   // if placed in vendor/ds/
   // or
   #include "include/ds.h"             // if placed at project root
   ```

3. Compile your app **together with** the unity file `ds_all.c`, and add the header path.

---

### GCC / Clang

**If ds is at project root:**
```bash
cc -std=c11 -O2 -Iinclude ds_all.c your_app.c -o your_app
```

**If ds is vendored under `vendor/ds/`:**
```bash
cc -std=c11 -O2 -Ivendor/ds/include vendor/ds/ds_all.c your_app.c -o your_app
```

---

### MSVC (Windows)

**If ds is vendored under `vendor\ds\`:**
```bat
cl /std:c17 /O2 /I vendor\ds\include vendor\ds\ds_all.c your_app.c /Fe:your_app.exe
```

---

## Important

- **Compile exactly one** unity file: `ds_all.c`.  
  Do **not** also compile individual files in `src/` (duplicate symbols).
- Keep the include path correct (`-Iinclude` or `-Ivendor/ds/include`).
- APIs return status codes (e.g., `QUEUE_OK`, `STACK_ERR_EMPTY`). Only `*_debug_*` helpers print.

---

## Example

```c
// main.c
#include "include/ds.h"   // or "vendor/ds/include/ds.h"
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
```

**Build (root layout):**
```bash
cc -std=c11 -O2 -Iinclude ds_all.c main.c -o demo
./demo
```

---

## Using only some structures

Edit `ds_all.c` to include only what you need:
```c
#include "src/queue.c"
#include "src/stack.c"
#include "src/rb_tree.c"
```
