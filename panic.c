#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/kdb.h>

static int timewait = 0;
static char method[128];
static int enter = 0;

static struct sysctl_ctx_list clist;
static struct sysctl_oid *poid;
static int (*null_func_p)(void) = NULL; 
static int *rnd; 

static int
sysctl_panic_procedure(SYSCTL_HANDLER_ARGS)
{
    int error = 0;
    error = sysctl_handle_int(oidp, &enter, 0, req);

    if (enter == 1) {
        printf("Panic enter: wait %d secs, method %s\n",
                timewait, method);
        if (timewait) pause("Panic wait", timewait*hz);
        printf("Panic execute!\n");
        if (strcmp(method, "kdb") == 0)
            kdb_enter(KDB_WHY_SYSCTL, "sysctl panic.enter");
        else if (strcmp(method, "direct") == 0)
            panic("Panic by panic kld");
        else if (strcmp(method, "null") == 0)
            null_func_p();
        else if (strcmp(method, "random") == 0) {
            rnd = (int *)random();
            printf("%d", *rnd);
        } else {
            printf("Panic: method not support!\n");
        }
            
    }
    enter = 0;
    

    return error;
}


static int
panic_modevent(module_t mod __unused, int event, void *arg __unused)
{
    int error = 0;

    switch (event) {
        case MOD_LOAD:
            sysctl_ctx_init(&clist);

            poid = SYSCTL_ADD_ROOT_NODE(&clist, OID_AUTO,
                    "panic", 0, 0, "panic root");
            if (poid == NULL) {
                uprintf("SYSCTL_ADD_NODE failed.\n");
                return (EINVAL);
            }
            SYSCTL_ADD_INT(&clist, SYSCTL_CHILDREN(poid), OID_AUTO,
                    "timewait", CTLFLAG_RW, &timewait, 0, "time to panic (in seconds)");
            SYSCTL_ADD_STRING(&clist, SYSCTL_CHILDREN(poid), OID_AUTO,
                    "method", CTLFLAG_RW, method, sizeof(method), "direct, null, random, kdb");
            SYSCTL_ADD_PROC(&clist, SYSCTL_CHILDREN(poid), OID_AUTO,
                    "enter", CTLTYPE_INT | CTLFLAG_RW, 0, 0, sysctl_panic_procedure,
                    "I", "enter panic");

            uprintf("Panic module loaded, use 'sysctl panic' to execute.\n");
            break;
        case MOD_UNLOAD:
            if (sysctl_ctx_free(&clist)) {
                uprintf("sysctl_ctx_free failed.\n");
                return (ENOTEMPTY);
            }
            uprintf("Panic module unloaded.\n");
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }

    return (error);
}

static moduledata_t panic_mod = {
    "panic",
    panic_modevent,
    NULL
};

DECLARE_MODULE(panic, panic_mod, SI_SUB_EXEC, SI_ORDER_ANY);



