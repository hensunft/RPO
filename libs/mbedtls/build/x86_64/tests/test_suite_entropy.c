#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : .\test_suite_entropy.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/main_test.function
 *      Platform code file  : C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/host_test.function
 *      Helper file         : C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/helpers.function
 *      Test suite file     : C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function
 *      Test suite data     : C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.data
 *
 */

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L // for fileno() from <stdio.h>
#endif
#endif

#include "mbedtls/build_info.h"

/* Test code may use deprecated identifiers only if the preprocessor symbol
 * MBEDTLS_TEST_DEPRECATED is defined. When building tests, set
 * MBEDTLS_TEST_DEPRECATED explicitly if MBEDTLS_DEPRECATED_WARNING is
 * enabled but the corresponding warnings are not treated as errors.
 */
#if !defined(MBEDTLS_DEPRECATED_REMOVED) && !defined(MBEDTLS_DEPRECATED_WARNING)
#define MBEDTLS_TEST_DEPRECATED
#endif

/*----------------------------------------------------------------------------*/
/* Common helper code */

#line 2 "suites/helpers.function"
/*----------------------------------------------------------------------------*/
/* Headers */

#include <test/arguments.h>
#include <test/helpers.h>
#include <test/macros.h>
#include <test/random.h>
#include <test/bignum_helpers.h>
#include <test/psa_crypto_helpers.h>
#include <test/threading_helpers.h>

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(MBEDTLS_ERROR_C)
#include "mbedtls/error.h"
#endif
#include "mbedtls/platform.h"

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

/*----------------------------------------------------------------------------*/
/* Status and error constants */

#define DEPENDENCY_SUPPORTED            0   /* Dependency supported by build */
#define KEY_VALUE_MAPPING_FOUND         0   /* Integer expression found */
#define DISPATCH_TEST_SUCCESS           0   /* Test dispatch successful */

#define KEY_VALUE_MAPPING_NOT_FOUND     -1  /* Integer expression not found */
#define DEPENDENCY_NOT_SUPPORTED        -2  /* Dependency not supported */
#define DISPATCH_TEST_FN_NOT_FOUND      -3  /* Test function not found */
#define DISPATCH_INVALID_TEST_DATA      -4  /* Invalid test parameter type.
                                               Only int, string, binary data
                                               and integer expressions are
                                               allowed */
#define DISPATCH_UNSUPPORTED_SUITE      -5  /* Test suite not supported by the
                                               build */

/*----------------------------------------------------------------------------*/
/* Global variables */

/*----------------------------------------------------------------------------*/
/* Helper flags for complex dependencies */

/* Indicates whether we expect mbedtls_entropy_init
 * to initialize some strong entropy source. */
#if !defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES) && \
    (!defined(MBEDTLS_NO_PLATFORM_ENTROPY) ||       \
    defined(MBEDTLS_ENTROPY_HARDWARE_ALT) ||        \
    defined(ENTROPY_NV_SEED))
#define ENTROPY_HAVE_STRONG
#endif


/*----------------------------------------------------------------------------*/
/* Helper Functions */

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
static int redirect_output(FILE *out_stream, const char *path)
{
    int out_fd, dup_fd;
    FILE *path_stream;

    out_fd = fileno(out_stream);
    dup_fd = dup(out_fd);

    if (dup_fd == -1) {
        return -1;
    }

    path_stream = fopen(path, "w");
    if (path_stream == NULL) {
        close(dup_fd);
        return -1;
    }

    fflush(out_stream);
    if (dup2(fileno(path_stream), out_fd) == -1) {
        close(dup_fd);
        fclose(path_stream);
        return -1;
    }

    fclose(path_stream);
    return dup_fd;
}

static int restore_output(FILE *out_stream, int dup_fd)
{
    int out_fd = fileno(out_stream);

    fflush(out_stream);
    if (dup2(dup_fd, out_fd) == -1) {
        close(out_fd);
        close(dup_fd);
        return -1;
    }

    close(dup_fd);
    return 0;
}
#endif /* __unix__ || __APPLE__ __MACH__ */


#line 43 "suites/main_test.function"


/*----------------------------------------------------------------------------*/
/* Test Suite Code */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ENTROPY_C)
#if !defined(MBEDTLS_PSA_INJECT_ENTROPY)
#line 2 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
#include "mbedtls/entropy.h"
#include "entropy_poll.h"
#include "mbedtls/md.h"
#include "string.h"

typedef enum {
    DUMMY_CONSTANT_LENGTH, /* Output context->length bytes */
    DUMMY_REQUESTED_LENGTH, /* Output whatever length was requested */
    DUMMY_FAIL, /* Return an error code */
} entropy_dummy_instruction;

typedef struct {
    entropy_dummy_instruction instruction;
    size_t length; /* Length to return for DUMMY_CONSTANT_LENGTH */
    size_t calls; /* Incremented at each call */
} entropy_dummy_context;

/*
 * Dummy entropy source
 *
 * If data is NULL, write exactly the requested length.
 * Otherwise, write the length indicated by data or error if negative
 */
static int entropy_dummy_source(void *arg, unsigned char *output,
                                size_t len, size_t *olen)
{
    entropy_dummy_context *context = arg;
    ++context->calls;

    switch (context->instruction) {
        case DUMMY_CONSTANT_LENGTH:
            *olen = context->length;
            break;
        case DUMMY_REQUESTED_LENGTH:
            *olen = len;
            break;
        case DUMMY_FAIL:
            return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    memset(output, 0x2a, *olen);
    return 0;
}

/*
 * Ability to clear entropy sources to allow testing with just predefined
 * entropy sources. This function or tests depending on it might break if there
 * are internal changes to how entropy sources are registered.
 *
 * To be called immediately after mbedtls_entropy_init().
 *
 * Just resetting the counter. New sources will overwrite existing ones.
 * This might break memory checks in the future if sources need 'free-ing' then
 * as well.
 */
static void entropy_clear_sources(mbedtls_entropy_context *ctx)
{
    ctx->source_count = 0;
}

#if defined(MBEDTLS_ENTROPY_NV_SEED)

#if defined(MBEDTLS_MD_LIGHT) && defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
/*
 * NV seed read/write functions that use a buffer instead of a file
 */
static unsigned char buffer_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];

static int buffer_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    if (buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE) {
        return -1;
    }

    memcpy(buf, buffer_seed, MBEDTLS_ENTROPY_BLOCK_SIZE);
    return 0;
}

static int buffer_nv_seed_write(unsigned char *buf, size_t buf_len)
{
    if (buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE) {
        return -1;
    }

    memcpy(buffer_seed, buf, MBEDTLS_ENTROPY_BLOCK_SIZE);
    return 0;
}
#endif /* MBEDTLS_MD_LIGHT && MBEDTLS_PLATFORM_NV_SEED_ALT */

#if defined(MBEDTLS_FS_IO)
/*
 * NV seed read/write helpers that fill the base seedfile
 */
