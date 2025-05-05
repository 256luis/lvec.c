# lvec.c
lvec.c is a simple  generic dynamic array implementation.

NOTE: Use of this library requires `typeof` support.

## Basic usage:

Include the `lvec.h` header file and use the `lvec_`* functions.

```c
// include ...
#include "lvec.h"

int main()
{
	int* nums = lvec_new(int);

    lvec_append(nums, 10);
    lvec_append(nums, 20);

    for (size_t i = 0; i < lvec_get_length(nums); i++)
    {
        printf("%d", nums[i]);
    }

	free(nums);
}
```

Make sure to use the *_aggregate functions when appending and inserting to an `lvec` of structs or unions.

```c
typedef struct Vector2
{
    int x;
    int y;
} Vector2;

int main()
{
    Vector2* vecs = lvec_new(Vector2);

    Vector2 vec1 = { 0, 1 };
    lvec_append_aggregate(vecs, vec1);

    Vector2 vec2 = { 3, 6 };
    lvec_append_aggregate(vecs, vec2);

    free(nums);
}
```

You can also check the return value for success or failure.

```c
int main()
{
    int* nums = lvec_new(int);

    LVecResult result = lvec_append(nums, 20);
    if (result != LVECRESULT_SUCCESS)
    {
        // handle error
    }

    free(nums);
}
```

## Building from source
Make a build directory (or not) then call CMake at the project root.

```bash
$ mkdir build
$ cd build
$ cmake ..
```

This generates the appropriate build system files based on your platform and generator (e.g., Makefiles, Ninja files, Visual Studio solutions, etc.).

Once the build completes, the resulting static library will be placed in the `lib` directory.