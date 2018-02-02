#define  LOG_TAG    "SCANNER"
#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <pthread.h>

#include <fcntl.h>
#include <linux/types.h>
#include <linux/ioctl.h>

#include <cutils/sockets.h>
#include <cutils/log.h>
#include <JNIHelp.h>
#include <ScopedLocalRef.h>
#include "android_runtime/AndroidRuntime.h"
using namespace android;

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define CMD_QUIT            'Q'
//fengmc modify    171225  180104
//static const char *classPathName = "com/routon/scanner/RoutonScanner";
static const char *classPathName = "android/hardware/RoutonScanner";
//fengmc end
#define SERIAL_READ     0
static const char *devpath = "/dev/ttyI2C0";

#if SERIAL_READ
static int send_cmd_serial(int fd)
{
    unsigned char testdata[]={0x7e, 0, 0, 0x05, 0x33, 0x48, 0x30, 0x33, 0x30, 0xB2, 0x0d, 0x0a};
    int l = write(fd, testdata, sizeof(testdata));
    printf("\nwrite l: %d\n", l);
     return l==len ? 0 : -1;
}
#endif


#define MAX_FRAME_SIZE  2048
static char frame[MAX_FRAME_SIZE];
static char *pf = frame;

struct scanner_jni_native_data {
    pthread_t thread;
    JavaVM *vm;
    JNIEnv *e;
    jobject o;
    jmethodID cb;
    int control[2];
};

static void checkAndClearExceptionFromCallback(JNIEnv* env, const char* methodName) {
    if (env->ExceptionCheck()) {
        LOGE("An exception was thrown by callback '%s'.", methodName);
        env->ExceptionClear();
    }
}

static void post_frame(
        struct scanner_jni_native_data *nat,
        char *data, int size)
{
    data[size] = 0;

    JNIEnv *env = nat->e;
    jstring jstr = env->NewStringUTF(data);
    // ALOGI("messageMe found, call it... (%d)(%s)", size, data);
    env->CallVoidMethod(nat->o, nat->cb, jstr);
    env->DeleteLocalRef(jstr);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
}

static void parse_data(
        struct scanner_jni_native_data *nat,
        char *data, int size)
{
    if (size <= 0)
        return;

    char *p = data;
    if (pf == frame) {
        // cut head zeros
        while (*p == 0) {
            size--;
            p++;
        }
    }

    if (size > 0) {
        // if larger than buf, post the frame
        if (pf + size > frame + sizeof(frame)) {
            ALOGI("frame full\n");
            post_frame(nat, frame, pf - frame);
            pf = frame;
            return;
        }

        memcpy(pf, p, size);
        pf += size;

        if (size >= 2) {
            // tail: 0x0d, 0x0a
            if ((p[size - 1] == 0x0a) && (p[size - 2] == 0x0d)) {
                // a whole frame, post the frame
                pf -= 2;
                *pf = 0;
                *(pf + 1) = 0;
                ALOGI("a frame ready...");
                post_frame(nat, frame, pf - frame);
                pf = frame;
                return;
            }

            // tail: 0x0d
            if (p[size - 1] == 0x0d) {
                // a whole frame, post the frame
                pf -= 1;
                *pf = 0;
                ALOGI("a frame ready...");
                post_frame(nat, frame, pf - frame);
                pf = frame;
                return;
            }
        }

        // ALOGI("PART...[%s]", frame);
    }
}

static int epoll_register( int  epoll_fd, int  fd )
{
    struct epoll_event  ev;
    int                 ret, flags;

    /* important: make the fd non-blocking */
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    ev.events  = EPOLLIN;
    ev.data.fd = fd;
    do {
        ret = epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &ev );
    } while (ret < 0 && errno == EINTR);
    return ret;
}


static int epoll_deregister( int  epoll_fd, int  fd )
{
    int  ret;
    do {
        ret = epoll_ctl( epoll_fd, EPOLL_CTL_DEL, fd, NULL );
    } while (ret < 0 && errno == EINTR);
    return ret;
}