static int write_nv_seed(unsigned char *buf, size_t buf_len)
{
    FILE *f;

    if (buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE) {
        return -1;
    }

    if ((f = fopen(MBEDTLS_PLATFORM_STD_NV_SEED_FILE, "w")) == NULL) {
        return -1;
    }

    if (fwrite(buf, 1, MBEDTLS_ENTROPY_BLOCK_SIZE, f) !=
        MBEDTLS_ENTROPY_BLOCK_SIZE) {
        fclose(f);
        return -1;
    }

    fclose(f);

    return 0;
}

#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
static int read_nv_seed(unsigned char *buf, size_t buf_len)
{
    FILE *f;

    if (buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE) {
        return -1;
    }

    if ((f = fopen(MBEDTLS_PLATFORM_STD_NV_SEED_FILE, "rb")) == NULL) {
        return -1;
    }

    if (fread(buf, 1, MBEDTLS_ENTROPY_BLOCK_SIZE, f) !=
        MBEDTLS_ENTROPY_BLOCK_SIZE) {
        fclose(f);
        return -1;
    }

    fclose(f);

    return 0;
}
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#line 152 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_init_free(int reinit)
{
    mbedtls_entropy_context ctx;

    /* Double free is not explicitly documented to work, but it is convenient
     * to call mbedtls_entropy_free() unconditionally on an error path without
     * checking whether it has already been called in the success path. */

    mbedtls_entropy_init(&ctx);
    mbedtls_entropy_free(&ctx);

    if (reinit) {
        mbedtls_entropy_init(&ctx);
    }
    mbedtls_entropy_free(&ctx);

    /* This test case always succeeds, functionally speaking. A plausible
     * bug might trigger an invalid pointer dereference or a memory leak. */
    goto exit;
exit:
    ;
}

static void test_entropy_init_free_wrapper( void ** params )
{

    test_entropy_init_free( ((mbedtls_test_argument_t *) params[0])->sint );
}
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
#line 175 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_seed_file(char *path, int ret)
{
    mbedtls_entropy_context ctx;
    mbedtls_entropy_init(&ctx);

    MD_PSA_INIT();

    TEST_ASSERT(mbedtls_entropy_write_seed_file(&ctx, path) == ret);
    TEST_ASSERT(mbedtls_entropy_update_seed_file(&ctx, path) == ret);

exit:
    mbedtls_entropy_free(&ctx);
    MD_PSA_DONE();
}

static void test_entropy_seed_file_wrapper( void ** params )
{

    test_entropy_seed_file( (char *) params[0], ((mbedtls_test_argument_t *) params[1])->sint );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
#line 192 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_write_base_seed_file(int ret)
{
    mbedtls_entropy_context ctx;
    mbedtls_entropy_init(&ctx);

    MD_PSA_INIT();

    TEST_ASSERT(mbedtls_entropy_write_seed_file(&ctx, MBEDTLS_PLATFORM_STD_NV_SEED_FILE) == ret);
    TEST_ASSERT(mbedtls_entropy_update_seed_file(&ctx, MBEDTLS_PLATFORM_STD_NV_SEED_FILE) == ret);

exit:
    mbedtls_entropy_free(&ctx);
    MD_PSA_DONE();
}

static void test_entropy_write_base_seed_file_wrapper( void ** params )
{

    test_entropy_write_base_seed_file( ((mbedtls_test_argument_t *) params[0])->sint );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#line 209 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_no_sources(void)
{
    mbedtls_entropy_context ctx;
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];

    mbedtls_entropy_init(&ctx);
    entropy_clear_sources(&ctx);
    TEST_EQUAL(mbedtls_entropy_func(&ctx, buf, sizeof(buf)),
               MBEDTLS_ERR_ENTROPY_NO_SOURCES_DEFINED);

exit:
    mbedtls_entropy_free(&ctx);
}

static void test_entropy_no_sources_wrapper( void ** params )
{
    (void)params;

    test_entropy_no_sources(  );
}
#line 225 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_too_many_sources(void)
{
    mbedtls_entropy_context ctx;
    size_t i;
    entropy_dummy_context dummy = { DUMMY_REQUESTED_LENGTH, 0, 0 };

    mbedtls_entropy_init(&ctx);

    /*
     * It's hard to tell precisely when the error will occur,
     * since we don't know how many sources were automatically added.
     */
    for (i = 0; i < MBEDTLS_ENTROPY_MAX_SOURCES; i++) {
        (void) mbedtls_entropy_add_source(&ctx, entropy_dummy_source, &dummy,
                                          16, MBEDTLS_ENTROPY_SOURCE_WEAK);
    }

    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, entropy_dummy_source, &dummy,
                                           16, MBEDTLS_ENTROPY_SOURCE_WEAK)
                == MBEDTLS_ERR_ENTROPY_MAX_SOURCES);

exit:
    mbedtls_entropy_free(&ctx);
}

static void test_entropy_too_many_sources_wrapper( void ** params )
{
    (void)params;

    test_entropy_too_many_sources(  );
}
#if defined(ENTROPY_HAVE_STRONG)
#line 252 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_func_len(int len, int ret)
{
    mbedtls_entropy_context ctx;
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE + 10] = { 0 };
    unsigned char acc[MBEDTLS_ENTROPY_BLOCK_SIZE + 10] = { 0 };
    size_t i, j;

    mbedtls_entropy_init(&ctx);

    MD_PSA_INIT();

    /*
     * See comments in mbedtls_entropy_self_test()
     */
    for (i = 0; i < 8; i++) {
        TEST_ASSERT(mbedtls_entropy_func(&ctx, buf, len) == ret);
        for (j = 0; j < sizeof(buf); j++) {
            acc[j] |= buf[j];
        }
    }

    if (ret == 0) {
        for (j = 0; j < (size_t) len; j++) {
            TEST_ASSERT(acc[j] != 0);
        }
    }

    for (j = len; j < sizeof(buf); j++) {
        TEST_ASSERT(acc[j] == 0);
    }

exit:
    mbedtls_entropy_free(&ctx);
    MD_PSA_DONE();
}

static void test_entropy_func_len_wrapper( void ** params )
{

    test_entropy_func_len( ((mbedtls_test_argument_t *) params[0])->sint, ((mbedtls_test_argument_t *) params[1])->sint );
}
#endif /* ENTROPY_HAVE_STRONG */
#line 290 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_source_fail(char *path)
{
    mbedtls_entropy_context ctx;
    unsigned char buf[16];
    entropy_dummy_context dummy = { DUMMY_FAIL, 0, 0 };

    mbedtls_entropy_init(&ctx);

    MD_PSA_INIT();

    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, entropy_dummy_source,
                                           &dummy, 16,
                                           MBEDTLS_ENTROPY_SOURCE_WEAK)
                == 0);

    TEST_ASSERT(mbedtls_entropy_func(&ctx, buf, sizeof(buf))
                == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED);
    TEST_ASSERT(mbedtls_entropy_gather(&ctx)
                == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED);
#if defined(MBEDTLS_FS_IO) && defined(MBEDTLS_ENTROPY_NV_SEED)
    TEST_ASSERT(mbedtls_entropy_write_seed_file(&ctx, path)
                == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED);
    TEST_ASSERT(mbedtls_entropy_update_seed_file(&ctx, path)
                == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED);
