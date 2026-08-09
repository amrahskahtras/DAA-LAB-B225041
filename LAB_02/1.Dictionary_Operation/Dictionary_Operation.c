#include <stdio.h>
#include <stdlib.h>

long ua_search(int *arr, int n, int key) {
    long steps = 0;
    for (int i = 0; i < n; i++) { steps++; if (arr[i] == key) return steps; }
    return steps;
}
long ua_insert(void) { return 1; }
long ua_delete(void) { return 1; }
long ua_max(int n) { return n; }
long ua_min(int n) { return n; }
long ua_pred_succ(int n) { return n; } 

/* ================= Sorted array ================= */
long sa_search(int *arr, int n, int key) {
    long steps = 0;
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        steps++;
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == key) return steps;
        else if (arr[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    return steps;
}
long sa_insert(int n) { return n + 1; }
long sa_delete(int n) { return n; }
long sa_max(void) { return 1; }
long sa_min(void) { return 1; }
long sa_pred_succ(void) { return 1; } 

typedef struct SNode { int key; struct SNode *next; } SNode;

long sl_search(SNode *head, int key) {
    long steps = 0;
    for (SNode *cur = head; cur; cur = cur->next) { steps++; if (cur->key == key) return steps; }
    return steps;
}
long sl_insert(void) { return 1; }
long sl_delete_via_pred_scan(SNode *head, SNode *target) {
    long steps = 0;
    if (head == target) return 1;
    for (SNode *cur = head; cur; cur = cur->next) {
        steps++;
        if (cur->next == target) return steps + 1;
    }
    return steps;
}
long sl_max(SNode *head) { long s=0; for (SNode *c=head;c;c=c->next) s++; return s; }
long sl_min(SNode *head) { long s=0; for (SNode *c=head;c;c=c->next) s++; return s; }
long sl_pred_succ_unsorted(SNode *head) { long s=0; for (SNode *c=head;c;c=c->next) s++; return s; }

long ss_search(SNode *head, int key) {
    long steps = 0;
    for (SNode *cur = head; cur; cur = cur->next) {
        steps++;
        if (cur->key == key) return steps;
        if (cur->key > key) return steps;
    }
    return steps;
}
long ss_insert(int n) { return n + 1; }
long ss_delete_via_pred_scan(SNode *head, SNode *target) {
    long steps = 0;
    if (head == target) return 1;
    for (SNode *cur = head; cur; cur = cur->next) {
        steps++;
        if (cur->next == target) return steps + 1;
    }
    return steps;
}
long ss_max(int n) { return n; }
long ss_min(void) { return 1; }        
long ss_successor(void) { return 1; }  
long ss_predecessor_worst(SNode *head, SNode *target) {/
    long steps = 0;
    for (SNode *cur = head; cur; cur = cur->next) {
        steps++;
        if (cur->next == target) return steps;
    }
    return steps;
}

typedef struct DNode { int key; struct DNode *prev, *next; } DNode;

long dl_search(DNode *head, int key) {
    long steps = 0;
    for (DNode *cur = head; cur; cur = cur->next) { steps++; if (cur->key == key) return steps; }
    return steps;
}
long dl_insert(void) { return 1; }
long dl_delete(void) { return 1; }
long dl_max(int n) { return n; }
long dl_min(int n) { return n; }
long dl_pred_succ_unsorted(int n) { return n; }

long ds_search(DNode *head, int key) {
    long steps = 0;
    for (DNode *cur = head; cur; cur = cur->next) {
        steps++;
        if (cur->key == key) return steps;
        if (cur->key > key) return steps;
    }
    return steps;
}
long ds_insert(int n) { return n + 1; }
long ds_delete(void) { return 1; }
long ds_max(void) { return 1; }        
long ds_min(void) { return 1; }        
long ds_pred_succ(void) { return 1; }  

int *build_array(int n) {
    int *a = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) a[i] = i;
    return a;
}
SNode *build_slist(int n) {
    SNode *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        SNode *node = malloc(sizeof(SNode));
        node->key = i; node->next = NULL;
        if (!head) head = tail = node; else { tail->next = node; tail = node; }
    }
    return head;
}
DNode *build_dlist(int n) {
    DNode *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        DNode *node = malloc(sizeof(DNode));
        node->key = i; node->prev = tail; node->next = NULL;
        if (!head) head = node; else tail->next = node;
        tail = node;
    }
    return head;
}
SNode *slist_tail(SNode *head) { SNode *c = head; while (c->next) c = c->next; return c; }
void free_slist(SNode *head) { while (head) { SNode *n = head->next; free(head); head = n; } }
void free_dlist(DNode *head) { while (head) { DNode *n = head->next; free(head); head = n; } }

