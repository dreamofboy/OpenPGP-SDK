#include "CUnit/Basic.h"

#include <openpgpsdk/random.h>
#include "openpgpsdk/std_print.h"
#include "openpgpsdk/packet-show.h"
 
#include "tests.h"

/* 
 * initialisation
 */

int init_suite_crypto(void)
    {
    // Return success
    return 0;
    }

int clean_suite_crypto(void)
    {
    reset_vars();

    return 0;
    }

static void test_ecb(ops_symmetric_algorithm_t alg)
    {
    // Used for trying low-level OpenSSL tests

    int verbose=0;

    ops_crypt_t crypt;
    unsigned char *iv=NULL;
    unsigned char *key=NULL;
    unsigned char *in=NULL;
    unsigned char *out=NULL;
    unsigned char *out2=NULL;

    /*
     * Initialise Crypt structure
     * Empty IV, made-up key
     */

    if (!ops_crypt_any(&crypt, alg))
        {
        fprintf(stderr,"Failed to initialise crypt struct: alg=%d (%s)\n",alg,ops_show_symmetric_algorithm(alg));
        CU_FAIL("Failed to initialise crypt struct");
        return;
        }

    iv=ops_mallocz(crypt.blocksize);
    key=ops_mallocz(crypt.keysize);
    snprintf((char *)key, crypt.keysize, "MY KEY");
    crypt.set_iv(&crypt, iv);
    crypt.set_key(&crypt, key);
    ops_encrypt_init(&crypt);

    /*
     * Create test buffers
     */
    in=ops_mallocz(crypt.blocksize);
    out=ops_mallocz(crypt.blocksize);
    out2=ops_mallocz(crypt.blocksize);

    snprintf((char *)in,crypt.blocksize,"hello");

    crypt.block_encrypt(&crypt, out, in);

    crypt.block_decrypt(&crypt, out2, out);
    CU_ASSERT(memcmp((char *)in, (char *)out2, strlen((char *)in))==0);

    if (verbose)
        {
        // plaintext
        printf("\n");
        printf("plaintext: 0x%.2x 0x%.2x 0x%.2x 0x%.2x   0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", 
               in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);
        printf("plaintext: %c    %c    %c    %c      %c    %c    %c    %c\n", 
               in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);

        // encrypted
        printf("encrypted: 0x%.2x 0x%.2x 0x%.2x 0x%.2x   0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", 
               out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7]);
        printf("encrypted: %c    %c    %c    %c      %c    %c    %c    %c\n", 
               out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7]);

        // decrypted
        printf("decrypted: 0x%.2x 0x%.2x 0x%.2x 0x%.2x   0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", 
               out2[0], out2[1], out2[2], out2[3], out2[4], out2[5], out2[6], out2[7]);
        printf("decrypted: %c    %c    %c    %c      %c    %c    %c    %c\n", 
               out2[0], out2[1], out2[2], out2[3], out2[4], out2[5], out2[6], out2[7]);
        }
    }

#ifndef OPENSSL_NO_IDEA
static void test_ecb_idea()
    {
    test_ecb(OPS_SA_IDEA);
    }
#endif

static void test_ecb_3des()
    {
    test_ecb(OPS_SA_TRIPLEDES);
    }

static void test_ecb_cast()
    {
    test_ecb(OPS_SA_CAST5);
    }

static void test_ecb_aes128()
    {
    test_ecb(OPS_SA_AES_128);
    }

static void test_ecb_aes256()
    {
    test_ecb(OPS_SA_AES_256);
    }

