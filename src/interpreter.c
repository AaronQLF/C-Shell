#include <sys/stat.h> //new included library for stat
#include <unistd.h> //new included library for chdir
#include <stdio.h>
#include "shell.h"
#include <stdlib.h>
#include "shellmemory.h"
#include <string.h> 
#include <stdbool.h>
typedef struct pcb pcb;
#define MAX_PROCESSES 100
struct pcb {
    int pid;        // process ID
    int start_pos;  // start position of script instructions in Shell memory
    int curr_inst;  // current instruction to execute (program counter)
	int length;		//length of the script
	int age;
	char* name;};
	char* name;
    // other fields to store information about the process and its state
};
//fixing pull problem in here
struct node {
    pcb pcb;
    struct node* next;};

struct ready_queue {
    pcb queue[MAX_PROCESSES];  // Array of PCB pointers
    int size;} ready_queue;
struct ready_queue* readyQ;
void sort_ready_queue(struct ready_queue * ReadyQueue);
void addToReadyQueue(pcb PCB);
typedef struct node node;
struct node* head = NULL;  // global pointer variable to the head of the linked list
int MAX_ARGS_SIZE = 7;
int runSJFAging();
void sort_ready_queue();
int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;}
int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 2;}
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;}
int badcommandMkdir(){
	printf("%s\n", "Bad command: my_mkdir");
	return 4;}
int badcommandCd(){
	printf("%s\n", "Bad command: my_cd");
	return 5;}
int badCommandFileIsEmpty(){
	printf("%s\n", "Bad command: File is empty");
	return 6;}
int help();
int runRR(int slice);
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int echo(char* var);
int my_ls();
int my_mkdir(char* dirname);
int my_touch(char* filename);
int my_cd(char* dirname);
int scheduler();
int helper(char* line);
void addToEnd(pcb PCB);
int exec(char** directory, int args);
pcb makePCB(char* file_content,int value,char* name,int valuee);
off_t get_file_length(const char* filename);
void sort_filenames_by_length(char** filenames, int num_files);
bool isaPolicy(char* policy);
int count_lines(char* str);
int count_lines(char* str) {
    int num_lines = 1; // start at 1 since there's always at least one line
    char* ptr = str;
    while (*ptr != '\0') {
        if (*ptr == '\n') {
            num_lines++;
        }
        ptr++;
    }
    return num_lines-1;}
pcb makePCB(char* file_content,int value,char* name,int valuee){
	pcb PCB;
	PCB.pid = rand() % 1000;
	PCB.curr_inst = 0;
	PCB.start_pos=value;
	PCB.length=count_lines(file_content);
	PCB.name=name;
	if (valuee){
	if (head == NULL){
		head = (struct node*) malloc(sizeof(struct node));
		head->pcb = PCB;
		head->next = NULL;
		return PCB;
	}
	else {
		struct node* temp = head;
		while (temp->next != NULL){
			temp = temp->next;
		}
		temp->next = (struct node*) malloc(sizeof(struct node));
		temp->next->pcb = PCB;
		temp->next->next = NULL;
		return PCB;
	}
	}
	return PCB;}
int interpreter(char* command_args[], int args_size){
	if (args_size < 1){
		return badcommand();
	}

	if (args_size > MAX_ARGS_SIZE){
		return badcommandTooManyTokens();
	}

	for (int i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3) return badcommand();	
		int total_len = 0;
		for(int i=2; i<args_size; i++){
			total_len+=strlen(command_args[i])+1;
		}
		char *value = (char*) calloc(1, total_len);
		char spaceChar = ' ';
		for(int i = 2; i < args_size; i++){
			strncat(value, command_args[i], strlen(command_args[i]));
			if(i < args_size-1){
				strncat(value, &spaceChar, 1);
			}
		}
		int errCode = set(command_args[1], value);
		free(value);
		return errCode;
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "echo")==0){
		if (args_size > 2) return badcommand();
		return echo(command_args[1]);

	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size > 1) return badcommand();
		return my_ls();
	
	} else if (strcmp(command_args[0], "my_mkdir")==0) {
		if (args_size > 2) return badcommand();
		return my_mkdir(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_touch")==0) {
		if (args_size > 2) return badcommand();
		return my_touch(command_args[1]);
	
	}else if (strcmp(command_args[0], "exec")==0)
	{
		return exec(command_args, args_size);
	}
	 else if (strcmp(command_args[0], "my_cd")==0) {
		if (args_size > 2) return badcommand();
		return my_cd(command_args[1]);
	
	} else return badcommand();}
