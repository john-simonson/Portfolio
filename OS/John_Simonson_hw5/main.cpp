#include"main.hpp"
//Policy functions
int best(int, string);
int first(int, string);
//int buddy(int, string);

int main(int argc, char *argv[]){

    if( argc != 4){
        cout << "Error: invalid args" << endl;
        cout << "Args must use format: (policy) (total memory pool size) (input file)" << endl;
    }
    string policy = argv[1]; // policy type
    int mem = atoi(argv[2]); // memory
    string in_file_name = argv[3]; // in file

    if(policy == "best"){
        best(mem, in_file_name);
    }
    else if(policy == "first"){
        first(mem, in_file_name);
    }
    else{
        cout << "ERROR: invalid policy arg" << endl;
        exit(1);
    }
     /*if(policy == "buddy"){
        buddy(mem, in_file_name);
    }*/

    return 0;

}

int update_list(int index)
{
    struct free_list* fList;
    struct free_list* new_block;
    struct free_list* combine;

    if(req_array[index].is_allocated == FALSE)
    {
        return 0;
    }

    req_array[index].is_allocated = DONE;
    total_free += req_array[index].size;

    for(fList = list_head.next; fList; fList = fList->next)
    {

        if(req_array[index].base_adr > fList -> block_adr)
        {
            continue;
        }

        new_block = (free_list*)malloc(sizeof(struct free_list));
        new_block->block_size = req_array[index].size;
        new_block->block_adr = req_array[index].base_adr;
        new_block->adjacent_adr = new_block->block_adr + new_block->block_size;

        new_block->next = fList;
        fList->previous->next = new_block;
        new_block->previous = fList->previous;
        fList->previous = new_block;

        if(new_block->adjacent_adr == new_block->next->block_adr)
        {
            combine = new_block->next;
            new_block->block_size += combine->block_size;
            new_block->adjacent_adr = combine->adjacent_adr;
            new_block->next = combine->next;

            if(new_block->next)
            {
                new_block->next->previous = new_block;
            }


        }

        new_block = new_block->previous;
        if((new_block != NULL) && (new_block->adjacent_adr != 0))
        {
            if(new_block->adjacent_adr == new_block->next->block_adr)
            {
                combine = new_block->next;
                new_block->block_size += combine->block_size;
                new_block->adjacent_adr = combine->adjacent_adr;
                new_block->next = combine->next;

                if(new_block->next)
                {
                    new_block->next->previous = new_block;
                }

            }
        }

        return 0;

    }
    return 0;
}

int allocate_memory_best(struct request* request)
{
    struct free_list* fList = NULL;
    struct free_list* vList = NULL;

    int size_diff = 0;
    int leftover = 0;

    int flag = TRUE;

    for(fList = list_head.next; fList; fList = fList->next)
    {
        if(request->size <= fList->block_size)
        {
            size_diff = fList->block_size - request->size;

            if(flag)
            {
                leftover = size_diff;
                vList = fList;
                flag = FALSE;
            }

            if (leftover > size_diff)
            {
                leftover = size_diff;
                vList = fList;
            }

        }
    }

    if(vList != NULL)
    {
        request->is_allocated = TRUE;
        request->base_adr = vList->block_adr;
        request->next_boundary_adr = request->base_adr + request->size;

        total_free -= request->size;
        request->memory_left = total_free;

        if((vList->block_size -= request->size) == 0)
        {
        
            vList->previous->next = vList->next;
            vList->next->previous = vList->previous;

            free(vList);
            return 0;
        }

        vList->block_adr = vList->block_adr + request->size;
        return 0;

    }

    request->memory_left = total_free;
    return 0;

}


