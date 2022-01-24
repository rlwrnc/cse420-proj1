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

void populate_list(char *path, dll *list)   // see print_structure for inspiration
{
    static int level = 1;
    if (level == 1) {                                           //for first level case
        insert_sorted(create_node(path, level), list);
        level++;
    }
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
        insert_sorted(create_node(tmp, level), list);
        if (S_ISDIR(buf.st_mode)) {                             //if the file is a directory, call the function one directory deeper
            level++;
            populate_list(tmp, list);
            level--;
        }
    }
    closedir(ds);
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
    int order;
    dnode *curr = list->head;
    while (curr != NULL) {
        if (curr->prev != NULL && curr->level == curr->prev->level)
            order++;
        else
            order = 1;
        printf("%d:%d:%s\n", curr->level, order, curr->path);
        curr = curr->next;
    }
}

void print_list_to_file(dll *list, char *filename)
{
    int order;
    dnode *curr = list->head;
    FILE *fs = fopen(filename, "w");
    while (curr != NULL) {
        if (curr->prev != NULL && curr->level == curr->prev->level)
            order++;
        else
            order = 1;
        fprintf(fs, "%d:%d:%s\n", curr->level, order, curr->path);
        curr = curr->next;
    }
    fclose(fs); 
}

/* sorting algorithms */

void insertion_sort_by_level_increasing(dll *list)
{
    dnode *fi = list->head->next, *bi, *tmp;
    int key;
    while (fi != NULL) {
        key = fi->level;
        bi = fi;
        while (bi->prev != NULL && bi->prev->level >= key)
            bi = bi->prev;
        tmp = fi;
        fi = fi->next;
        if (bi != tmp) {
            if (tmp == list->tail)
                list->tail = tmp->prev;
            else
                tmp->next->prev = tmp->prev;
            tmp->prev->next = tmp->next;

            tmp->next = bi;
            tmp->prev = bi->prev;
            if (bi == list->head)
                list->head = tmp;
            else
                bi->prev->next = tmp;
            bi->prev = tmp;
        }
    }
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
    populate_list(dirpath, dirlist);
    insertion_sort_by_level_increasing(dirlist);
    print_list_to_file(dirlist, outfile);
    destroy_list(dirlist);
    return 0;
}
