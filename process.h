typedef struct process
{
	pid_t pid;
	char* path;
	struct process* next;
}
process_t;

void print_process_list(process_t* head);
void push(process_t* head, pid_t pid, char* path);
pid_t remove_by_value(process_t** head, pid_t pid);