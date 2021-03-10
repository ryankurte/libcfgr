
#include "cfgr/cfgr.h"

#include <string.h>

#ifndef CFGR_MAX_DATA
#define CFGR_MAX_DATA 1024
#endif

int CFGR_init(cfgr_mgr_t* cfg) {
    if (!cfg) {
        return CFGR_ERR_NO_CTX;
    }

    if (!cfg->entries || (cfg->entry_count == 0)) {
        return CFGR_ERR_NO_ENTRIES;
    }

    if (!cfg->be_read || !cfg->be_read) {
        return CFGR_ERR_NO_BACKEND;
    }

    return CFGR_RES_OK;
}

// Find config entry index by name
int cfgr_find_entry_idx(cfgr_mgr_t* cfg, char* key) {
    for (size_t i=0; i<cfg->entry_count; i++) {
        if (strcmp(key, cfg->entries[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

// Find config entry pointer by name
cfgr_entry_t* cfgr_find_entry_ptr(cfgr_mgr_t* cfg, char* key) {
    for (size_t i=0; i<cfg->entry_count; i++) {
        if (strcmp(key, cfg->entries[i].name) == 0) {
            return &cfg->entries[i];
        }
    }
    return NULL;
}

int CFGR_get(cfgr_mgr_t* cfg, char* key, cfgr_value_t** value) {
    uint32_t flags = 0;

    // Look-up entry
    cfgr_entry_t* e = cfgr_find_entry_ptr(cfg, key);
    if (!e) {
        return CFGR_ERR_NO_ENTRY;
    }

    // Read length from underlying driver
    int res = cfg->be_read(cfg->be_ctx, key, e->kind, &flags, NULL, 0);
    if (res < 0) {
        return CFGR_ERR_BACKEND;
    } else if (res == 0) {
        return CFGR_ERR_NO_ENTRY;
    }
    int value_len = res;

    // Allocate storage for value
    *value = calloc(sizeof(cfgr_value_t) * res);
    if (!*value) {
        return CFGR_ERR_ALLOC;
    }

    // Read actual data
    res = cfg->be_read(cfg->be_ctx, key, e->kind, &flags, (*value)->data, value_len);
    if (res < 0) {
        free(*value);
        return CFGR_ERR_BACKEND;
    }

    // Assign data to value
    (*value)->kind = e->kind;
    (*value)->len = value_len;

    return -1;
}


int CFGR_set(cfgr_mgr_t* cfg, cfgr_value_t *value) {
    return -1;
}