int memory_init()
{
	for (int i = 0; i < MAX_PROCESSES; i++) {
        readyQ->queue[i].pid = -1;
    }
}
int help(){
char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);}
int set(char* var, char* value){
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);
	mem_set_value(var, value);
	return 0;}
int print(char* var){
	char *value = mem_get_value(var);
	if(value == NULL) value = "\n";
	printf("%s\n", value); 
	return 0;}
void sort_filenames_by_length(char** filenames, int num_files) {
    int i, j;
    char* temp;
    for (i = 0; i < num_files - 1; i++) {
        for (j = 0; j < num_files - i - 1; j++) {
            off_t len1 = get_file_length(filenames[j]);
            off_t len2 = get_file_length(filenames[j + 1]);
            if (len1 > len2) {
                temp = filenames[j];
                filenames[j] = filenames[j + 1];
                filenames[j + 1] = temp;
            }
        }
    }}
off_t get_file_length(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;}

int run(char* script){
	int errCode= 0;
	long file_size;
	char* memory_space;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file
	if(p == NULL) return badcommandFileDoesNotExist();
	fseek(p, 0, SEEK_END);
	file_size = ftell(p);
	rewind(p);
	char* file_content=(char*) malloc(file_size*sizeof(char));
	if (file_content == NULL) return badCommandFileIsEmpty();
	while(fgets(line,sizeof(line),p) != NULL){
		strcat(file_content, line);
	}
	int start_pos=mem_set_value(script, file_content);
	pcb PCB = makePCB(file_content,start_pos,script,1);
	free(file_content);
	scheduler();}
int echo(char* var){
	if(var[0] == '$') print(++var);
	else printf("%s\n", var); 
	return 0;}
int my_ls(){
	int errCode = system("ls | sort");
	return errCode;}
int my_mkdir(char *dirname){
	char *dir = dirname;
	if(dirname[0] == '$'){
		char *value = mem_get_value(++dirname);
		if(value == NULL || strchr(value, ' ') != NULL){
			return badcommandMkdir();
		}
		dir = value;
	}
	int namelen = strlen(dir);
	char* command = (char*) calloc(1, 7+namelen); 
	strncat(command, "mkdir ", 7);
	strncat(command, dir, namelen);
	int errCode = system(command);
	free(command);
	return errCode;}