int main(void) {
    FILE *csv = fopen("dict_data.csv", "w");
    if (!csv) { perror("fopen"); return 1; }
    fprintf(csv,
        "n,"
        "ua_search,sa_search,sl_search,ss_search,dl_search,ds_search,"
        "ua_insert,sa_insert,sl_insert,ss_insert,dl_insert,ds_insert,"
        "ua_delete,sa_delete,sl_delete,ss_delete,dl_delete,ds_delete,"
        "ua_max,sa_max,sl_max,ss_max,dl_max,ds_max,"
        "ua_min,sa_min,sl_min,ss_min,dl_min,ds_min,"
        "ua_pred,sa_pred,sl_pred,ss_pred,dl_pred,ds_pred,"
        "ua_succ,sa_succ,sl_succ,ss_succ,dl_succ,ds_succ\n");

    int sizes[] = {100, 200, 400, 800, 1600, 3200, 6400};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%6s | %8s %8s %8s %8s %8s %8s\n", "n", "ua_srch","sa_srch","sl_srch","ss_srch","dl_srch","ds_srch");

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];

        int *arr = build_array(n);
        SNode *slist = build_slist(n);
        DNode *dlist = build_dlist(n);
        int missing_key = n;
        SNode *s_tail = slist_tail(slist);

        long ua_s = ua_search(arr, n, missing_key);
        long sa_s = sa_search(arr, n, missing_key);
        long sl_s = sl_search(slist, missing_key);
        long ss_s = ss_search(slist, missing_key);
        long dl_s = dl_search(dlist, missing_key);
        long ds_s = ds_search(dlist, missing_key);

        long ua_i = ua_insert();
        long sa_i = sa_insert(n);
        long sl_i = sl_insert();
        long ss_i = ss_insert(n);
        long dl_i = dl_insert();
        long ds_i = ds_insert(n);

        long ua_d = ua_delete();
        long sa_d = sa_delete(n);
        long sl_d = sl_delete_via_pred_scan(slist, s_tail);
        long ss_d = ss_delete_via_pred_scan(slist, s_tail);
        long dl_d = dl_delete();
        long ds_d = ds_delete();

        long ua_mx = ua_max(n);
        long sa_mx = sa_max();
        long sl_mx = sl_max(slist);
        long ss_mx = ss_max(n);
        long dl_mx = dl_max(n);
        long ds_mx = ds_max();

        long ua_mn = ua_min(n);
        long sa_mn = sa_min();
        long sl_mn = sl_min(slist);
        long ss_mn = ss_min();
        long dl_mn = dl_min(n);
        long ds_mn = ds_min();

        long ua_p = ua_pred_succ(n);
        long sa_p = sa_pred_succ();
        long sl_p = sl_pred_succ_unsorted(slist);
        long ss_p = ss_predecessor_worst(slist, s_tail);
        long dl_p = dl_pred_succ_unsorted(n);
        long ds_p = ds_pred_succ();

        long ua_su = ua_pred_succ(n);
        long sa_su = sa_pred_succ();
        long sl_su = sl_pred_succ_unsorted(slist);
        long ss_su = ss_successor();
        long dl_su = dl_pred_succ_unsorted(n);
        long ds_su = ds_pred_succ();

        printf("%6d | %8ld %8ld %8ld %8ld %8ld %8ld\n", n, ua_s, sa_s, sl_s, ss_s, dl_s, ds_s);

        fprintf(csv, "%d,"
            "%ld,%ld,%ld,%ld,%ld,%ld,"
            "%ld,%ld,%ld,%ld,%ld,%ld,"
            "%ld,%ld,%ld,%ld,%ld,%ld,"
            "%ld,%ld,%ld,%ld,%ld,%ld,"
            "%ld,%ld,%ld,%ld,%ld,%ld,"
            "%ld,%ld,%ld,%ld,%ld,%ld,"
            "%ld,%ld,%ld,%ld,%ld,%ld\n",
            n,
            ua_s, sa_s, sl_s, ss_s, dl_s, ds_s,
            ua_i, sa_i, sl_i, ss_i, dl_i, ds_i,
            ua_d, sa_d, sl_d, ss_d, dl_d, ds_d,
            ua_mx, sa_mx, sl_mx, ss_mx, dl_mx, ds_mx,
            ua_mn, sa_mn, sl_mn, ss_mn, dl_mn, ds_mn,
            ua_p, sa_p, sl_p, ss_p, dl_p, ds_p,
            ua_su, sa_su, sl_su, ss_su, dl_su, ds_su);

        free(arr); free_slist(slist); free_dlist(dlist);
    }

    fclose(csv);
    printf("\nData written to dict_data.csv (all 7 operations)\n");
    return 0;
}