static void test_cfb(ops_symmetric_algorithm_t alg)
    {
    // Used for trying low-level OpenSSL tests

    int verbose=0;

    char *plaintext="This is a very long piece of text so that we can test that CFB mode works. It must be larger than the largest blocksize of the supported set of ciphers";
    const unsigned int sz_plaintext=strlen(plaintext+1);

    ops_crypt_t crypt;
    unsigned char *iv=NULL;
    unsigned char *key=NULL;

    unsigned char *out=NULL;
    unsigned char *out2=NULL;

    /*
     * Initialise Crypt structure
     * Empty IV, made-up key
     */

    if(!ops_crypt_any(&crypt, alg))
        {
        fprintf(stderr,"Failed to initialise crypt struct: alg=%d (%s)\n",alg,ops_show_symmetric_algorithm(alg));
        CU_FAIL("Failed to initialise crypt struct");
        return;
        }
    iv=ops_mallocz(crypt.blocksize);
    key=ops_mallocz(crypt.keysize);
    snprintf((char *)key, crypt.keysize, "MY CFB KEY");
    crypt.set_iv(&crypt, iv);
    crypt.set_key(&crypt, key);
    ops_encrypt_init(&crypt);

    /*
     * Create test buffers
     */
    out=ops_mallocz(sz_plaintext);
    out2=ops_mallocz(sz_plaintext);

    crypt.cfb_encrypt(&crypt, out, plaintext, sz_plaintext);

    /*
     * Reset IV and decrypt
     */

    crypt.set_iv(&crypt, iv);
    ops_decrypt_init(&crypt);
    crypt.cfb_decrypt(&crypt, out2, out, sz_plaintext);
    CU_ASSERT(memcmp(plaintext, (char *)out2, sz_plaintext)==0);

    if (verbose)
        {
        // plaintext
        printf("\n");
        printf("plaintext: 0x%.2x 0x%.2x 0x%.2x 0x%.2x   0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", 
               plaintext[0], plaintext[1], plaintext[2], plaintext[3], plaintext[4], plaintext[5], plaintext[6], plaintext[7]);
        printf("plaintext: %c    %c    %c    %c      %c    %c    %c    %c\n", 
               plaintext[0], plaintext[1], plaintext[2], plaintext[3], plaintext[4], plaintext[5], plaintext[6], plaintext[7]);

        // encrypted
        printf("encrypted: 0x%.2x 0x%.2x 0x%.2x 0x%.2x   0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", 
               out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7]);
        printf("encrypted: %c    %c    %c    %c      %c    %c    %c    %c\n", 
               out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7]);

        // decrypted
        printf("decrypted: 0x%.2x 0x%.2x 0x%.2x 0x%.2x   0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", 
               out2[0], out2[1], out2[2], out2[3], out2[4], out2[5], out2[6], out2[7]);
        printf("decrypted: %c    %c    %c    %c      %c    %c    %c    %c\n", 
               out2[0], out2[1], out2[2], out2[3], out2[4], out2[5], out2[6], out2[7]);
        }
    }

#ifdef LATER
#ifndef OPENSSL_NO_IDEA
static void test_cfb_idea()
    {
    test_cfb(OPS_SA_IDEA);
    }
#endif
#endif

static void test_cfb_3des()
    {
    test_cfb(OPS_SA_TRIPLEDES);
    }

static void test_cfb_cast()
    {
    test_cfb(OPS_SA_CAST5);
    }

static void test_cfb_aes128()
    {
    test_cfb(OPS_SA_AES_128);
    }

static void test_cfb_aes256()
    {
    test_cfb(OPS_SA_AES_256);
    }

