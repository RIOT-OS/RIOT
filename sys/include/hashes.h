/******************************************************************************
 * djb2_hash
 * `````````
 * HISTORY
 * This algorithm (k=33) was first reported by Dan Bernstein many years
 * ago in comp.lang.c. Another version of this algorithm (now favored by
 * bernstein) uses XOR:
 *
 *      hash(i) = hash(i - 1) * 33 ^ str[i];
 *
 * The magic of number 33 (why it works better than many other constants,
 * prime or not) has never been adequately explained.
 *
 ******************************************************************************/
static inline unsigned long djb2_hash(const char *str)
{
    unsigned long hash;
    int c;

    hash = 5381;

    while ((c = (unsigned char) * str++)) {
        hash = ((hash << 5) + hash) + c;    /* hash * 33 + c */
    }

    return hash;
}

/******************************************************************************
 * sdbm_hash
 * `````````
 * HISTORY
 * This algorithm was created for sdbm (a public-domain reimplementation
 * of ndbm) database library. It was found to do well in scrambling bits,
 * causing better distribution of the keys and fewer splits. it also
 * happens to be a good general hashing function with good distribution.
 *
 * The actual function is
 *
 *      hash(i) = hash(i - 1) * 65599 + str[i];
 *
 * What is included below is the faster version used in gawk. [there is
 * even a faster, duff-device version] the magic constant 65599 was picked
 * out of thin air while experimenting with different constants, and turns
 * out to be a prime. this is one of the algorithms used in berkeley db
 * (see sleepycat) and elsewhere.
 *
 ******************************************************************************/
static inline unsigned long sdbm_hash(const char *str)
{
    unsigned long hash;
    int c;

    hash = 0;

    while ((c = (unsigned char) * str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

/******************************************************************************
 * lose lose
 * `````````
 * HISTORY
 * This hash function appeared in K&R (1st ed) but at least the reader
 * was warned:
 *
 *      "This is not the best possible algorithm, but it has the merit
 *      of extreme simplicity."
 *
 * This is an understatement. It is a terrible hashing algorithm, and it
 * could have been much better without sacrificing its "extreme simplicity."
 * [see the second edition!]
 *
 * Many C programmers use this function without actually testing it, or
 * checking something like Knuth's Sorting and Searching, so it stuck.
 * It is now found mixed with otherwise respectable code, eg. cnews. sigh.
 * [see also: tpop]
 *
 ******************************************************************************/
static inline unsigned long kr_hash(const char *str)
{
    unsigned int hash;
    unsigned int c;

    hash = 0;

    while ((c = (unsigned char) * str++)) {
        hash += c;
    }

    return hash;
}

/******************************************************************************
 * sax_hash
 * ````````
 * Shift, Add, XOR
 *
 ******************************************************************************/
static inline unsigned int sax_hash(const char *key)
{
    unsigned int h;

    h = 0;

    while (*key) {
        h ^= (h << 5) + (h >> 2) + (unsigned char) * key++;
    }

    return h;
}


/******************************************************************************
 * dek_hash
 * ````````
 * HISTORY
 * Proposed by Donald E. Knuth in The Art Of Computer Programming Vol. 3,
 * under the topic of "Sorting and Search", Chapter 6.4.
 *
 ******************************************************************************/
static inline unsigned int dek_hash(const char *str, unsigned int len)
{
    unsigned int hash;
    unsigned int c;

    hash = len;
    c = 0;

    while ((c = (unsigned int) * str++)) {
        hash = ((hash << 5) ^ (hash >> 27)) ^ (c);
    }

    return hash;
}


/******************************************************************************
 * fnv_hash
 * ````````
 * NOTE
 * For a more fully featured and modern version of this hash, see fnv32.c
 *
 ******************************************************************************/
static inline unsigned int fnv_hash(const char *str)
{
#define FNV_PRIME 0x811C9DC5
    unsigned int hash;
    unsigned int c;

    hash = 0;
    c    = 0;

    while ((c = (unsigned int) * str++)) {
        hash *= FNV_PRIME;
        hash ^= (c);
    }

    return hash;
}