int best(int mem, string in_file_name)
{

    int i = 0;
    int seq_num = 0;       //serial num
    int type_val = 0;      //val
    string type;           // type of policy

    //open file
    ifstream fp;
    fp.open(in_file_name);

    struct free_list* p;

    total_free_space = (mem * 1024);
    total_free = (mem * 1024);

    for(i = 0; i < NUMBER_ENTRIES; i++)
    {
        req_array[i].is_req = FALSE;
        req_array[i].is_allocated = FALSE;
    }

    top = (free_list*)malloc(sizeof(struct free_list));
    top -> next = NULL;
    top -> previous = &list_head;
    top -> block_size = total_free_space;
    top -> block_adr = 0;
    top -> adjacent_adr = total_free_space;

    list_head.next = top;
    list_head.previous = NULL;
    list_head.block_size = -1;
    list_head.block_adr = -1;
    list_head.adjacent_adr = 0;

    while(!fp.eof())
    {
        fp >> seq_num;
        fp >> type;
        fp >> type_val;

    //moloney code
          if(type == "alloc"){
        req_array[seq_num].is_req = TRUE;
        req_array[seq_num].size = type_val;
        allocate_memory_best(&req_array[seq_num]);
        req_array[seq_num].elements_on_free_list = 0;
        req_array[seq_num].largest_chunk = 0;
        for(p=list_head.next; p; p=p->next){
            ++req_array[seq_num].elements_on_free_list;
            if(p->block_size > req_array[seq_num].largest_chunk){
              req_array[seq_num].largest_chunk = p->block_size;
            }
        }

      }else{
        req_array[seq_num].size = req_array[type_val].size;
        req_array[seq_num].is_allocated = req_array[type_val].is_allocated;
        update_list(type_val);
        req_array[seq_num].memory_left = total_free;
        req_array[seq_num].elements_on_free_list = 0;
        req_array[seq_num].largest_chunk = 0;
        for(p=list_head.next; p; p=p->next){
            ++req_array[seq_num].elements_on_free_list;
            if(p->block_size > req_array[seq_num].largest_chunk){
              req_array[seq_num].largest_chunk = p->block_size;
            }
        }
        // initialize match_alloc
        req_array[seq_num].match_alloc = type_val;
       }
}   

    //print
    cout << "MANAGEMENT POLICY: Best\tPOOL SIZE " << mem << " KB" << endl;
    
    int k = 1;
    int alloc_count = 0;
    int fails = 0;
    char function[6];
    vector<int> fail_index;

    for(k = 1; k < NUMBER_ENTRIES; k++)
    {
        if(req_array[k].is_req == 1){
            alloc_count++;
        }

        if(req_array[k].is_allocated == FALSE)
        {
        if(req_array[k].is_req != 0){
            cout << "Failed Req " << k << " is size " << req_array[k].size << endl;
        }
            req_array[k].base_adr = -1;
            fails++;
        }

    }

    cout << endl << "TOTAL ALLOCATIONS: " << alloc_count - (fails / 2) << endl << endl;
    cout << "Seq #\tType\tSize/Rq#\tDone?\tFree Total\tFree Elems\tLargest Chunk" << endl;


    for(k = 1; k < NUMBER_ENTRIES; k++)
    {
        if(k%20 == 0){
            cout << endl << "Seq #\tType\tSize/Rq#\tDone?\tFree Total\tFree Elems\tLargest Chunk" << endl;
        }
        cout << k << "\t" << ((req_array[k].is_req) ? "alloc" : "free") << "\t";
        cout << ((req_array[k].is_req) ? req_array[k].size : req_array[k].match_alloc) << "\t" ;
        for(int j = 0; j < fail_index.size(); j++){
            if(fail_index[j] == req_array[k].size && (req_array[k].is_req) == 0){
                cout << "\tNO\t";
            }
        }
        if(req_array[k].is_allocated == FALSE){
            cout << "\tNO\t";
            fail_index.push_back(i);
        }
        else{
            cout << "\tYES\t";
        }
        cout << req_array[k].memory_left <<"\t\t";
        cout << req_array[k].elements_on_free_list << "\t\t" << req_array[k].largest_chunk << endl;

    }

return 0;
}

int allocate_memory_first(struct request* request)
{
    struct free_list* fList = NULL;

    for(fList = list_head.next; fList; fList = fList->next)
    {
        if(request->size <= fList->block_size)
        {

            request->is_allocated = TRUE;
            request->base_adr = fList->block_adr;
            request->next_boundary_adr = request->base_adr + request->size;

            total_free -= request->size;
            request->memory_left = total_free;

            //handle perfect match
            if((fList->block_size -= request->size) == 0)
            {
                //skip the valid block, shift list
                fList->previous->next = fList->next;
                fList->next->previous = fList->previous;
                return 0;
            }

            fList->block_adr += request->size;
            return 0;

        }
    }
    //spot was not found
    request->memory_left = total_free;
    return 0;

}

