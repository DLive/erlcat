//
//  erlcat.c
//
//  Created by zengxiantao951 on 2018/12/27.
//  Copyright © 2018 pingan. All rights reserved.
//
#include <stdio.h>
#include <string.h>

#include "../include/client.h"
#include "erl_nif.h"

#define MAXKEYLEN 128
#define MAXVALLEN 1024

static ErlNifResourceType* cattrans_res = NULL;
typedef struct _transaction_t {
    CatTransaction* _trans;
} transaction_t;

static ERL_NIF_TERM make_atom(ErlNifEnv *env, const char *atom_name) {
    ERL_NIF_TERM atom;
    if(enif_make_existing_atom(env, atom_name, &atom, ERL_NIF_LATIN1)) {
        return atom;
    }
    return enif_make_atom(env, atom_name);
}

static void cattrans_cleanup(ErlNifEnv* env, void* arg) {
//    enif_free(arg);
}

static int load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    cattrans_res = enif_open_resource_type(env,NULL,"cattrans",
        &cattrans_cleanup,
        ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER
        ,NULL);
    return 0;
}


//Common Apis.

// 初始化cat实例.
ERL_NIF_TERM catClientInitForErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char appKey[MAXKEYLEN];
    (void)memset(&appKey, '\0', sizeof(appKey));
    
    if (enif_get_string(env, argv[0], appKey, sizeof(appKey), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    CatClientConfig config = DEFAULT_CCAT_CONFIG;
    config.enableHeartbeat = 0;
    config.enableDebugLog = 1;
    config.encoderType = 0;
    // catClientInitWithConfig(appKey, &config);

    return enif_make_int(env, catClientInitWithConfig(appKey,&config));
    //return enif_make_int(env, catClientInit(appKey));
}

// 返回cat版本.
ERL_NIF_TERM catVersionForErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return enif_make_string(env, catVersion(), ERL_NIF_LATIN1);
}

// 返回cat是否可用（是否被初始化）.
ERL_NIF_TERM isCatEnabledForErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return enif_make_int(env, isCatEnabled());
}

// 销毁cat实例，释放所有cat已经申请的资源.
ERL_NIF_TERM catClientDestroyForErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return enif_make_int(env, catClientDestroy());
}

//Event Apis.