#else
    ((void) path);
#endif

exit:
    mbedtls_entropy_free(&ctx);
    MD_PSA_DONE();
}

static void test_entropy_source_fail_wrapper( void ** params )
{

    test_entropy_source_fail( (char *) params[0] );
}
#line 325 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_threshold(int threshold, int chunk_size, int result)
{
    mbedtls_entropy_context ctx;
    entropy_dummy_context strong =
    { DUMMY_CONSTANT_LENGTH, MBEDTLS_ENTROPY_BLOCK_SIZE, 0 };
    entropy_dummy_context weak = { DUMMY_CONSTANT_LENGTH, chunk_size, 0 };
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE] = { 0 };
    int ret;

    mbedtls_entropy_init(&ctx);
    entropy_clear_sources(&ctx);

    MD_PSA_INIT();

    /* Set strong source that reaches its threshold immediately and
     * a weak source whose threshold is a test parameter. */
    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, entropy_dummy_source,
                                           &strong, 1,
                                           MBEDTLS_ENTROPY_SOURCE_STRONG) == 0);
    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, entropy_dummy_source,
                                           &weak, threshold,
                                           MBEDTLS_ENTROPY_SOURCE_WEAK) == 0);

    ret = mbedtls_entropy_func(&ctx, buf, sizeof(buf));

    if (result >= 0) {
        TEST_ASSERT(ret == 0);
#if defined(MBEDTLS_ENTROPY_NV_SEED)
        /* If the NV seed functionality is enabled, there are two entropy
         * updates: before and after updating the NV seed. */
        result *= 2;
#endif
        TEST_ASSERT(weak.calls == (size_t) result);
    } else {
        TEST_ASSERT(ret == result);
    }

exit:
    mbedtls_entropy_free(&ctx);
    MD_PSA_DONE();
}

static void test_entropy_threshold_wrapper( void ** params )
{

    test_entropy_threshold( ((mbedtls_test_argument_t *) params[0])->sint, ((mbedtls_test_argument_t *) params[1])->sint, ((mbedtls_test_argument_t *) params[2])->sint );
}
#line 369 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_calls(int strength1, int strength2,
                   int threshold, int chunk_size,
                   int result)
{
    /*
     * if result >= 0: result = expected number of calls to source 1
     * if result < 0: result = expected return code from mbedtls_entropy_func()
     */

    mbedtls_entropy_context ctx;
    entropy_dummy_context dummy1 = { DUMMY_CONSTANT_LENGTH, chunk_size, 0 };
    entropy_dummy_context dummy2 = { DUMMY_CONSTANT_LENGTH, chunk_size, 0 };
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE] = { 0 };
    int ret;

    mbedtls_entropy_init(&ctx);
    entropy_clear_sources(&ctx);

    MD_PSA_INIT();

    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, entropy_dummy_source,
                                           &dummy1, threshold,
                                           strength1) == 0);
    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, entropy_dummy_source,
                                           &dummy2, threshold,
                                           strength2) == 0);

    ret = mbedtls_entropy_func(&ctx, buf, sizeof(buf));

    if (result >= 0) {
        TEST_ASSERT(ret == 0);
#if defined(MBEDTLS_ENTROPY_NV_SEED)
        /* If the NV seed functionality is enabled, there are two entropy
         * updates: before and after updating the NV seed. */
        result *= 2;
#endif
        TEST_ASSERT(dummy1.calls == (size_t) result);
    } else {
        TEST_ASSERT(ret == result);
    }

exit:
    mbedtls_entropy_free(&ctx);
    MD_PSA_DONE();
}

static void test_entropy_calls_wrapper( void ** params )
{

    test_entropy_calls( ((mbedtls_test_argument_t *) params[0])->sint, ((mbedtls_test_argument_t *) params[1])->sint, ((mbedtls_test_argument_t *) params[2])->sint, ((mbedtls_test_argument_t *) params[3])->sint, ((mbedtls_test_argument_t *) params[4])->sint );
}
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
#line 417 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_nv_seed_file_create(void)
{
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];

    memset(buf, 0, MBEDTLS_ENTROPY_BLOCK_SIZE);

    TEST_ASSERT(write_nv_seed(buf, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);
exit:
    ;
}