static void scanner_jni_client_thread(void *arg)
{
    char buf[1024];
    struct scanner_jni_native_data *nat = (struct scanner_jni_native_data *)arg;

    int         control_fd = nat->control[1];
    int         epoll_fd   = epoll_create(2);

    nat->e = AndroidRuntime::getJNIEnv();

    struct termios  ios;
    int scanner_fd = open(devpath, O_RDWR | O_NOCTTY);
    if (scanner_fd == -1) {
        ALOGE("open error. %s\n", strerror(errno));
        return;
    }

    if (!isatty(scanner_fd)) {
        ALOGE("not a tty ?\n");
        close(scanner_fd);
        return;
    }

    tcflush(scanner_fd, TCIFLUSH);

    tcgetattr( scanner_fd, &ios );
    cfmakeraw(&ios);
    ios.c_iflag &= ~ICRNL;
    ios.c_lflag = 0;
    ios.c_cc[VTIME] = 150; /* Set overtime value 15 seconds*/
    ios.c_cc[VMIN] = 0; /* Update the options and do it NOW */
    if (tcsetattr(scanner_fd, TCSANOW, &ios) != 0) {
        ALOGE("SetupSerial xxxxxx");
        return;
    }

#if SERIAL_READ
    send_cmd_serial(scanner_fd);
#endif

    ALOGI("Version 1.0. Ready ...\n");

    epoll_register( epoll_fd, control_fd );
    epoll_register( epoll_fd, scanner_fd );

#define CMD_KEYEVT           TIOCPKT // 0x5420
    // disable report keyevent
    {
        int r;
        r = ioctl(scanner_fd, CMD_KEYEVT, 0);
        ALOGI("CMD_KEYEVT 0, r=%d, err=%s\n", r, strerror(errno));
    }

    while (true) {
        struct epoll_event   events[2];
        int                  ne, nevents;

        nevents = epoll_wait( epoll_fd, events, 2, -1 );
        if (nevents < 0) {
            if (errno != EINTR)
                ALOGE("epoll_wait() unexpected error: %s", strerror(errno));
            continue;
        }

        for (ne = 0; ne < nevents; ne++) {
            if ((events[ne].events & (EPOLLERR|EPOLLHUP)) != 0) {
                LOGE("EPOLLERR or EPOLLHUP after epoll_wait() !?");
                return;
            }
            if ((events[ne].events & EPOLLIN) != 0) {
                int  fd = events[ne].data.fd;

                if (fd == control_fd) {
                    char  cmd = 255;
                    int   ret;
                    ALOGD("scanner control fd event");  
                    do {
                        ret = read( fd, &cmd, 1 );
                    } while (ret < 0 && errno == EINTR);

                    if (cmd == CMD_QUIT) {
                        LOGI("scanner thread quitting on demand");
                        goto exit_thread;
                    }
                    else {
                        LOGI("scanner unknown cmd: %c", cmd);
                    }
                }
                else if (fd == scanner_fd) {
                    int l = read(fd, buf, sizeof(buf));
                    ALOGD("read l: %d\n", l);
                    if (l > 0) {
                        buf[l] = 0;
#if 0  
                        {
                            char dump[2048];
                            char *p = dump;
                            int i;
                            for (i=0; i<l; i++)  p += sprintf(p, "%02x ", buf[i]);
                            *p = 0;
                            ALOGI("DUMP [%s]", dump);
                        }
#endif
                        parse_data(nat, buf, l);
                    }
                }
                else {
                    LOGE("epoll_wait() returned unkown fd %d ?", fd);
                }
            }
        }
    }
exit_thread:
    // enable report keyevent
    ioctl(scanner_fd, CMD_KEYEVT, 1);

    close(scanner_fd);
    close(nat->control[0]);
    close(nat->control[1]);

    nat->control[0] = nat->control[1] = -1;

    ALOGE("\n\nh %s() l %d QUIT xxxxxxxxxxxx \n", __FUNCTION__, __LINE__);

    return;
}