#ifdef LATER
static void test_rsa()
    {
    unsigned char* in=NULL;
    unsigned char* encrypted=NULL;
    unsigned char* decrypted=NULL;
    const ops_key_data_t *pub_key=NULL;
    const ops_public_key_t *pkey=NULL;
    const ops_key_data_t *sec_key=NULL;
    const ops_secret_key_t *skey=NULL;

    in=ops_mallocz(128);
    encrypted=ops_mallocz(128);
    decrypted=ops_mallocz(128);

    ops_random(in,128);

    int n=0;
    pub_key=ops_keyring_find_key_by_userid(&pub_keyring, alpha_user_id);
    //    ops_print_public_key(pub_key);
    pkey=ops_get_public_key_from_data(pub_key);

    sec_key=ops_keyring_find_key_by_userid(&sec_keyring, alpha_user_id);
    //    ops_print_secret_key(sec_key);
    skey=ops_get_secret_key_from_data(sec_key);

    /*
    unsigned int i;
    fprintf(stderr,"in:        ");
    for (i=0; i<128; i++)
        fprintf(stderr,"%2x ", in[i]);
    fprintf(stderr,"\n");
    */

    n=ops_rsa_public_encrypt(&encrypted[0], (unsigned char *)in, 128, &pkey->key.rsa);
    CU_ASSERT(n!=-1);
    if (n==-1)
        return;

    /*
    fprintf(stderr,"%d encrypted\n",n);
    fprintf(stderr,"encrypted: ");
    for (i=0; i<128; i++)
        fprintf(stderr,"%2x ", encrypted[i]);
    fprintf(stderr,"\n");
    */
    n=ops_rsa_private_decrypt(&decrypted[0], encrypted, 128,
                            &skey->key.rsa, &pkey->key.rsa);
    CU_ASSERT(n!=-1);
    if (n==-1)
        return;

    /*
    fprintf(stderr,"%d decrypted\n",n);
    fprintf(stderr,"decrypted: ");
    for (i=0; i<128; i++)
        fprintf(stderr,"%2x ", decrypted[i]);
    fprintf(stderr,"\n");
    */
    CU_ASSERT(memcmp(in,&decrypted[0],128)==0);

    //    fprintf(stderr,"memcmp returns %d\n",memcmp(in,&decrypted[0],128));

    free(encrypted);
    free(decrypted);
    }
#endif

CU_pSuite suite_crypto()
{
    CU_pSuite suite = NULL;

    suite = CU_add_suite("Crypto Suite", init_suite_crypto, clean_suite_crypto);
    if (!suite)
	    return NULL;

    // add tests to suite
    
    /* ECB test */

#ifndef OPENSSL_NO_IDEA
    if (NULL == CU_add_test(suite, "Test ECB (IDEA)", test_ecb_idea))
	    return NULL;
#endif

    if (NULL == CU_add_test(suite, "Test ECB (TripleDES)", test_ecb_3des))
	    return NULL;

    if (NULL == CU_add_test(suite, "Test ECB (CAST)", test_ecb_cast))
	    return NULL;

    //    test_one_ecb(OPS_SA_BLOWFISH);

    if (NULL == CU_add_test(suite, "Test ECB (AES 128)", test_ecb_aes128))
	    return NULL;

    //    test_one_ecb(OPS_SA_AES_192);

    if (NULL == CU_add_test(suite, "Test ECB (AES 256)", test_ecb_aes256))
	    return NULL;

    //    test_one_cfb(OPS_SA_TWOFISH);

    /* CFB tests */

#ifdef LATER
#ifndef OPENSSL_NO_IDEA
    if (NULL == CU_add_test(suite, "Test CFB (IDEA)", test_cfb_idea))
	    return NULL;
#endif
#endif

    if (NULL == CU_add_test(suite, "Test CFB (TripleDES)", test_cfb_3des))
	    return NULL;

    if (NULL == CU_add_test(suite, "Test CFB (CAST)", test_cfb_cast))
	    return NULL;

    //    test_one_cfb(OPS_SA_BLOWFISH);

    if (NULL == CU_add_test(suite, "Test CFB (AES 128)", test_cfb_aes128))
	    return NULL;

    //    test_one_cfb(OPS_SA_AES_192);

    if (NULL == CU_add_test(suite, "Test CFB (AES 256)", test_cfb_aes256))
        return NULL;

    //    test_one_cfb(OPS_SA_TWOFISH);

    /*
     */
#ifdef LATER
    if (NULL == CU_add_test(suite, "Test RSA", test_rsa))
	    return NULL;
#endif

    return suite;
}

// EOF