int exec(char* directory[], int args){
	char* policy="FCFS";
	if (isaPolicy(directory[args-1]))
	{
		policy = directory[args-1];
	}
	
	if (strcmp(policy, "FCFS") == 0){
	for (int i=1 ; i< args-1; i++)
	{
			run(directory[i]);
	}
	}
	if (strcmp(policy, "SJF") == 0)
	{
		char* newDirectory[args-1];
		//create a new array of directories
		for (int j=1; j<args-1; j++)
		{
			newDirectory[j-1] = directory[j];
		}
		//sort the new array
    	sort_filenames_by_length(newDirectory, args-2);
		//run the new array
		for (int i=0;i<args-2;i++)
		{
			run(newDirectory[i]);
		}
	}
	if (strcmp(policy,"RR")==0){
		//The policy is RR
		for (int i=1 ; i< args-1; i++)
		{
			//we first place all the processes in the PCB 
			int errCode= 0;
			long file_size;
			char* memory_space;
			char line[1000];
			char* script = directory[i];
			FILE *p = fopen(script,"rt");  // the program is in a file
			if(p == NULL) return badcommandFileDoesNotExist();
			fseek(p, 0, SEEK_END);
			file_size = ftell(p);
			rewind(p);
			char* file_content=(char*) malloc(file_size*sizeof(char));
			if (file_content == NULL) return badCommandFileIsEmpty();
			while(fgets(line,sizeof(line),p) != NULL){
				strcat(file_content, line);
			}
			int start_pos=mem_set_value(script, file_content);
			pcb PCB = makePCB(file_content,start_pos,script,1);
		} 
	    runRR(2);}
	if (strcmp(policy,"AGING")==0){
		char* newDirectory[args-1];
		//create a new array of directories
		for (int j=1; j<args-1; j++)
		{
			newDirectory[j-1] = directory[j]; 
		}
		sort_filenames_by_length(newDirectory, args-2);
		for (int i=1;i<args-2;i++)
		{
			int errCode= 0;
			long file_size;
			char* memory_space;
			char line[1000];
			char* script = newDirectory[i];
			FILE *p = fopen(script,"rt");  // the program is in a file
			if(p == NULL) return badcommandFileDoesNotExist();
			fseek(p, 0, SEEK_END);
			file_size = ftell(p);
			char* file_content=(char*) malloc(file_size*sizeof(char));
			if (file_content == NULL) return badCommandFileIsEmpty();
			while(fgets(line,sizeof(line),p) != NULL){
				strcat(file_content, line);
			}
			int start_pos=mem_set_value(script, file_content);
			pcb PCB = makePCB(file_content,start_pos,script,0);
			addToReadyQueue(PCB);
		}
	    runSJFAging();
	}
		return 0;}
int runRR(int slice){
	struct node * temp = head;
	while (temp != NULL)
	{
		pcb PCB = temp->pcb;
		char* file_content = mem_get_value(PCB.name);
		char* line =strtok(file_content, "\n");
		for (int i=0;i<PCB.curr_inst;i++)
		{
			line = strtok(NULL, "\n");
		}
		int value = 0;
		while (line != NULL && value <slice)
		{
			PCB.curr_inst++;
			char* command_args[100];
			int args_size = 0;
			char* delimiter = " ";
			char* token;
			char* start = line;
			int token_len;
			while (*start != '\0') {
    		start += strspn(start, delimiter);
    		if (*start == '\0') {break;}
		    token_len = strcspn(start, delimiter);
    		token = (char*) malloc(token_len + 1);
    		memcpy(token, start, token_len);
    		token[token_len] = '\0';
    		command_args[args_size] = token;
    		args_size++;
    		start += token_len;}
			int errCode = interpreter(command_args, args_size);
			for (int i = 0; i < args_size; i++)free(command_args[i]);
			line = strtok(NULL, "\n");
			value++;
		}
	if (line == NULL)
	{
		head =temp->next;
		temp = temp->next;

	}
	else {
		//add the node at the end of the linked list	
		struct node * new_node = (struct node*) malloc(sizeof(struct node));
		new_node->pcb = PCB;
		new_node->next = NULL;
		struct node * temp2 = head;
		while (temp2->next != NULL)
		{
			temp2 = temp2->next;
		}
		temp2->next = new_node;
		temp = temp->next;
	}}return 0;}

bool isaPolicy(char* policy){
	if(strcmp(policy, "FCFS") == 0) return true;
	if(strcmp(policy, "RR") == 0) return true;
	if(strcmp(policy, "SJF") == 0) return true;
	if(strcmp(policy, "PSJF") == 0) return true;
	if ((strcmp(policy, "AGING") == 0)) return true;
	return false;}
int my_touch(char* filename){
	int namelen = strlen(filename);
	char* command = (char*) calloc(1, 7+namelen); 
	strncat(command, "touch ", 7);
	strncat(command, filename, namelen);
	int errCode = system(command);
	free(command);
	return errCode;}