// 记录Event.
ERL_NIF_TERM logEventForErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char type[MAXKEYLEN];
    (void)memset(&type, '\0', sizeof(type));
    
    if (enif_get_string(env, argv[0], type, sizeof(type), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    char name[MAXKEYLEN];
    (void)memset(&name, '\0', sizeof(name));
    
    if (enif_get_string(env, argv[1], name, sizeof(name), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    char status[MAXKEYLEN];
    (void)memset(&status, '\0', sizeof(status));
    
    if (enif_get_string(env, argv[2], status, sizeof(status), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    char data[MAXVALLEN];
    (void)memset(&data, '\0', sizeof(data));
    
    if (enif_get_string(env, argv[3], data, sizeof(data), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    logEvent(type, name, status, data);
    return make_atom(env, "ok");
}

// 记录错误Event.
ERL_NIF_TERM logErrorForErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char msg[MAXKEYLEN];
    (void)memset(&msg, '\0', sizeof(msg));
    
    if (enif_get_string(env, argv[0], msg, sizeof(msg), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    char errStr[MAXVALLEN];
    (void)memset(&errStr, '\0', sizeof(errStr));
    
    if (enif_get_string(env, argv[1], errStr, sizeof(errStr), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    logError(msg, errStr);
    return make_atom(env, "ok");
}

// 记录count类型Metric.
ERL_NIF_TERM logMetricForCountOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char name[MAXKEYLEN];
    (void)memset(&name, '\0', sizeof(name));
    
    if (enif_get_string(env, argv[0], name, sizeof(name), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    int count;
    if(!enif_get_int(env, argv[1], &count)) {
        return enif_make_badarg(env);
    }
    
    logMetricForCount(name, count);
    return make_atom(env, "ok");
}

// 记录duration类型Metric.
ERL_NIF_TERM logMetricForDurationOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char name[MAXKEYLEN];
    (void)memset(&name, '\0', sizeof(name));
    
    if (enif_get_string(env, argv[0], name, sizeof(name), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    long duration;
    if(!enif_get_long(env, argv[1], &duration)) {
        return enif_make_badarg(env);
    }
    
    logMetricForDuration(name, duration);
    return make_atom(env, "ok");
}

// 记录sum类型(等价count类型)Metric.
ERL_NIF_TERM logMetricForSumOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char name[MAXKEYLEN];
    (void)memset(&name, '\0', sizeof(name));
    
    if (enif_get_string(env, argv[0], name, sizeof(name), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    int count;
    if(!enif_get_int(env, argv[1], &count)) {
        return enif_make_badarg(env);
    }
    
    logMetricForCount(name, count);
    return make_atom(env, "ok");
}

// 记录耗时类的Transaction.
ERL_NIF_TERM logTransactionWithDurationOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char type[MAXKEYLEN];
    (void)memset(&type, '\0', sizeof(type));
    
    if (enif_get_string(env, argv[0], type, sizeof(type), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    char name[MAXKEYLEN];
    (void)memset(&name, '\0', sizeof(name));
    
    if (enif_get_string(env, argv[1], name, sizeof(name), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    
    long duration;
    if(!enif_get_long(env, argv[2], &duration)) {
        return enif_make_badarg(env);
    }
    
    newCompletedTransactionWithDuration(type, name, duration);
    return make_atom(env, "ok");
}

// 记录耗时类的Transaction.
ERL_NIF_TERM newTransactionOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    char type[MAXKEYLEN];
    char name[MAXKEYLEN];
//    CatTransaction* trans;
    transaction_t* trans_t;
    ERL_NIF_TERM retterm;
    (void)memset(&type, '\0', sizeof(type));
    
    if (enif_get_string(env, argv[0], type, sizeof(type), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }

    (void)memset(&name, '\0', sizeof(name));
    
    if (enif_get_string(env, argv[1], name, sizeof(name), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }

    trans_t = enif_alloc_resource(cattrans_res, sizeof(transaction_t));
    if(trans_t == NULL) return enif_make_badarg(env);
    retterm = enif_make_resource(env, trans_t);

	trans_t->_trans = newTransaction(type, name);

//    enif_release_resource(trans_t);
    return retterm;
}

// 记录耗时类的Transaction complete.
ERL_NIF_TERM completeOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    CatTransaction *trans;
    transaction_t* trans_t;
    if (!enif_get_resource(env, argv[0], cattrans_res, (void**) &trans_t)) {
        return enif_make_badarg(env);
    }
    printf("getcattrans %s\n",trans_t);
    trans=trans_t->_trans;
    trans->complete(trans);
    trans_t->_trans=NULL;
    enif_release_resource(trans_t);
    return make_atom(env, "ok");
}

ERL_NIF_TERM setStatusOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    // CatTransaction *trans;
    transaction_t* trans_t;
    char status[MAXKEYLEN];
    if (!enif_get_resource(env, argv[0], cattrans_res, (void**) &trans_t)) {
        return enif_make_badarg(env);
    }
    (void)memset(&status, '\0', sizeof(status));
    if (enif_get_string(env, argv[1], status, sizeof(status), ERL_NIF_LATIN1) < 1) {
        return enif_make_badarg(env);
    }
    // printf("getcattrans %s\n",trans_t);
    CatTransaction *trans=trans_t->_trans;
    trans->setStatus(trans, status);
    return make_atom(env, "ok");
}

// 创建messageId.
ERL_NIF_TERM createMessageIdOfErlang(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return enif_make_string(env, createMessageId(), ERL_NIF_LATIN1);
}

static ErlNifFunc nif_funcs[] = {
    {"cat_client_init", 1, catClientInitForErlang},
    
    {"cat_version", 0, catVersionForErlang},
    {"is_cat_enabled", 0, isCatEnabledForErlang},
    {"cat_client_destroy", 0, catClientDestroyForErlang},
    {"create_message_id", 0, createMessageIdOfErlang},
    
    {"new_transaction",2,newTransactionOfErlang},
    {"complete",1,completeOfErlang},
    {"set_status",2,setStatusOfErlang},

    {"log_event", 4, logEventForErlang},
    {"log_error", 2, logErrorForErlang},
    {"log_metric_for_count", 2, logMetricForCountOfErlang},
    {"log_metric_for_duration", 2, logMetricForDurationOfErlang},
    {"log_metric_for_sum", 2, logMetricForSumOfErlang},
    {"log_transaction_with_duration", 3, logTransactionWithDurationOfErlang}
};

ERL_NIF_INIT(erlcat, nif_funcs, &load, NULL, NULL, NULL);

