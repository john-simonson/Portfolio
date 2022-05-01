#pragma once
//includes
#include<iostream>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<cstring>
#include<vector>
#include<fstream>
#include<algorithm>
// using declarations
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;

//Mononey code
#define NUMBER_ENTRIES  (1001)
#define FALSE           (0)
#define TRUE            (1)
#define DONE            (2)

struct request{
        int is_req;
        int is_allocated;
        int size;
	int match_alloc;
        int base_adr;
        int next_boundary_adr;
        int memory_left;
        int largest_chunk;
        int elements_on_free_list;
}req_array[NUMBER_ENTRIES];

struct free_list{
        struct free_list *next;
        struct free_list *previous;
        int block_size;
        int block_adr;
        int adjacent_adr;
}list_head, *top;

int total_free_space;
int total_free;

int allocate_memory(struct request *);
int update_list(int);

struct lel{
        struct lel *next;
        struct lel *prev;
        int adr;
        int bud_adr;
        int bit;
};

struct lh{
        struct lel *head;
        int cnt;
};

struct requests{
        int is_req;
        int is_allocated;
        int size;
        int act_size;
        int base_adr;
        int next_boundary_adr;
        int memory_left;
        int largest_chunk;
        int elements_on_free_list;
        struct lel *this_req;
}req_array_buddy[NUMBER_ENTRIES];

struct lh lst_ary[21];

struct lel *find_el(int bit);
int update_lists(struct lel *el);