int first(int mem, string in_file_name)
{

    int i = 0;
    int seq_num = 0;     
    int type_val = 0;      
    string type;     

    ifstream fp;
    fp.open(in_file_name);

    struct free_list* p;

    total_free_space = (mem * 1024);
    total_free = (mem * 1024);

    for(i = 0; i < NUMBER_ENTRIES; i++)
    {
        req_array[i].is_req = FALSE;
        req_array[i].is_allocated = FALSE;
    }

    top = (free_list*)malloc(sizeof(struct free_list));
    top -> next = NULL;
    top -> previous = &list_head;
    top -> block_size = total_free_space;
    top -> block_adr = 0;
    top -> adjacent_adr = total_free_space;

    list_head.next = top;
    list_head.previous = NULL;
    list_head.block_size = -1;
    list_head.block_adr = -1;
    list_head.adjacent_adr = 0;

    while(!fp.eof())
    {
        fp >> seq_num;
        fp >> type;
        fp >> type_val;

          if(type == "alloc"){
        req_array[seq_num].is_req = TRUE;
        req_array[seq_num].size = type_val;
        allocate_memory_first(&req_array[seq_num]);
        req_array[seq_num].elements_on_free_list = 0;
        req_array[seq_num].largest_chunk = 0;
        for(p=list_head.next; p; p=p->next){
            ++req_array[seq_num].elements_on_free_list;
            if(p->block_size > req_array[seq_num].largest_chunk){
              req_array[seq_num].largest_chunk = p->block_size;
            }
        }

      }else{
        req_array[seq_num].size = req_array[type_val].size;
        req_array[seq_num].is_allocated = req_array[type_val].is_allocated;
        update_list(type_val);
        req_array[seq_num].memory_left = total_free;
        req_array[seq_num].elements_on_free_list = 0;
        req_array[seq_num].largest_chunk = 0;
        for(p=list_head.next; p; p=p->next){
            ++req_array[seq_num].elements_on_free_list;
            if(p->block_size > req_array[seq_num].largest_chunk){
              req_array[seq_num].largest_chunk = p->block_size;
            }
        }
        req_array[seq_num].match_alloc = type_val;
       }
}   

    //print
    cout << "MANAGEMENT POLICY: First\tPOOL SIZE " << mem << " KB" << endl;
    
    int k = 1;
    int alloc_count = 0;
    int fails = 0;
    char function[6];
    vector<int> fail_index;

    for(k = 1; k < NUMBER_ENTRIES; k++)
    {
        if(req_array[k].is_req == 1){
            alloc_count++;
        }

        if(req_array[k].is_allocated == FALSE)
        {
            if(req_array[k].is_req != 0){
                cout << "Failed Req " << k << " is size " << req_array[k].size << endl;
            }
            req_array[k].base_adr = -1;
            fails++;
        }

    }

    cout << endl << "TOTAL ALLOCATIONS: " << alloc_count - (fails / 2) << endl << endl;
    cout << "Seq #\tType\tSize/Rq#\tDone?\tFree Total\tFree Elems\tLargest Chunk" << endl;


    for(k = 1; k < NUMBER_ENTRIES; k++)
    {
        if(k%20 == 0){
            cout << endl << "Seq #\tType\tSize/Rq#\tDone?\tFree Total\tFree Elems\tLargest Chunk" << endl;
        }
        cout << k << "\t" << ((req_array[k].is_req) ? "alloc" : "free") << "\t";
        cout << ((req_array[k].is_req) ? req_array[k].size : req_array[k].match_alloc) << "\t" ;
        for(int j = 0; j < fail_index.size(); j++){
            if(fail_index[j] == req_array[k].size && (req_array[k].is_req) == 0){
                cout << "\tNO\t";
            }
        }
        if(req_array[k].is_allocated == FALSE){
            cout << "\tNO\t";
            fail_index.push_back(i);
        }
        else{
            cout << "\tYES\t";
        }
        cout << req_array[k].memory_left <<"\t\t";
        cout << req_array[k].elements_on_free_list << "\t\t" << req_array[k].largest_chunk << endl;

    }

return 0;
}
