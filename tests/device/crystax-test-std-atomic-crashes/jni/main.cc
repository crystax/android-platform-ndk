#include <atomic>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <stdio.h>

std::atomic<long> g_shared_counter(0);
std::atomic<bool> g_stop(false);

#define MAX_THREADS  2

int g_thread_counts[MAX_THREADS];
pthread_t g_threads[MAX_THREADS];

void* thread_main(void* arg) {
  int *thread_counts = static_cast<int*>(arg);

  while (!g_stop) {
      for (size_t nn = 0; nn < 10000; ++nn) {
        // Atomic increment here.
        g_shared_counter++;

        // Non-atomic increment, but only for debugging.
        *thread_counts += 1;
      }
  }
  return NULL;
}

#define MAX_TIME       60
#define CHECK_INTERVAL 2

int main(void) {
    int num_threads = MAX_THREADS;

    g_shared_counter = 0;
    g_stop = false;

    for (int i = 0; i < num_threads; ++i) {
        g_thread_counts[i] = 0;
        pthread_create(&g_threads[i], NULL, &thread_main, reinterpret_cast<void*>(&g_thread_counts[i]));
    }

    time_t start_time = time(NULL);
    time_t last_time = start_time;

    for (;;) {
        time_t now = time(NULL);

        if (now - start_time >= MAX_TIME)
            break;

        if (now - last_time > CHECK_INTERVAL) {
            char buffer[64];
            long atomic_count = g_shared_counter;
            std::string message("Thread updates: ");
            for (int i = 0; i < num_threads; ++i) {
                snprintf(buffer, sizeof(buffer), " [%3d %d]", i, g_thread_counts[i]);
                message.append(buffer);
                g_thread_counts[i] = 0;
            }
            printf("%s count=%ld\n", message.c_str(), atomic_count);
            last_time = now;
        }
        printf(".");
        fflush(stdout);
        sleep(1);
    }
    printf("Stopping threads\n");
    fflush(stdout);
    g_stop = true;
    for (int i = 0; i < num_threads; ++i) {
        void* dummy;
        pthread_join(g_threads[i], &dummy);
    }

    long atomic_count = g_shared_counter;
    printf("Final: %ld\n", atomic_count);

    return 0;
}
