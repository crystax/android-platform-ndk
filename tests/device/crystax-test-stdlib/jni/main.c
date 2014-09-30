/* test rand_r */

#include <stdio.h>
#include <stdlib.h>

#define RAND_VAL_0    520932930
#define RAND_VAL_1    28925691
#define RAND_VAL_2    822784415
#define RAND_VAL_3    822784415


static int test_rand_r(void);


int main()
{
    if (test_rand_r() != 0)
        return 1;

    return 0;
}


int test_rand_r(void)
{
    unsigned int v0 = 0, v0_1 = 0;
    unsigned int v1 = 0, v1_1 = 0;
    unsigned int v2 = 0, v2_1 = 0;
    unsigned int v3 = 0, v3_1 = 0;

    v1 = rand_r(&v0);
    v2 = rand_r(&v1);
    v3 = rand_r(&v2);

    /* printf("v0 = %i\n", v0); */
    /* printf("v1 = %i\n", v1); */
    /* printf("v2 = %i\n", v2); */
    /* printf("v3 = %i\n", v3); */

    v1_1 = rand_r(&v0_1);
    v2_1 = rand_r(&v1_1);
    v3_1 = rand_r(&v2_1);

    /* printf("v0_1 = %i\n", v0_1); */
    /* printf("v1_1 = %i\n", v1_1); */
    /* printf("v2_1 = %i\n", v2_1); */
    /* printf("v3_1 = %i\n", v3_1); */

    if ((v0 != v0_1) || (v1 != v1_1) || (v2 != v2_1) || (v3 != v3_1)) {
        printf("rand_r test failed\n");
        return 1;
    }

    return 0;
}
