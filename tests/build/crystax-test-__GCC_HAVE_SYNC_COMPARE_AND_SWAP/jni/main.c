#if __arm__ && __ARM_ARCH_5TE__
/* armv5 don't define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_N macros */
#else /* !__arm__ || !__ARM_ARCH_5TE__ */

#if !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1)
#error __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 not defined
#endif

#if !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2)
#error __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 not defined
#endif

#if !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
#error __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 not defined
#endif

#if !(__mips__ && !__LP64__)

#if !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
#error __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8 not defined
#endif

#endif /* !(__mips__ && !__LP64__) */

#endif /* !__arm__ || !__ARM_ARCH_5TE__ */

int main()
{
    return 0;
}
