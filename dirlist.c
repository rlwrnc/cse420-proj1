#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
/* Linked List implementation w/ subroutines */

typedef struct dnode dnode;
struct dnode {
    char *path;             //absolute path of directory
    int level;              //level of path relative to head
    dnode *next;            //next node in list
    dnode *prev;            //previous node in list
};                          //directory node

typedef struct dll dll;
struct dll {
    dnode *head;    //points to first node
    dnode *tail;    //points to last node
    int length;     //number of nodes
};                  //doubly linked list

//creation subroutines

dnode *create_node(char *path, int level) 
{
    dnode *node = malloc(sizeof(dnode));
    if (node == NULL) {
        fprintf(stderr, "%s: Couldn't create memory for the list; %s\n", "dirlist", strerror(errno));
        exit(-1);
    }
    node->path = strdup(path);
    node->level = level;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

dll *create_list() {
    dll *list = malloc(sizeof(dll));
    if (list == NULL) {
        fprintf(stderr, "%s: Couldn't create memory for the list; %s\n", "dirlist", strerror(errno));
        exit(-1);
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

//additions

void insert_tail(dnode *node, dll *list)
{
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
        list->tail = node;
    }
    list->length++;
}

void insert_sorted(dnode *node, dll *list)
{
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else if (strcmp(node->path, list->head->path) <= 0) {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    } else if (strcmp(node->path, list->head->path) > 0) {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    } else {
        dnode *ptr = list->head;
        while (ptr != NULL) {
            if (strcmp(node->path, ptr->path) <= 0) {
                node->next = ptr;
                node->prev = ptr->prev;
                ptr->prev->next = node;
                ptr->prev = node;
            }
            ptr = ptr->next;
        }
    }
}

/*
 *   populate_list
 *   populates a linked list with the contents of a supplied directory
 */

void populate_list(dll *list, char *path)   // see print_structure for inspiration
{
    DIR *ds = opendir(path);
    char tmp[255];
    struct dirent *d;
    struct stat buf;
    while ((d = readdir(ds)) != NULL) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;
    }
}

//destroy

void destroy_list(dll *list)
{
    dnode *curr = list->head;
    dnode *tmp;
    while (curr != NULL) {
        free(curr->path);
        tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(list);
}

//print

void print_list(dll *list)
{
    dnode *curr = list->head;
    while (curr != NULL) {
        if (curr != list->head)
            printf("->");
        printf("(%s, %d)", curr->path, curr->level);
        curr = curr->next;
    }
    printf("\nThe list contains %d elements.\n", list->length);
}

/* sorting algorithms */

void print_structure(char *path)
{
    DIR *ds = opendir(path);
    char tmp[255];
    struct dirent *d;
    struct stat buf;
    while ((d = readdir(ds)) != NULL) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;
        strcpy(tmp, path);
        strcat(tmp, "/");
        strcat(tmp, d->d_name);
        stat(tmp, &buf);
        if (S_ISDIR(buf.st_mode)) 
            print_structure(tmp);
        printf("%s\n", tmp);
    }
    closedir(ds);
}


/* main */

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: dirlist directory_path output_file\n");
        return -1;
    }

    char *dirpath = argv[1];
    char *outfile = argv[2];
    dll *dirlist = create_list();
    return 0;
}