static void test_nv_seed_file_create_wrapper( void ** params )
{
    (void)params;

    test_nv_seed_file_create(  );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
#line 428 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_nv_seed_std_io(void)
{
    unsigned char io_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char check_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];

    memset(io_seed, 1, MBEDTLS_ENTROPY_BLOCK_SIZE);
    memset(check_seed, 0, MBEDTLS_ENTROPY_BLOCK_SIZE);

    mbedtls_platform_set_nv_seed(mbedtls_platform_std_nv_seed_read,
                                 mbedtls_platform_std_nv_seed_write);

    /* Check if platform NV read and write manipulate the same data */
    TEST_ASSERT(write_nv_seed(io_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);
    TEST_ASSERT(mbedtls_nv_seed_read(check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) ==
                MBEDTLS_ENTROPY_BLOCK_SIZE);

    TEST_ASSERT(memcmp(io_seed, check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);

    memset(check_seed, 0, MBEDTLS_ENTROPY_BLOCK_SIZE);

    /* Check if platform NV write and raw read manipulate the same data */
    TEST_ASSERT(mbedtls_nv_seed_write(io_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) ==
                MBEDTLS_ENTROPY_BLOCK_SIZE);
    TEST_ASSERT(read_nv_seed(check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);

    TEST_ASSERT(memcmp(io_seed, check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);
exit:
    ;
}

static void test_entropy_nv_seed_std_io_wrapper( void ** params )
{
    (void)params;

    test_entropy_nv_seed_std_io(  );
}
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#if defined(MBEDTLS_MD_LIGHT)
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
#line 458 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_nv_seed(data_t *read_seed)
{
#if defined(MBEDTLS_ENTROPY_SHA512_ACCUMULATOR)
    const mbedtls_md_info_t *md_info =
        mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);
#elif defined(MBEDTLS_ENTROPY_SHA256_ACCUMULATOR)
    const mbedtls_md_info_t *md_info =
        mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
#else
#error "Unsupported entropy accumulator"
#endif
    mbedtls_md_context_t accumulator;
    mbedtls_entropy_context ctx;
    int (*original_mbedtls_nv_seed_read)(unsigned char *buf, size_t buf_len) =
        mbedtls_nv_seed_read;
    int (*original_mbedtls_nv_seed_write)(unsigned char *buf, size_t buf_len) =
        mbedtls_nv_seed_write;

    unsigned char header[2];
    unsigned char entropy[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char empty[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char check_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char check_entropy[MBEDTLS_ENTROPY_BLOCK_SIZE];

    memset(entropy, 0, MBEDTLS_ENTROPY_BLOCK_SIZE);
    memset(buf, 0, MBEDTLS_ENTROPY_BLOCK_SIZE);
    memset(empty, 0, MBEDTLS_ENTROPY_BLOCK_SIZE);
    memset(check_seed, 2, MBEDTLS_ENTROPY_BLOCK_SIZE);
    memset(check_entropy, 3, MBEDTLS_ENTROPY_BLOCK_SIZE);

    // Make sure we read/write NV seed from our buffers
    mbedtls_platform_set_nv_seed(buffer_nv_seed_read, buffer_nv_seed_write);

    mbedtls_md_init(&accumulator);
    mbedtls_entropy_init(&ctx);
    entropy_clear_sources(&ctx);

    MD_PSA_INIT();

    TEST_ASSERT(mbedtls_entropy_add_source(&ctx, mbedtls_nv_seed_poll, NULL,
                                           MBEDTLS_ENTROPY_BLOCK_SIZE,
                                           MBEDTLS_ENTROPY_SOURCE_STRONG) == 0);

    // Set the initial NV seed to read
    TEST_ASSERT(read_seed->len >= MBEDTLS_ENTROPY_BLOCK_SIZE);
    memcpy(buffer_seed, read_seed->x, MBEDTLS_ENTROPY_BLOCK_SIZE);

    // Do an entropy run
    TEST_ASSERT(mbedtls_entropy_func(&ctx, entropy, sizeof(entropy)) == 0);
    // Determine what should have happened with manual entropy internal logic

    // Init accumulator
    header[1] = MBEDTLS_ENTROPY_BLOCK_SIZE;
    TEST_ASSERT(mbedtls_md_setup(&accumulator, md_info, 0) == 0);

    // First run for updating write_seed
    header[0] = 0;
    TEST_ASSERT(mbedtls_md_starts(&accumulator) == 0);
    TEST_ASSERT(mbedtls_md_update(&accumulator, header, 2) == 0);
    TEST_ASSERT(mbedtls_md_update(&accumulator,
                                  read_seed->x, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);
    TEST_ASSERT(mbedtls_md_finish(&accumulator, buf) == 0);

    TEST_ASSERT(mbedtls_md_starts(&accumulator) == 0);
    TEST_ASSERT(mbedtls_md_update(&accumulator,
                                  buf, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);

    TEST_ASSERT(mbedtls_md(md_info, buf, MBEDTLS_ENTROPY_BLOCK_SIZE,
                           check_seed) == 0);

    // Second run for actual entropy (triggers mbedtls_entropy_update_nv_seed)
    header[0] = MBEDTLS_ENTROPY_SOURCE_MANUAL;
    TEST_ASSERT(mbedtls_md_update(&accumulator, header, 2) == 0);
    TEST_ASSERT(mbedtls_md_update(&accumulator,
                                  empty, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);

    header[0] = 0;
    TEST_ASSERT(mbedtls_md_update(&accumulator, header, 2) == 0);
    TEST_ASSERT(mbedtls_md_update(&accumulator,
                                  check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);
    TEST_ASSERT(mbedtls_md_finish(&accumulator, buf) == 0);

    TEST_ASSERT(mbedtls_md(md_info, buf, MBEDTLS_ENTROPY_BLOCK_SIZE,
                           check_entropy) == 0);

    // Check result of both NV file and entropy received with the manual calculations
    TEST_ASSERT(memcmp(check_seed, buffer_seed, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);
    TEST_ASSERT(memcmp(check_entropy, entropy, MBEDTLS_ENTROPY_BLOCK_SIZE) == 0);

exit:
    mbedtls_md_free(&accumulator);
    mbedtls_entropy_free(&ctx);
    mbedtls_nv_seed_read = original_mbedtls_nv_seed_read;
    mbedtls_nv_seed_write = original_mbedtls_nv_seed_write;
    MD_PSA_DONE();
}

static void test_entropy_nv_seed_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], ((mbedtls_test_argument_t *) params[1])->len};

    test_entropy_nv_seed( &data0 );
}
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#endif /* MBEDTLS_MD_LIGHT */
#if defined(ENTROPY_HAVE_STRONG)
#if defined(MBEDTLS_SELF_TEST)
#line 558 "C:/Users/User/Desktop/rpo/libs/mbedtls/mbedtls/tests/suites/test_suite_entropy.function"
static void test_entropy_selftest(int result)
{
    MD_PSA_INIT();

    TEST_ASSERT(mbedtls_entropy_self_test(1) == result);

exit:
    MD_PSA_DONE();
}

static void test_entropy_selftest_wrapper( void ** params )
{

    test_entropy_selftest( ((mbedtls_test_argument_t *) params[0])->sint );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* ENTROPY_HAVE_STRONG */
#endif /* !MBEDTLS_PSA_INJECT_ENTROPY */
#endif /* MBEDTLS_ENTROPY_C */


#line 54 "suites/main_test.function"


/*----------------------------------------------------------------------------*/
/* Test dispatch code */


/**
 * \brief       Evaluates an expression/macro into its literal integer value.
 *              For optimizing space for embedded targets each expression/macro
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and evaluation code is generated by script:
 *              generate_test_code.py
 *
 * \param exp_id    Expression identifier.
 * \param out_value Pointer to int to hold the integer.
 *
 * \return       0 if exp_id is found. 1 otherwise.
 */
static int get_expression(int32_t exp_id, intmax_t *out_value)
{
    int ret = KEY_VALUE_MAPPING_FOUND;

    (void) exp_id;
    (void) out_value;

    switch (exp_id) {
    
#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)

        case 0:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_FILE_IO_ERROR;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ENTROPY_SOURCE_WEAK;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ENTROPY_BLOCK_SIZE;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_NO_STRONG_SOURCE;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ENTROPY_SOURCE_STRONG;
            }
            break;
        case 6:
            {
                *out_value = (MBEDTLS_ENTROPY_BLOCK_SIZE+1)/2;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ENTROPY_BLOCK_SIZE+1;
            }
            break;
#endif

#line 82 "suites/main_test.function"
        default:
        {
            ret = KEY_VALUE_MAPPING_NOT_FOUND;
        }
        break;
    }
    return ret;
}


/**
 * \brief       Checks if the dependency i.e. the compile flag is set.
 *              For optimizing space for embedded targets each dependency
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and check code is generated by script:
 *              generate_test_code.py
 *
 * \param dep_id    Dependency identifier.
 *
 * \return       DEPENDENCY_SUPPORTED if set else DEPENDENCY_NOT_SUPPORTED
 */
static int dep_check(int dep_id)
{
    int ret = DEPENDENCY_NOT_SUPPORTED;

    (void) dep_id;

    switch (dep_id) {
    
#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)

#endif

#line 112 "suites/main_test.function"
        default:
            break;
    }
    return ret;
}


/**
 * \brief       Function pointer type for test function wrappers.
 *
 * A test function wrapper decodes the parameters and passes them to the
 * underlying test function. Both the wrapper and the underlying function
 * return void. Test wrappers assume that they are passed a suitable
 * parameter array and do not perform any error detection.
 *
 * \param param_array   The array of parameters. Each element is a `void *`
 *                      which the wrapper casts to the correct type and
 *                      dereferences. Each wrapper function hard-codes the
 *                      number and types of the parameters.
 */
typedef void (*TestWrapper_t)(void **param_array);


/**
 * \brief       Table of test function wrappers. Used by dispatch_test().
 *              This table is populated by script:
 *              generate_test_code.py
 *
 */
TestWrapper_t test_funcs[] =
{
    /* Function Id: 0 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)
    test_entropy_init_free_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO)
    test_entropy_seed_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO)
    test_entropy_write_base_seed_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)
    test_entropy_no_sources_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)
    test_entropy_too_many_sources_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(ENTROPY_HAVE_STRONG)
    test_entropy_func_len_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)
    test_entropy_source_fail_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)
    test_entropy_threshold_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY)
    test_entropy_calls_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO)
    test_nv_seed_file_create_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
    test_entropy_nv_seed_std_io_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(MBEDTLS_MD_LIGHT) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
    test_entropy_nv_seed_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_PSA_INJECT_ENTROPY) && defined(ENTROPY_HAVE_STRONG) && defined(MBEDTLS_SELF_TEST)
    test_entropy_selftest_wrapper,
#else
    NULL,
#endif

#line 145 "suites/main_test.function"
};

/**
 * \brief        Dispatches test functions based on function index.
 *
 * \param func_idx    Test function index.
 * \param params      The array of parameters to pass to the test function.
 *                    It will be decoded by the #TestWrapper_t wrapper function.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
static int dispatch_test(size_t func_idx, void **params)
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if (func_idx < (int) (sizeof(test_funcs) / sizeof(TestWrapper_t))) {
        fp = test_funcs[func_idx];
        if (fp) {
            #if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
            mbedtls_test_enable_insecure_external_rng();
            #endif

            fp(params);

            #if defined(MBEDTLS_TEST_MUTEX_USAGE)
            mbedtls_test_mutex_usage_check();
            #endif /* MBEDTLS_TEST_MUTEX_USAGE */
        } else {
            ret = DISPATCH_UNSUPPORTED_SUITE;
        }
    } else {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return ret;
}


/**
 * \brief       Checks if test function is supported in this build-time
 *              configuration.
 *
 * \param func_idx    Test function index.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
static int check_test(size_t func_idx)
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if (func_idx < (int) (sizeof(test_funcs)/sizeof(TestWrapper_t))) {
        fp = test_funcs[func_idx];
        if (fp == NULL) {
            ret = DISPATCH_UNSUPPORTED_SUITE;
        }
    } else {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return ret;
}


#line 2 "suites/host_test.function"

/**
 * \brief       Verifies that string is in string parameter format i.e. "<str>"
 *              It also strips enclosing '"' from the input string.
 *
 * \param str   String parameter.
 *
 * \return      0 if success else 1
 */
static int verify_string(char **str)
{
    if ((*str)[0] != '"' ||
        (*str)[strlen(*str) - 1] != '"') {
        mbedtls_fprintf(stderr,
                        "Expected string (with \"\") for parameter and got: %s\n", *str);
        return -1;
    }

    (*str)++;
    (*str)[strlen(*str) - 1] = '\0';

    return 0;
}

/**
 * \brief       Verifies that string is an integer. Also gives the converted
 *              integer value.
 *
 * \param str   Input string.
 * \param p_value Pointer to output value.
 *
 * \return      0 if success else 1
 */
static int verify_int(char *str, intmax_t *p_value)
{
    char *end = NULL;
    errno = 0;
    /* Limit the range to long: for large integers, the test framework will
     * use expressions anyway. */
    long value = strtol(str, &end, 0);
    if (errno == EINVAL || *end != '\0') {
        mbedtls_fprintf(stderr,
                        "Expected integer for parameter and got: %s\n", str);
        return KEY_VALUE_MAPPING_NOT_FOUND;
    }
    if (errno == ERANGE) {
        mbedtls_fprintf(stderr, "Integer out of range: %s\n", str);
        return KEY_VALUE_MAPPING_NOT_FOUND;
    }
    *p_value = value;
    return 0;
}


/**
 * \brief       Usage string.
 *
 */
#define USAGE \
    "Usage: %s [OPTIONS] files...\n\n" \
    "   Command line arguments:\n" \
    "     files...          One or more test data files. If no file is\n" \
    "                       specified the following default test case\n" \
    "                       file is used:\n" \
    "                           %s\n\n" \
    "   Options:\n" \
    "     -v | --verbose    Display full information about each test\n" \
    "     -h | --help       Display this information\n\n", \
    argv[0], \
    "TESTCASE_FILENAME"


/**
 * \brief       Read a line from the passed file pointer.
 *
 * \param f     FILE pointer
 * \param buf   Pointer to memory to hold read line.
 * \param len   Length of the buf.
 *
 * \return      0 if success else -1
 */
static int get_line(FILE *f, char *buf, size_t len)
{
    char *ret;
    int i = 0, str_len = 0, has_string = 0;

    /* Read until we get a valid line */
    do {
        ret = fgets(buf, len, f);
        if (ret == NULL) {
            return -1;
        }

        str_len = strlen(buf);

        /* Skip empty line and comment */
        if (str_len == 0 || buf[0] == '#') {
            continue;
        }
        has_string = 0;
        for (i = 0; i < str_len; i++) {
            char c = buf[i];
            if (c != ' ' && c != '\t' && c != '\n' &&
                c != '\v' && c != '\f' && c != '\r') {
                has_string = 1;
                break;
            }
        }
    } while (!has_string);

    /* Strip new line and carriage return */
    ret = buf + strlen(buf);
    if (ret-- > buf && *ret == '\n') {
        *ret = '\0';
    }
    if (ret-- > buf && *ret == '\r') {
        *ret = '\0';
    }

    return 0;
}

/**
 * \brief       Splits string delimited by ':'. Ignores '\:'.
 *
 * \param buf           Input string
 * \param len           Input string length
 * \param params        Out params found
 * \param params_len    Out params array len
 *
 * \return      Count of strings found.
 */
static int parse_arguments(char *buf, size_t len, char **params,
                           size_t params_len)
{
    size_t cnt = 0, i;
    char *cur = buf;
    char *p = buf, *q;

    params[cnt++] = cur;

    while (*p != '\0' && p < (buf + len)) {
        if (*p == '\\') {
            p++;
            p++;
            continue;
        }
        if (*p == ':') {
            if (p + 1 < buf + len) {
                cur = p + 1;
                TEST_HELPER_ASSERT(cnt < params_len);
                params[cnt++] = cur;
            }
            *p = '\0';
        }

        p++;
    }

    /* Replace backslash escapes in strings */
    for (i = 0; i < cnt; i++) {
        p = params[i];
        q = params[i];

        while (*p != '\0') {
            if (*p == '\\') {
                ++p;
                switch (*p) {
                    case 'n':
                        *p = '\n';
                        break;
                    default:
                        // Fall through to copying *p
                        break;
                }
            }
            *(q++) = *(p++);
        }
        *q = '\0';
    }

    return cnt;
}

/**
 * \brief       Converts parameters into test function consumable parameters.
 *              Example: Input:  {"int", "0", "char*", "Hello",
 *                                "hex", "abef", "exp", "1"}
 *                      Output:  {
 *                                0,                // Verified int
 *                                "Hello",          // Verified string
 *                                2, { 0xab, 0xef },// Converted len,hex pair
 *                                9600              // Evaluated expression
 *                               }
 *
 *
 * \param cnt               Parameter array count.
 * \param params            Out array of found parameters.
 * \param int_params_store  Memory for storing processed integer parameters.
 *
 * \return      0 for success else 1
 */
static int convert_params(size_t cnt, char **params,
                          mbedtls_test_argument_t *int_params_store)
{
    char **cur = params;
    char **out = params;
    int ret = DISPATCH_TEST_SUCCESS;

    while (cur < params + cnt) {
        char *type = *cur++;
        char *val = *cur++;

        if (strcmp(type, "char*") == 0) {
            if (verify_string(&val) == 0) {
                *out++ = val;
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else if (strcmp(type, "int") == 0) {
            if (verify_int(val, &int_params_store->sint) == 0) {
                *out++ = (char *) int_params_store++;
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else if (strcmp(type, "hex") == 0) {
            if (verify_string(&val) == 0) {
                size_t len;

                TEST_HELPER_ASSERT(
                    mbedtls_test_unhexify((unsigned char *) val, strlen(val),
                                          val, &len) == 0);

                int_params_store->len = len;
                *out++ = val;
                *out++ = (char *) (int_params_store++);
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else if (strcmp(type, "exp") == 0) {
            int exp_id = strtol(val, NULL, 10);
            if (get_expression(exp_id, &int_params_store->sint) == 0) {
                *out++ = (char *) int_params_store++;
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else {
            ret = (DISPATCH_INVALID_TEST_DATA);
            break;
        }
    }
    return ret;
}

/**
 * \brief       Tests snprintf implementation with test input.
 *
 * \note
 * At high optimization levels (e.g. gcc -O3), this function may be
 * inlined in run_test_snprintf. This can trigger a spurious warning about
 * potential misuse of snprintf from gcc -Wformat-truncation (observed with
 * gcc 7.2). This warning makes tests in run_test_snprintf redundant on gcc
 * only. They are still valid for other compilers. Avoid this warning by
 * forbidding inlining of this function by gcc.
 *
 * \param n         Buffer test length.
 * \param ref_buf   Expected buffer.
 * \param ref_ret   Expected snprintf return value.
 *
 * \return      0 for success else 1
 */
#if defined(__GNUC__)
__attribute__((__noinline__))
#endif
static int test_snprintf(size_t n, const char *ref_buf, int ref_ret)
{
    int ret;
    char buf[10] = "xxxxxxxxx";
    const char ref[10] = "xxxxxxxxx";

    if (n >= sizeof(buf)) {
        return -1;
    }
    ret = mbedtls_snprintf(buf, n, "%s", "123");
    if (ret < 0 || (size_t) ret >= n) {
        ret = -1;
    }

    if (strncmp(ref_buf, buf, sizeof(buf)) != 0 ||
        ref_ret != ret ||
        memcmp(buf + n, ref + n, sizeof(buf) - n) != 0) {
        return 1;
    }

    return 0;
}

/**
 * \brief       Tests snprintf implementation.
 *
 * \return      0 for success else 1
 */
static int run_test_snprintf(void)
{
    return test_snprintf(0, "xxxxxxxxx",  -1) != 0 ||
           test_snprintf(1, "",           -1) != 0 ||
           test_snprintf(2, "1",          -1) != 0 ||
           test_snprintf(3, "12",         -1) != 0 ||
           test_snprintf(4, "123",         3) != 0 ||
           test_snprintf(5, "123",         3) != 0;
}

/** \brief Write the description of the test case to the outcome CSV file.
 *
 * \param outcome_file  The file to write to.
 *                      If this is \c NULL, this function does nothing.
 * \param argv0         The test suite name.
 * \param test_case     The test case description.
 */
static void write_outcome_entry(FILE *outcome_file,
                                const char *argv0,
                                const char *test_case)
{
    /* The non-varying fields are initialized on first use. */
    static const char *platform = NULL;
    static const char *configuration = NULL;
    static const char *test_suite = NULL;

    if (outcome_file == NULL) {
        return;
    }

    if (platform == NULL) {
        platform = getenv("MBEDTLS_TEST_PLATFORM");
        if (platform == NULL) {
            platform = "unknown";
        }
    }
    if (configuration == NULL) {
        configuration = getenv("MBEDTLS_TEST_CONFIGURATION");
        if (configuration == NULL) {
            configuration = "unknown";
        }
    }
    if (test_suite == NULL) {
        test_suite = strrchr(argv0, '/');
        if (test_suite != NULL) {
            test_suite += 1; // skip the '/'
        } else {
            test_suite = argv0;
        }
    }

    /* Write the beginning of the outcome line.
     * Ignore errors: writing the outcome file is on a best-effort basis. */
    mbedtls_fprintf(outcome_file, "%s;%s;%s;%s;",
                    platform, configuration, test_suite, test_case);
}

/** \brief Write the result of the test case to the outcome CSV file.
 *
 * \param outcome_file  The file to write to.
 *                      If this is \c NULL, this function does nothing.
 * \param unmet_dep_count            The number of unmet dependencies.
 * \param unmet_dependencies         The array of unmet dependencies.
 * \param missing_unmet_dependencies Non-zero if there was a problem tracking
 *                                   all unmet dependencies, 0 otherwise.
 * \param ret                        The test dispatch status (DISPATCH_xxx).
 */
static void write_outcome_result(FILE *outcome_file,
                                 size_t unmet_dep_count,
                                 int unmet_dependencies[],
                                 int missing_unmet_dependencies,
                                 int ret)
{
    if (outcome_file == NULL) {
        return;
    }

    /* Write the end of the outcome line.
     * Ignore errors: writing the outcome file is on a best-effort basis. */
    switch (ret) {
        case DISPATCH_TEST_SUCCESS:
            if (unmet_dep_count > 0) {
                size_t i;
                mbedtls_fprintf(outcome_file, "SKIP");
                for (i = 0; i < unmet_dep_count; i++) {
                    mbedtls_fprintf(outcome_file, "%c%d",
                                    i == 0 ? ';' : ':',
                                    unmet_dependencies[i]);
                }
                if (missing_unmet_dependencies) {
                    mbedtls_fprintf(outcome_file, ":...");
                }
                break;
            }
            switch (mbedtls_test_get_result()) {
                case MBEDTLS_TEST_RESULT_SUCCESS:
                    mbedtls_fprintf(outcome_file, "PASS;");
                    break;
                case MBEDTLS_TEST_RESULT_SKIPPED:
                    mbedtls_fprintf(outcome_file, "SKIP;Runtime skip");
                    break;
                default:
                    mbedtls_fprintf(outcome_file, "FAIL;%s:%d:%s",
                                    mbedtls_get_test_filename(),
                                    mbedtls_test_get_line_no(),
                                    mbedtls_test_get_test());
                    break;
            }
            break;
        case DISPATCH_TEST_FN_NOT_FOUND:
            mbedtls_fprintf(outcome_file, "FAIL;Test function not found");
            break;
        case DISPATCH_INVALID_TEST_DATA:
            mbedtls_fprintf(outcome_file, "FAIL;Invalid test data");
            break;
        case DISPATCH_UNSUPPORTED_SUITE:
            mbedtls_fprintf(outcome_file, "SKIP;Unsupported suite");
            break;
        default:
            mbedtls_fprintf(outcome_file, "FAIL;Unknown cause");
            break;
    }
    mbedtls_fprintf(outcome_file, "\n");
    fflush(outcome_file);
}

#if defined(__unix__) ||                                \
    (defined(__APPLE__) && defined(__MACH__))
#define MBEDTLS_HAVE_CHDIR
#endif

#if defined(MBEDTLS_HAVE_CHDIR)
/** Try chdir to the directory containing argv0.
 *
 * Failures are silent.
 */
static void try_chdir_if_supported(const char *argv0)
{
    /* We might want to allow backslash as well, for Windows. But then we also
     * need to consider chdir() vs _chdir(), and different conventions
     * regarding paths in argv[0] (naively enabling this code with
     * backslash support on Windows leads to chdir into the wrong directory
     * on the CI). */
    const char *slash = strrchr(argv0, '/');
    if (slash == NULL) {
        return;
    }
    size_t path_size = slash - argv0 + 1;
    char *path = mbedtls_calloc(1, path_size);
    if (path == NULL) {
        return;
    }
    memcpy(path, argv0, path_size - 1);
    path[path_size - 1] = 0;
    int ret = chdir(path);
    if (ret != 0) {
        mbedtls_fprintf(stderr, "%s: note: chdir(\"%s\") failed.\n",
                        __func__, path);
    }
    mbedtls_free(path);
}
#else /* MBEDTLS_HAVE_CHDIR */
/* No chdir() or no support for parsing argv[0] on this platform. */
static void try_chdir_if_supported(const char *argv0)
{
    (void) argv0;
    return;
}
#endif /* MBEDTLS_HAVE_CHDIR */

/**
 * \brief       Desktop implementation of execute_tests().
 *              Parses command line and executes tests from
 *              supplied or default data file.
 *
 * \param argc  Command line argument count.
 * \param argv  Argument array.
 *
 * \return      Program exit status.
 */
static int execute_tests(int argc, const char **argv)
{
    /* Local Configurations and options */
    const char *default_filename = ".\\test_suite_entropy.datax";
    const char *test_filename = NULL;
    const char **test_files = NULL;
    size_t testfile_count = 0;
    int option_verbose = 0;
    size_t function_id = 0;

    /* Other Local variables */
    int arg_index = 1;
    const char *next_arg;
    size_t testfile_index, i, cnt;
    int ret;
    unsigned total_errors = 0, total_tests = 0, total_skipped = 0;
    FILE *file;
    char buf[5000];
    char *params[50];
    /* Store for processed integer params. */
    mbedtls_test_argument_t int_params[50];
    void *pointer;
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    int stdout_fd = -1;
#endif /* __unix__ || __APPLE__ __MACH__ */
    const char *outcome_file_name = getenv("MBEDTLS_TEST_OUTCOME_FILE");
    FILE *outcome_file = NULL;

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && \
    !defined(TEST_SUITE_MEMORY_BUFFER_ALLOC)
    unsigned char alloc_buf[1000000];
    mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));
#endif

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_init();
#endif

    /*
     * The C standard doesn't guarantee that all-bits-0 is the representation
     * of a NULL pointer. We do however use that in our code for initializing
     * structures, which should work on every modern platform. Let's be sure.
     */
    memset(&pointer, 0, sizeof(void *));
    if (pointer != NULL) {
        mbedtls_fprintf(stderr, "all-bits-zero is not a NULL pointer\n");
        return 1;
    }

    /*
     * Make sure we have a snprintf that correctly zero-terminates
     */
    if (run_test_snprintf() != 0) {
        mbedtls_fprintf(stderr, "the snprintf implementation is broken\n");
        return 1;
    }

    if (outcome_file_name != NULL && *outcome_file_name != '\0') {
        outcome_file = fopen(outcome_file_name, "a");
        if (outcome_file == NULL) {
            mbedtls_fprintf(stderr, "Unable to open outcome file. Continuing anyway.\n");
        }
    }

    while (arg_index < argc) {
        next_arg = argv[arg_index];

        if (strcmp(next_arg, "--verbose") == 0 ||
            strcmp(next_arg, "-v") == 0) {
            option_verbose = 1;
        } else if (strcmp(next_arg, "--help") == 0 ||
                   strcmp(next_arg, "-h") == 0) {
            mbedtls_fprintf(stdout, USAGE);
            mbedtls_exit(EXIT_SUCCESS);
        } else {
            /* Not an option, therefore treat all further arguments as the file
             * list.
             */
            test_files = &argv[arg_index];
            testfile_count = argc - arg_index;
            break;
        }

        arg_index++;
    }

    /* If no files were specified, assume a default */
    if (test_files == NULL || testfile_count == 0) {
        test_files = &default_filename;
        testfile_count = 1;
    }

    /* Initialize the struct that holds information about the last test */
    mbedtls_test_info_reset();

    /* Now begin to execute the tests in the testfiles */
    for (testfile_index = 0;
         testfile_index < testfile_count;
         testfile_index++) {
        size_t unmet_dep_count = 0;
        int unmet_dependencies[20];
        int missing_unmet_dependencies = 0;

        test_filename = test_files[testfile_index];

        file = fopen(test_filename, "r");
        if (file == NULL) {
            mbedtls_fprintf(stderr, "Failed to open test file: %s\n",
                            test_filename);
            if (outcome_file != NULL) {
                fclose(outcome_file);
            }
            return 1;
        }

        while (!feof(file)) {
            if (unmet_dep_count > 0) {
                mbedtls_fprintf(stderr,
                                "FATAL: Dep count larger than zero at start of loop\n");
                mbedtls_exit(MBEDTLS_EXIT_FAILURE);
            }
            unmet_dep_count = 0;
            missing_unmet_dependencies = 0;

            if ((ret = get_line(file, buf, sizeof(buf))) != 0) {
                break;
            }
            mbedtls_fprintf(stdout, "%s%.66s",
                            mbedtls_test_get_result() == MBEDTLS_TEST_RESULT_FAILED ?
                            "\n" : "", buf);
            mbedtls_fprintf(stdout, " ");
            for (i = strlen(buf) + 1; i < 67; i++) {
                mbedtls_fprintf(stdout, ".");
            }
            mbedtls_fprintf(stdout, " ");
            fflush(stdout);
            write_outcome_entry(outcome_file, argv[0], buf);

            total_tests++;

            if ((ret = get_line(file, buf, sizeof(buf))) != 0) {
                break;
            }
            cnt = parse_arguments(buf, strlen(buf), params,
                                  sizeof(params) / sizeof(params[0]));

            if (strcmp(params[0], "depends_on") == 0) {
                for (i = 1; i < cnt; i++) {
                    int dep_id = strtol(params[i], NULL, 10);
                    if (dep_check(dep_id) != DEPENDENCY_SUPPORTED) {
                        if (unmet_dep_count <
                            ARRAY_LENGTH(unmet_dependencies)) {
                            unmet_dependencies[unmet_dep_count] = dep_id;
                            unmet_dep_count++;
                        } else {
                            missing_unmet_dependencies = 1;
                        }
                    }
                }

                if ((ret = get_line(file, buf, sizeof(buf))) != 0) {
                    break;
                }
                cnt = parse_arguments(buf, strlen(buf), params,
                                      sizeof(params) / sizeof(params[0]));
            }

            // If there are no unmet dependencies execute the test
            if (unmet_dep_count == 0) {
                mbedtls_test_info_reset();

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
                /* Suppress all output from the library unless we're verbose
                 * mode
                 */
                if (!option_verbose) {
                    stdout_fd = redirect_output(stdout, "/dev/null");
                    if (stdout_fd == -1) {
                        /* Redirection has failed with no stdout so exit */
                        exit(1);
                    }
                }
#endif /* __unix__ || __APPLE__ __MACH__ */

                function_id = strtoul(params[0], NULL, 10);
                if ((ret = check_test(function_id)) == DISPATCH_TEST_SUCCESS) {
                    ret = convert_params(cnt - 1, params + 1, int_params);
                    if (DISPATCH_TEST_SUCCESS == ret) {
                        ret = dispatch_test(function_id, (void **) (params + 1));
                    }
                }

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
                if (!option_verbose && restore_output(stdout, stdout_fd)) {
                    /* Redirection has failed with no stdout so exit */
                    exit(1);
                }
#endif /* __unix__ || __APPLE__ __MACH__ */

            }

            write_outcome_result(outcome_file,
                                 unmet_dep_count, unmet_dependencies,
                                 missing_unmet_dependencies,
                                 ret);
            if (unmet_dep_count > 0 || ret == DISPATCH_UNSUPPORTED_SUITE) {
                total_skipped++;
                mbedtls_fprintf(stdout, "----");

                if (1 == option_verbose && ret == DISPATCH_UNSUPPORTED_SUITE) {
                    mbedtls_fprintf(stdout, "\n   Test Suite not enabled");
                }

                if (1 == option_verbose && unmet_dep_count > 0) {
                    mbedtls_fprintf(stdout, "\n   Unmet dependencies: ");
                    for (i = 0; i < unmet_dep_count; i++) {
                        mbedtls_fprintf(stdout, "%d ",
                                        unmet_dependencies[i]);
                    }
                    if (missing_unmet_dependencies) {
                        mbedtls_fprintf(stdout, "...");
                    }
                }
                mbedtls_fprintf(stdout, "\n");
                fflush(stdout);

                unmet_dep_count = 0;
                missing_unmet_dependencies = 0;
            } else if (ret == DISPATCH_TEST_SUCCESS) {
                if (mbedtls_test_get_result() == MBEDTLS_TEST_RESULT_SUCCESS) {
                    mbedtls_fprintf(stdout, "PASS\n");
                } else if (mbedtls_test_get_result() == MBEDTLS_TEST_RESULT_SKIPPED) {
                    mbedtls_fprintf(stdout, "----\n");
                    total_skipped++;
                } else {
                    char line_buffer[MBEDTLS_TEST_LINE_LENGTH];

                    total_errors++;
                    mbedtls_fprintf(stdout, "FAILED\n");
                    mbedtls_fprintf(stdout, "  %s\n  at ",
                                    mbedtls_test_get_test());
                    if (mbedtls_test_get_step() != (unsigned long) (-1)) {
                        mbedtls_fprintf(stdout, "step %lu, ",
                                        mbedtls_test_get_step());
                    }
                    mbedtls_fprintf(stdout, "line %d, %s",
                                    mbedtls_test_get_line_no(),
                                    mbedtls_get_test_filename());

                    mbedtls_test_get_line1(line_buffer);
                    if (line_buffer[0] != 0) {
                        mbedtls_fprintf(stdout, "\n  %s", line_buffer);
                    }
                    mbedtls_test_get_line2(line_buffer);
                    if (line_buffer[0] != 0) {
                        mbedtls_fprintf(stdout, "\n  %s", line_buffer);
                    }
                }
                fflush(stdout);
            } else if (ret == DISPATCH_INVALID_TEST_DATA) {
                mbedtls_fprintf(stderr, "FAILED: FATAL PARSE ERROR\n");
                fclose(file);
                mbedtls_exit(2);
            } else if (ret == DISPATCH_TEST_FN_NOT_FOUND) {
                mbedtls_fprintf(stderr, "FAILED: FATAL TEST FUNCTION NOT FOUND\n");
                fclose(file);
                mbedtls_exit(2);
            } else {
                total_errors++;
            }
        }
        fclose(file);
    }

    if (outcome_file != NULL) {
        fclose(outcome_file);
    }

    mbedtls_fprintf(stdout,
                    "\n----------------------------------------------------------------------------\n\n");
    if (total_errors == 0) {
        mbedtls_fprintf(stdout, "PASSED");
    } else {
        mbedtls_fprintf(stdout, "FAILED");
    }

    mbedtls_fprintf(stdout, " (%u / %u tests (%u skipped))\n",
                    total_tests - total_errors, total_tests, total_skipped);

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_end();
#endif

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && \
    !defined(TEST_SUITE_MEMORY_BUFFER_ALLOC)
#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_status();
#endif
    mbedtls_memory_buffer_alloc_free();
#endif

    return total_errors != 0;
}


#line 217 "suites/main_test.function"

/*----------------------------------------------------------------------------*/
/* Main Test code */


/**
 * \brief       Program main. Invokes platform specific execute_tests().
 *
 * \param argc      Command line arguments count.
 * \param argv      Array of command line arguments.
 *
 * \return       Exit code.
 */
int main(int argc, const char *argv[])
{
#if defined(MBEDTLS_TEST_HOOKS)
    extern void (*mbedtls_test_hook_test_fail)(const char *test, int line, const char *file);
    mbedtls_test_hook_test_fail = &mbedtls_test_fail;
#if defined(MBEDTLS_ERROR_C)
    mbedtls_test_hook_error_add = &mbedtls_test_err_add_check;
#endif
#endif

    /* Try changing to the directory containing the executable, if
     * using the default data file. This allows running the executable
     * from another directory (e.g. the project root) and still access
     * the .datax file as well as data files used by test cases
     * (typically from framework/data_files).
     *
     * Note that we do this before the platform setup (which may access
     * files such as a random seed). We also do this before accessing
     * test-specific files such as the outcome file, which is arguably
     * not desirable and should be fixed later.
     */
    if (argc == 1) {
        try_chdir_if_supported(argv[0]);
    }

    int ret = mbedtls_test_platform_setup();
    if (ret != 0) {
        mbedtls_fprintf(stderr,
                        "FATAL: Failed to initialize platform - error %d\n",
                        ret);
        return -1;
    }

    ret = execute_tests(argc, argv);
    mbedtls_test_platform_teardown();
    return ret;
}
