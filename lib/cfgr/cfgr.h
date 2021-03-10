
#ifndef CFGR_H
#define CFGR_H


#define CFGR_FLAG_WRITE  0x01    // Config variable may be overwritten
#define CFGR_FLAG_SECRET 0x02    // Secret config variable, should not be externally exposed

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

/// Configuration entry kind enumeration
/// This selects the parser / encoders used for a given entry
typedef enum {
    CFGR_KIND_FLAG = 0,      // Boolean / flag
    CFGR_KIND_INT = 1,       // Signed 32-bit integer
    CFGR_KIND_UINT = 2,      // Unsigned 32-bit integer
    CFGR_KIND_STR = 3,       // String value
    CFGR_KIND_BLOB = 4,      // Binary blob
    CFGR_KIND_OTHER = 0xFF,  // Other / externally defined object type
} cfgr_kind_e;

/// Configuration entries define a configuration option
typedef struct {
    char* name;     // Configuration value name, mapped to file or key in k/v store
    uint32_t kind;  // Configuration data kind
    uint32_t flags; // Configuration key flags
} cfgr_entry_t;

/// Configuration value type
typedef struct {
    uint32_t kind;  // Configuration value kind
    size_t len;     // Configuration value length
    // Configuration value data
    uint8_t data[];
} cfgr_value_t;

typedef union {
    bool flag;
    uint32_t u32;
    int32_t i32;
    char *str;
    uint8_t *blob;
} cfgr_data_u;

/// Configuration manager return enumeration
typedef enum {
    CFGR_RES_OK = 0,
    CFGR_ERR_NO_CTX = -1,
    CFGR_ERR_NO_ENTRIES = -2,
    CFGR_ERR_NO_BACKEND = -3,
    CFGR_ERR_NO_ENTRY = -4,
    CFGR_ERR_BACKEND = -5,
    CFGR_ERR_ALLOC = -6,
} cfgr_res_e;

/// Backends are bound using read/write functions with a context provided to store
/// relevant information. _If_ backends support reading and writing flags these may
/// be used for unrecognised entries, however will be overwritten by those in the
/// configuration object.

/// Backend read function
/// Returns -ve on error or the number of data bytes read
typedef int(*cfgr_backend_read_f)(void* ctx, char* key, int32_t kind, uint32_t* flags, uint8_t* data, size_t max_data_len);

/// Backend write function
/// Returns -ve on error or the number of data bytes written
typedef int(*cfgr_backend_write_f)(void* ctx, char* key, int32_t kind, uint32_t* flags, uint8_t* data);


/// Configuration manager object
typedef struct {
    cfgr_entry_t* entries;          /// List of configuration entries
    size_t entry_count;             /// Number of entries in configuration list

    void* be_ctx;                   /// Context for backend function calls
    cfgr_backend_read_f be_read;    /// Backend read function
    cfgr_backend_write_f be_write;  /// Backend write function
} cfgr_mgr_t;


/// Initialise the configuration manager
int CFGR_init(cfgr_mgr_t* cfg);


/// Fetch a configuration value
int CFGR_get(cfgr_mgr_t* cfg, char* key, cfgr_value_t** value);


/// Write a configuration value
int CFGR_set(cfgr_mgr_t* cfg, cfgr_value_t *value);


int CFGR_print_val(cfgr_value_t* value);

#endif