static jint scanner_start(JNIEnv *e, jobject o)
{
    ALOGI("%s()", __FUNCTION__);

    ScopedLocalRef<jclass> cls(e, e->GetObjectClass(o));
    jfieldID f = e->GetFieldID(cls.get(), "mNative", "J");

    struct scanner_jni_native_data *nat = (struct scanner_jni_native_data *)e->GetLongField(o, f);
    if (!nat) {
        nat = new struct scanner_jni_native_data;
        e->SetLongField(o, f, (jlong)nat);
    }
    nat->e = e;
    nat->o = e->NewGlobalRef(o);
    e->GetJavaVM(&(nat->vm));

    jmethodID messageMe = e->GetMethodID(cls.get(), "messageMe", "(Ljava/lang/String;)V");
    if (!messageMe) {
        ALOGI("messageMe is NULL");
        return -1;
    }
    nat->cb = messageMe;

    if ( socketpair( AF_LOCAL, SOCK_STREAM, 0, nat->control ) < 0 ) {
        LOGE("could not create thread control socket pair: %s", strerror(errno));
        goto clean_and_return;
    }

    nat->thread = (pthread_t)AndroidRuntime::createJavaThread("scanner", scanner_jni_client_thread, nat);

    return 0;
clean_and_return:
    delete nat;
    return -1;
}

static void scanner_stop(JNIEnv *e, jobject o)
{
    ALOGI("%s()", __FUNCTION__);

    ScopedLocalRef<jclass> cls(e, e->GetObjectClass(o));
    jfieldID f = e->GetFieldID(cls.get(), "mNative", "J");
    struct scanner_jni_native_data *nat = (struct scanner_jni_native_data *)e->GetLongField(o, f);

    if (nat) {
        char   cmd = CMD_QUIT;
        void*  dummy;
        write( nat->control[0], &cmd, 1 );
        if (nat->thread) {
            pthread_join(nat->thread, &dummy);
            nat->thread = 0;
            ALOGI("send quit ... 111");
        }
    }
    else {
        ALOGI("send quit ... 000");
    }

}

static void scanner_poweron_trigger(JNIEnv *e, jobject o)
{
    ALOGI("%s()", __FUNCTION__);
    //fengmc add stopbarcodeservice 180123
    system("/system/bin/scanner_begin.sh stopbarcodeservice");
    //end
    
}

static void scanner_poweroff(JNIEnv *e, jobject o)
{
    ALOGI("%s()", __FUNCTION__);
    //fengmc add restartbarcodeservice 180123
    system("/system/bin/scanner_end.sh restartbarcodeservice"); 
    //end
}


static void scanner_uninit(JNIEnv *e, jobject o)
{
    ALOGI("%s()", __FUNCTION__);

    ScopedLocalRef<jclass> cls(e, e->GetObjectClass(o));
    jfieldID f = e->GetFieldID(cls.get(), "mNative", "J");
    struct scanner_jni_native_data *nat = (struct scanner_jni_native_data *)e->GetLongField(o, f);

    scanner_poweroff(e, o);

    if (nat) {
        scanner_stop(e, o);
        delete nat;
        nat = NULL;
        e->SetLongField(o, f, (jlong)nat);
    }
}

static JNINativeMethod methods[] = {
    {"start", "()I", (void *)scanner_start},
    {"stop", "()V", (void *)scanner_stop},
    {"poweron_trigger", "()V", (void *)scanner_poweron_trigger},
    {"poweroff", "()V", (void *)scanner_poweroff},
    {"uninit", "()V", (void *)scanner_uninit},
    
};


typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    ALOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    ALOGI("registerNatives");

    jniRegisterNativeMethods(env, classPathName, methods, NELEM(methods));

    result = JNI_VERSION_1_6;
bail:
    return result;
}

