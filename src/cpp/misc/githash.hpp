#define STRINGFY_VAL(s) STRINGFY(s)
#define STRINGFY(s) #s

#ifndef G_HASH
#define G_HASH HASH_UNAVAILABLE
#endif

#define GIT_HASH STRINGFY_VAL(G_HASH)