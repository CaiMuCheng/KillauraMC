#include <jni.h>
#include <Compose.h>
#include <bits/sysconf.h>
#include <asm-generic/mman.h>
#include <sys/mman.h>
#include "Substrate.h"
#include <android/log.h>
#include <set>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "KillauraNative", __VA_ARGS__)

bool initialized = false;
Player *thePlayer = nullptr;
Level *theLevel = nullptr;
GameMode *gameMode = nullptr;
std::set<Entity *> entities;
JNIEnv *GlobalEnv = nullptr;

jobject getApplicationContext(JNIEnv *env) {
    //获取Activity Thread的实例对象
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread,
                                                             "currentActivityThread",
                                                             "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication",
                                                "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    return context;
}

void showToast(JNIEnv *env, jstring toast) {
    jclass jc_Toast = env->FindClass("android/widget/Toast");
    jmethodID jm_makeText = env->GetStaticMethodID(jc_Toast, "makeText",
                                                   "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    jobject jo_Toast = env->CallStaticObjectMethod(jc_Toast, jm_makeText,
                                                   getApplicationContext(env), toast, 0);
    jmethodID jm_Show = env->GetMethodID(jc_Toast, "show", "()V");
    env->CallVoidMethod(jo_Toast, jm_Show);
}

void (*old_attack)(Player *, Entity &);

void hook_attack(Player *player, Entity &entity) {
    old_attack(player, entity);
    LOGE("%p ==? %p", player, &entity);
    entities.insert(&entity);
}

void (*old_level_tick)(Level *);

void hook_level_tick(Level *level) {
    old_level_tick(level);
    theLevel = level;
    thePlayer = level->getLocalPlayer();
}

void (*old_tick)(GameMode *);

void hook_tick(GameMode *pGameMode) {
    old_tick(pGameMode);
    gameMode = pGameMode;
}

bool MSHookFunctionFixed(void *origin, void *hook, void **result) {
    int pagesize = sysconf(_SC_PAGESIZE);
    int protectsize = pagesize * 4;
    void *protectaddr = (void *) ((int) origin - ((int) origin % pagesize) - pagesize);
    if (mprotect(protectaddr, protectsize, PROT_EXEC | PROT_READ | PROT_WRITE) == 0) {
        MSHookFunction(origin, hook, result);
        return true;
    } else {
        return false;
    }
}


void native_attack(JNIEnv *env, jclass clazz, jfloat radius) {
    if (!initialized) {
        showToast(env, env->NewStringUTF("Killaura Core has't be initialized"));
        return;
    }

    LOGE("entity size: %d", entities.size());
    if (thePlayer != nullptr && theLevel != nullptr && gameMode != nullptr) {
        for (auto &entity: entities) {
            if (entity != nullptr) {
                if (thePlayer->getUniqueID() != entity->getUniqueID() && !entity->isRemoved()) {
                    // Attack the entity!
                    gameMode->attack(*thePlayer, *entity);
                }
            }
        }
    }

}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    if (vm->GetEnv((void **) &GlobalEnv, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    void *hookHandle = dlopen("libsubstrate.so", RTLD_GLOBAL | RTLD_LAZY);
    if (hookHandle == nullptr) {
        showToast(GlobalEnv, GlobalEnv->NewStringUTF("Open libsubstrate.so error."));
        return JNI_VERSION_1_6;
    }

    showToast(GlobalEnv, GlobalEnv->NewStringUTF("Openning libminecraft.so...."));
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    if (handle == nullptr) {
        showToast(GlobalEnv, GlobalEnv->NewStringUTF("Open libminecraft.so error."));
        return JNI_VERSION_1_6;
    }

    showToast(GlobalEnv,
              GlobalEnv->NewStringUTF("Open libsubstrate.so & libminecraft.so successfully."));

    bool hookLevelTick = MSHookFunctionFixed(dlsym(handle, "_ZN5Level4tickEv"),
                                             (void *) &hook_level_tick, (void **) &old_level_tick);
    bool hookAttack = MSHookFunctionFixed(dlsym(handle, "_ZN6Player6attackER6Entity"),
                                          (void *) &hook_attack,
                                          (void **) &old_attack);
    bool hookGameMode = MSHookFunctionFixed(dlsym(handle, "_ZN8GameMode4tickEv"),
                                            (void *) &hook_tick,
                                            (void **) &old_tick);

    if (hookLevelTick && hookAttack && hookGameMode) {
        showToast(GlobalEnv, GlobalEnv->NewStringUTF("Killaura Core is initialized successfully."));
    } else {
        showToast(GlobalEnv, GlobalEnv->NewStringUTF("Killaura Core is initialized error."));
        return JNI_VERSION_1_6;
    }

    jclass featuresClazz = GlobalEnv->FindClass(
            "io/github/caimucheng/killauramc/features/Features");
    if (featuresClazz == nullptr) {
        showToast(GlobalEnv, GlobalEnv->NewStringUTF("No mapped class"));
        return JNI_VERSION_1_6;
    }
    JNINativeMethod methods[] = {{
                                         "attack", "(F)V", (void *) native_attack
                                 }};
    if (GlobalEnv->RegisterNatives(featuresClazz, methods, 1) != JNI_OK) {
        showToast(GlobalEnv, GlobalEnv->NewStringUTF("JNI Register Failure"));
        return JNI_VERSION_1_6;
    }

    // Init core
    initialized = true;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *unused) {
    initialized = false;
}