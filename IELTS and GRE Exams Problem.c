#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdbool.h>

#define TOTAL_STUDENTS 300
#define ROOM_CAPACITY  30

typedef struct {
    int room_of_student[TOTAL_STUDENTS];
    int rooms;
} Shared;

static sem_t exam_start;
static sem_t *room_sem = NULL;
static pthread_mutex_t att_mtx = PTHREAD_MUTEX_INITIALIZER;
static int *room_attendance = NULL;
static int total_entered = 0;

static pthread_mutex_t exam_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  exam_over_cv = PTHREAD_COND_INITIALIZER;
static bool exam_over = false;

typedef struct {
    int id;
    int room;
} StudentArg;

typedef struct {
    int num_students;
    int duration_s;
    int start_delay_s;
} ControllerCfg;

// Assign students to rooms
static void allocator_child(Shared *sh, int N, int CAP) {
    int R = (N + CAP - 1) / CAP;
    sh->rooms = R;

    for (int i = 0; i < N; i++) {
        sh->room_of_student[i] = i / CAP;
    }

    for (int r = 1; r <= R; r++) {
        int start_id = (r - 1) * CAP + 1;
        int end_id   = r * CAP; if (end_id > N) end_id = N;
        printf("Students %d - %d -> Room %d\n", start_id, end_id, r);
    }
    _exit(0);
}

// Each student thread
static void *student_thread(void *vp) {
    StudentArg *arg = (StudentArg*)vp;
    int room = arg->room;
    free(arg);

    sem_wait(&exam_start);
    sem_wait(&room_sem[room]);

    pthread_mutex_lock(&att_mtx);
    room_attendance[room]++;
    total_entered++;
    pthread_mutex_unlock(&att_mtx);

    pthread_mutex_lock(&exam_mtx);
    while (!exam_over) pthread_cond_wait(&exam_over_cv, &exam_mtx);
    pthread_mutex_unlock(&exam_mtx);

    sem_post(&room_sem[room]);
    return NULL;
}

// Control exam timing
static void *controller_thread(void *vp) {
    ControllerCfg *cfg = (ControllerCfg*)vp;
    printf("Exam starts in %d second(s)...\n", cfg->start_delay_s);
    sleep(cfg->start_delay_s);
    for (int i = 0; i < cfg->num_students; i++) sem_post(&exam_start);
    printf(">>> Exam STARTED\n");

    sleep(cfg->duration_s);

    pthread_mutex_lock(&exam_mtx);
    exam_over = true;
    pthread_cond_broadcast(&exam_over_cv);
    pthread_mutex_unlock(&exam_mtx);
    printf(">>> Exam ENDED\n");

    return NULL;
}

int main(int argc, char **argv) {
    const int N   = TOTAL_STUDENTS;
    const int CAP = ROOM_CAPACITY;

    int duration_s = 5, start_delay_s = 1;

    Shared *sh = mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (fork() == 0) allocator_child(sh, N, CAP);

    int st;
    wait(&st);
    int rooms = sh->rooms;

    sem_init(&exam_start, 0, 0);

    room_sem = calloc(rooms, sizeof(sem_t));
    room_attendance = calloc(rooms, sizeof(int));

    for (int r = 0; r < rooms; r++) sem_init(&room_sem[r], 0, CAP);

    pthread_t ctrl_tid;
    ControllerCfg cfg = { N, duration_s, start_delay_s };
    pthread_create(&ctrl_tid, NULL, controller_thread, &cfg);

    pthread_t *tids = malloc(sizeof(pthread_t) * N);
    for (int i = 0; i < N; i++) {
        StudentArg *arg = malloc(sizeof(StudentArg));
        arg->id = i;
        arg->room = sh->room_of_student[i];
        pthread_create(&tids[i], NULL, student_thread, arg);
    }

    for (int i = 0; i < N; i++) pthread_join(tids[i], NULL);
    pthread_join(ctrl_tid, NULL);

    printf("\n=== Attendance Summary ===\n");
    for (int r = 0; r < rooms; r++) {
        printf("Room %2d: %3d / %3d\n", r+1, room_attendance[r], CAP);
    }
    printf("TOTAL: %d / %d\n", total_entered, N);

    for (int r = 0; r < rooms; r++) sem_destroy(&room_sem[r]);
    sem_destroy(&exam_start);

    munmap(sh, sizeof(Shared));
    free(room_sem);
    free(room_attendance);
    free(tids);

    return 0;
}


