#ifndef SUPPORT_LIBCXXRT_PTHREAD_H
#define SUPPORT_LIBCXXRT_PTHREAD_H

typedef int pthread_key_t;
typedef int pthread_once_t;
typedef int pthread_mutex_t;
typedef int pthread_cond_t;
typedef int pthread_mutexattr_t;

int pthread_key_create(pthread_key_t* key, void (*)(void*));
int pthread_once(pthread_once_t* control, void (*init)(void));
void* pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void* data);
int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t*);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);
int pthread_cond_wait(pthread_cond_t*, pthread_mutex_t*);
int pthread_cond_signal(pthread_cond_t*);

int sched_yield(void) {
  return 0;
}

#define PTHREAD_ONCE_INIT (-1)
#define PTHREAD_COND_INITIALIZER (-1)
#define PTHREAD_MUTEX_INITIALIZER (-1)

#endif // SUPPORT_LIBCXXRT_PTHREAD_H