int my_cd(char* dirname){
	struct stat info;
	if(stat(dirname, &info) == 0 && S_ISDIR(info.st_mode)) {
		//the object with dirname must exist and is a directory
		int errCode = chdir(dirname);
		return errCode;
	}
	return badcommandCd();}
int runSJFAging(){
	//the ready queue is sorted by the priority of the processes
	//the first element has the lowest priority.
	struct ready_queue * ReadyQueue = readyQ;
	while (ReadyQueue->queue != NULL)
	{
		//get the first element in the ready queue
		pcb PCB =ReadyQueue->queue[0];
		//get the content of the file
		char* file_content = mem_get_value(PCB.name);
		//get the current instruction
		char* line =strtok(file_content, "\n");
		for (int i=0;i<PCB.curr_inst;i++)
		{
			line = strtok(NULL, "\n");
		}
		//run the current instruction
		if (line != NULL){	
			char* command_args[100];
			int args_size = 0;
			char* delimiter = " ";
			char* token;
			char* start = line;
			int token_len;
			while (*start != '\0') {
			start += strspn(start, delimiter);
			if (*start == '\0') {break;}
		    token_len = strcspn(start, delimiter);
			token = (char*) malloc(token_len + 1);
			memcpy(token, start, token_len);
			token[token_len] = '\0';
			command_args[args_size] = token;
			args_size++;
			start += token_len;
			}
			int errCode = interpreter(command_args, args_size);
			for (int i = 0; i < args_size; i++)free(command_args[i]);
			PCB.curr_inst++;
			}
		else {
			//remove the first element from the ready queue
			for (int i = 0; i < ReadyQueue->size - 1; i++) {
				ReadyQueue->queue[i] = ReadyQueue->queue[i+1];
			}
			ReadyQueue->size--;	
			}
		//age all the elements in the ready queue
		for (int i = 1; i < ReadyQueue->size; i++) {
			ReadyQueue->queue[i].age++;
		}
		//sort the ready queue
		sort_ready_queue(ReadyQueue);
	}}
void sort_ready_queue(struct ready_queue * ReadyQueue) {
	int i, j;
	for (i = 0; i < ReadyQueue->size - 1; i++) {
		for (j = 0; j < ReadyQueue->size - i - 1; j++) {
			if (ReadyQueue->queue[j].age > ReadyQueue->queue[j+1].age) {
				pcb temp = ReadyQueue->queue[j];
				ReadyQueue->queue[j] = ReadyQueue->queue[j+1];
				ReadyQueue->queue[j+1] = temp;
 	   }
		}
	}}
void addToReadyQueue(pcb PCB){
	//add the PCB to the ready queue
	if (readyQ->size == 0){
		readyQ->queue[0] = PCB;
		readyQ->size++;
	}
	else {
		readyQ->queue[readyQ->size] = PCB;
		readyQ->size++;
	}}
int scheduler(){
	struct node* temp = head;
	while (temp != NULL)
	{
		pcb PCB = temp->pcb;
		char* file_content = mem_get_value(PCB.name);
		int value = PCB.start_pos;
		char* line = strtok(file_content, "\n");
		while (line != NULL) {
			char* command_args[100];
			int args_size = 0;
			char* delimiter = " ";
			char* token;
			char* start = line;
			int token_len;
			while (*start != '\0') {
    		start += strspn(start, delimiter);
    		if (*start == '\0') {break;}
		    token_len = strcspn(start, delimiter);
    		token = (char*) malloc(token_len + 1);
    		memcpy(token, start, token_len);
    		token[token_len] = '\0';
    		command_args[args_size] = token;
    		args_size++;
    		start += token_len;}
			int errCode = interpreter(command_args, args_size);
			for (int i = 0; i < args_size; i++)free(command_args[i]);
			PCB.curr_inst+=1;
			line = strtok(NULL, "\n");
		}
		head = temp->next;
		temp=temp->next;
		}
	return 0;
	}
