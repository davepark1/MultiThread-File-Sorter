#include "multiThreadSorter.h"


char* sc = NULL;
char* allC[28] = {"color", "director_name", "num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes", "actor_2_name", "actor_1_facebook_likes", "gross", "genres", "actor_1_name", "movie_title", "num_voted_users", "cast_total_facebook_likes", "actor_3_name", "facenumber_in_poster", "plot_keywords", "movie_imdb_link", "num_user_for_reviews", "language", "country", "content_rating", "budget", "title_year", "actor_2_facebook_likes", "imdb_score", "aspect_ratio", "movie_facebook_likes"};
// SHARED DATA: allThread linked list, movie nodes linked list (head), movie nodes link list (tail), total number of threads
//struct Thread* tlist = NULL;
//struct Thread* ttail = NULL;
struct Movie* mlist = NULL;
struct Movie* mtail = NULL;
//int totalThreads = 0;
int count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; //mlist
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; //mtail
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER; //count
//pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER; //ttail
//pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER; //totalThreads
//pthread_mutex_t mutex6 = PTHREAD_MUTEX_INITIALIZER;

char* trim(char* string){
	//trim column values
	//printf("in trim\n");
	if (strlen(string) == 0 || string == NULL){
		char* tmp = (char*) malloc(sizeof(char));
		tmp = "";
		return tmp;
	}
	int bpos = 0;
	int epos = strlen(string)-1;
	
	while(string[bpos] == ' '){
		bpos++;
	}
	
	while(string[epos] == ' ' || string[epos] == '\n'){
		epos--;
	}
	
	if (epos < bpos){
		char* tmp = (char*) malloc(sizeof(char));
		tmp = "";
		return tmp;
	}
	
	char* res = (char*)malloc(sizeof(char)*((epos-bpos)+2));
	int x, y;
	for (x = bpos, y = 0; x <= epos; x++, y++){
		res[y] = string[x];
	}
	res[y] = '\0';
	
	return res;
}

char** createColumns(char* string, int* totalClmns){
	//printf("in createColumns\n");
	//research strtok.
	char** columns = (char**)malloc(sizeof(char*)*28);
	int pos = 0;
	char* token;
	char* ptr = string;
	
	while((token = strtok_r(ptr, ",", &ptr))){
		char* tmp = (char*)malloc(sizeof(char)*(strlen(token)+1));
		//strcpy(tmp, token);
		tmp = trim(token);
		columns[pos] = tmp;
		pos++;
	}
	
	*totalClmns = pos;
	return columns;
}

struct Movie* createObject(char* string, int totalClmns){
	//printf("in createObject\n");
	struct Movie* movie = (struct Movie*) malloc(sizeof(struct Movie));
	movie->categories = (char**) malloc(sizeof(char*)*totalClmns);
	movie->trimmed = (char**) malloc(sizeof(char*)*totalClmns);
	movie->totalClmns = totalClmns;
	movie->next = NULL;

	
	int pos = 0;
	char* ptr; char* token = strtok_r(string, ",", &ptr);
	//char* ptr = string;
	
	//while((token = strtok_r(ptr, ",", &ptr))){
	while(token != NULL){
		//printf("%s\n", token);
		
		char* tmp;
		if (token[strlen(token)-1] == '\n'){
			tmp = (char*) malloc(sizeof(char)*(strlen(token)));
			/*//token[strlen(token)-1] = '\0';
			strncpy(tmp, token, strlen(token)-1);
			strcat(tmp, "\0");*/
			
			int pos = strlen(token)-1;
			int x;
			while(token[pos] == '\n' || token[pos] == '\0'){
				pos--;
			}
			
			for (x = 0; x <= pos; x++){
				tmp[x] = token[x];
			}
			tmp[x] = '\0';
		}else{
			tmp = (char*)malloc(sizeof(char)*(strlen(token)+1));
			strcpy(tmp, token);
		}	
		movie->categories[pos] = tmp;
		char* tmp2 = trim(token);
		movie->trimmed[pos] = tmp2;
		pos++;
		
		int y = 0;
		while (ptr[y] == ','){
			movie->categories[pos] = (char*) malloc(sizeof(char)*2); movie->categories[pos] = ""; movie->trimmed[pos] = (char*) malloc(sizeof(char)*2); movie->trimmed[pos] = ""; pos++; y++;
		}
		
		token = strtok_r(NULL, ",", &ptr);
	}
	
	if (pos > totalClmns+1){
		fprintf(stderr, "Error in file format. Data contains more columns that it should.\n");
		printf("Error in file format. Data contains more columns that it should.\n");
		return NULL;
	}
	
	//free(string);
	return movie;
	
}

int checkIfDigit(char* h1, char* h2){

	int x;
	for (x = 0; x < strlen(h1); x++){
		if (isdigit(h1[x]) == 0){
			return 0;
		}
	}
	
	for (x = 0; x < strlen(h2); x++){
		if (isdigit(h2[x]) == 0){
			return 0;
		}
	}
	return 1;
}

struct Movie* mergeSort(struct Movie* head, int sortclmn){
	//printf("in mergesort\n");
	struct Movie* mid = head;
	struct Movie* tmp = head;
	struct Movie* result;
	int x;
	int total = 0;
	
	while(tmp != NULL){
		total++;
		tmp = tmp->next;
	}
	
	if (head->next == NULL){
		return head;
	}
	
	for (x = 1; x < (total/2); x++){
		mid = mid->next;
	}
		
	tmp = mid;
	mid = mid->next;
	tmp->next = NULL;
	
	if (total > 1){
		struct Movie* h1 = mergeSort(head, sortclmn);
		struct Movie* h2 = mergeSort(mid, sortclmn);
		
		result = merge(h1, h2, sortclmn);

	}

	return result;
}

struct Movie* merge(struct Movie* h1, struct Movie* h2, int sortclmn){
	//printf("in merge\n");
	struct Movie* head = NULL;
	struct Movie* tail = NULL;
	
	int digit;
	
	while(h1 != NULL && h2 != NULL){
		char* h1value;
		char* h2value;
		
		if (h1->totalClmns <= sortclmn && h2->totalClmns <= sortclmn){
			h1value = "";
			h2value = "";
		}else if(h1->totalClmns <= sortclmn){
			h1value = "";
			if (strcmp(h2->trimmed[sortclmn], "") == 0){
				h2value = h2->categories[sortclmn];
			}else{
				h2value = h2->trimmed[sortclmn];
			}
		}else if(h2->totalClmns <= sortclmn){
			h2value = "";
			if (strcmp(h1->trimmed[sortclmn], "") == 0){
				h1value = h1->categories[sortclmn];
			}else{
				h1value = h1->trimmed[sortclmn];
			}
		}else{
			if (strcmp(h1->trimmed[sortclmn], "") == 0){
				h1value = h1->categories[sortclmn];
			}else{
				h1value = h1->trimmed[sortclmn];	
			}
			
			if (strcmp(h2->trimmed[sortclmn], "") == 0){
				h2value = h2->categories[sortclmn];
			}else{
				h2value = h2->trimmed[sortclmn];
			}
		}
		

		int h1digit;
		int h2digit;
		
		if (strcmp(h1value, "") == 0 || strcmp(h2value, "") == 0){
			if (strcmp(h1value, "") == 0){
				if (head == NULL){
					head = h1;
					tail = h1;
				}else{
					tail->next = h1;
					tail = tail->next;
				}
				
				h1 = h1->next;
			}else{
				if (head == NULL){
					head = h2;
					tail = h2;
				}else{
					tail->next = h2;
					tail = tail->next;
				}
				h2 = h2->next;
			}
			continue;
		}
		
		digit = checkIfDigit(h1value, h2value);
		
		if (digit == 1){
			h1digit = atoi(h1value);
			h2digit = atoi(h2value);
			
			if (h1digit <= h2digit){
				if (head == NULL){
					head = h1;
					tail = h1;
				}else{
					tail->next = h1;
					tail = tail->next;
				}
				
				h1 = h1->next;
			}else{
				if (head == NULL){
					head = h2;
					tail = h2;
				}else{
					tail->next = h2;
					tail = tail->next;
				}
				h2 = h2->next;
			}
		}else{
			if (strcmp(h1value, h2value) <= 0){
				if (head == NULL){
					head = h1;
					tail = h1;
				}else{
					tail->next = h1;
					tail = tail->next;
				}
				h1 = h1->next;
			}else{
				if (head == NULL){
					head = h2;
					tail = h2;
				}else{
					tail->next = h2;
					tail = tail->next;
				}
				h2 = h2->next;
			}
		}	
	}
	
	if (h1 == NULL){
		while(h2 != NULL){
			tail->next = h2;
			tail = tail->next;
			h2 = h2->next;
		}
	}else{
		while(h1 != NULL){
			tail->next = h1;
			tail = tail->next;
			h1 = h1->next;
		}
	}
	

	return head;
	
}

struct Movie* objDump(FILE* file){
	char* string = (char*)malloc(sizeof(char)*600);
	char** columns;
	
	fgets(string, 600, file);
		
	if (string == NULL){
		fprintf(stderr, "Invalid Header format. Too many columns or no columns.\n");
		printf("Invalid Header format. Too many columns or no columns.\n");
		pthread_exit(NULL);
	}
	
	int totalClmns;
	int* tc = &totalClmns;
	columns = createColumns(string, tc);
	
	struct Movie* head = NULL;
	struct Movie* tail = NULL;
	int num = 0;
	int len = 600;
	string = (char*) malloc(sizeof(char)*600);
	
	while (fgets(string, 600, file)){
		
		struct Movie* tmp = createObject(string, totalClmns);
		if (tmp == NULL){
			return NULL;
		}
		tmp->columns = columns;
		
		if (head == NULL){
			head = tmp;
			tail = tmp;
		}else{
			tail->next = tmp;
			tail = tail->next;
		}
		
	}
	
	/*int x;
	int sortclmn = -1;
	for (x = 0; x < totalClmns; x++){
		if (strcmp(columns[x], sc) == 0){
			sortclmn = x;
			break;
		}
	}
	
	if (sortclmn == -1){
		fprintf(stderr, "Column name not present in this file. No sorting done.\n");
		printf("Column name not present in this file. No sorting done.\n");
		return head;
	}
	
	head = mergeSort(head, sortclmn);*/

	return head;
}


void* runnable(void* arg){
	struct Arg* object = (struct Arg*)arg;
	char path[PATH_MAX+1];
	strcpy(path, object->path);
	char fname[256];
	strcpy(fname, object->fname);
	int status = object->status;
	free(arg);
	
	struct Thread* trackhead = NULL;
	struct Thread* tracktail = NULL;
	int totalThreads = 0;
	
	/*struct Thread* t = (struct Thread*) malloc(sizeof(struct Thread));
	t->next = NULL;
	
	//LOCK MUTEX3 AND MUTEX4 AND MUTEX5
	pthread_mutex_lock(&mutex3);
	pthread_mutex_lock(&mutex4);
	pthread_mutex_lock(&mutex5);
	pthread_mutex_lock(&mutex6);
	
	count++;
	t->tid = count;
	if (tlist == NULL){
		tlist = t;
		ttail = t;
	}else{
		ttail->next = t;
		ttail = ttail->next;
	}
	totalThreads++;
	
	//UNLOCK MUTEX3 AND MUTEX4 AND MUTEX5
	pthread_mutex_unlock(&mutex6);
	pthread_mutex_unlock(&mutex5);
	pthread_mutex_unlock(&mutex4);
	pthread_mutex_unlock(&mutex3);*/
	
	if (status == 0){
		//NEED TO FIGURE OUT HOW TO SEND STRUCT DIRENT*
		char* token;
		char* ptr = (char*) malloc(sizeof(char)*50);
		int iscsv = 0;
		int fn = 1;
		strcpy(ptr, fname);
		
		while((token = strtok_r(ptr, ".", &ptr))){
			if (strcmp(token, "csv") == 0){
					if (fn == 0){
						iscsv = 1;
					}
				}
				fn = 0;
			}
				
		if (iscsv == 1){
			struct Movie* head;
			struct Movie* tail;
			FILE* file = fopen(path, "r");
			if (file == NULL){
				fprintf(stderr, "Could not open file %s\n", fname);
				printf("Could not open file %s\n", fname);
				pthread_exit(NULL);
			}
			head = objDump(file);
			fclose(file);
			
			if (head == NULL){
				printf("Empty file or incorrect file format. Moving on to next file.\n");
				pthread_exit(NULL);
			}
			
			tail = head;
			while(tail != NULL){
				if (tail->next == NULL){
					break;
				}
				tail = tail->next;
			}
			
			//LOCK MUTEX1 AND MUTEX2
			pthread_mutex_lock(&mutex1);
			pthread_mutex_lock(&mutex2);
			
			if (mlist == NULL){
				mlist = head;
				mtail = tail;
			}else{
				mtail->next = head;
				mtail = tail;
			}
			//UNLOCK MUTEX1 AND MUTEX2
			pthread_mutex_unlock(&mutex2);
			pthread_mutex_unlock(&mutex1);
			
		}
		printf("Initial PID: %d\n", getpid());
		printf("TIDs of all spawned thread: \n");
		printf("Total number of threads: %d\n", totalThreads);
		pthread_exit(NULL);
	}
	
	DIR* dir;
	struct dirent* target;
	dir = opendir(path);
	
	
	while(target = readdir(dir)){
		struct stat type;
		char tmppath[PATH_MAX+1];
		strcpy(tmppath, path);
		if (tmppath[strlen(tmppath)-1] != '/'){
			strcat(tmppath, "/");
		}
		strcat(tmppath, target->d_name);
		status = stat(tmppath, &type);
		
		if (strcmp(target->d_name, ".") == 0 || strcmp(target->d_name, "..") == 0 || strcmp(target->d_name, "...") == 0){
			continue;
		}
		
		pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		
		status = S_ISDIR(type.st_mode);

		if (status != 0){
			//printf("%s is a directory.\n", target->d_name);	
			struct Arg* arg = (struct Arg*) malloc(sizeof(struct Arg));
			strcpy(arg->path, tmppath);
			arg->status = status;
			//printf("status is: %d\n", status);
				
			pthread_create(&thread, NULL, runnable, (void*)arg);
			pthread_attr_destroy(&attr);
			//printf("tid: %d\n", thread);
			pthread_join(thread, NULL);
		}else{
			//printf("This is not a directory and status is: %d\n", status);
			struct Arg* arg = (struct Arg*) malloc(sizeof(struct Arg));
			strcpy(arg->path, tmppath);
			strcpy(arg->fname, target->d_name);
			arg->status = status;
			
			pthread_create(&thread, NULL, runnable, (void*)arg);
			pthread_attr_destroy(&attr);
			//printf("tid: %d\n", thread);
			pthread_join(thread, NULL);
		}
		
		struct Thread* tmp = (struct Thread*) malloc(sizeof(struct Thread));
		totalThreads++;
		
		pthread_mutex_lock(&mutex3);
		count++;
		tmp->tid = count;
		tmp->next = NULL;
		pthread_mutex_unlock(&mutex3);
				
		if (trackhead == NULL){
			trackhead = tmp;
			tracktail = tmp;
		}else{
			tracktail->next = tmp;
			tracktail = tracktail->next;
		}
	}
	
	printf("Initial PID: %d\n", getpid());
	printf("TIDS of all spawned thread: ");
	struct Thread* tmp = trackhead;
	while(tmp != NULL){
		printf("%d ", tmp->tid);
		struct Thread* dump = tmp;
		tmp = tmp->next;
		free(dump);
	}
	printf("\n");
	printf("Total number of threads: %d\n", totalThreads);
	
	closedir(dir);
	pthread_exit(NULL);
}

int main (int argc, char** argv){
	
	DIR* dir;
	struct dirent* target;
	//struct stat type;
	//int status;	
	
	//char* sc = NULL;
	char path[PATH_MAX+1] = "";
	char* outdir = NULL;
	
	if (argc < 3){
		fprintf(stderr, "Invalid command line entry.\n");
		printf("Invalid command line entry.\n");
		return -1;
	}
	
	int a = 1;
	for ( ; a < argc; a++){
		if (strcmp(argv[a], "-c") == 0){
			sc = argv[a+1];
		}
		
		if (strcmp(argv[a], "-d") == 0){
			strcpy(path, argv[a+1]);
		}
		
		if (strcmp(argv[a], "-o") == 0){
			outdir = argv[a+1];
		}
	}
	
	if (sc == NULL){
		fprintf(stderr, "Invalid command line entry. Missing \"-c\"\n");
		printf("Invalid command line entry. Missing \"-c\"\n");
		exit(0);
	}
	
	int check = 0;
	int i = 0;
	for ( ; i < 28; i++){
		if (strcmp(sc, allC[i]) == 0){
			check = 1;
		}
	}
	
	if (check == 0){
		fprintf(stderr, "Invalid column name entry. Column name does not exist in Movie metadata. Check formatting: column name must be exact.\n");
		printf("Invalid column name entry. Column name does not exist in Movie metadata. Check formatting: column name must be exact.\n");
		exit(0);
	}
	
	if (strcmp(path, "") == 0){
		strcpy(path, ".");	
	}
	
	
	dir = opendir(path);
	if (dir == NULL){
		fprintf(stderr, "Could not open directory in path: %s\n", path);
		printf("Could not open directory in path: %s\n", path);
		exit(0);
	}
	
	pid_t pid = getpid();
	//printf("Initial PID: %d\n", pid);
	
	pthread_t thread;
	struct Arg* arg = (struct Arg*) malloc(sizeof(struct Arg));
	strcpy(arg->path, path);
	arg->status = 1;
	
	pthread_create(&thread, NULL, runnable, (void*)arg);
	
	pthread_join(thread, NULL);



	//SORT ALL INDIVIDUALLY SORTED OUTPUT
	int sortclmn = 0;
	for (a = 0; a < 28; a++){
		if (strcmp(allC[a], sc) == 0){
			sortclmn = a;
			break;
		}
	}
	//printf("sortclmn is: %d\n", sortclmn);
	
	
	
	mlist = mergeSort(mlist, sortclmn);


	
	if (outdir != NULL){
		strcpy(path, outdir);
	}
				
	if (path[strlen(path)-1] != '/'){
		strcat(path, "/");
	}
				
	strcat(path, "AllFiles-sorted-");
	strcat(path, sc);
	strcat(path, ".csv");
	FILE* newfile = fopen(path, "w");
	
	//place headers in file
	for (i = 0; i < 28; i++){
		if (i == 27){
			fprintf(newfile, "%s\n", allC[i]);
		}else{
			fprintf(newfile, "%s,", allC[i]);
		}
	}
	
	//place all data in file
	struct Movie* m = mlist;
	while(m != NULL){
		int x;
		for (x = 0; x < 28; x++){
			/*int contains = 0;
			int y;
			for (y = 0; y < m->totalClmns; y++){
				if(strcmp(allC[x], m->columns[y]) == 0){
					contains = 1;
					break;
				}
			}*/
			
			if (strcmp(m->categories[x], "") != 0){
				if (x == 27){
					fprintf(newfile, "%s\n", m->categories[x]);
				}else{
					fprintf(newfile, "%s,", m->categories[x]);
				}
			}else{
				if (x == 27){
					fprintf(newfile, " \n");
				}else{
					fprintf(newfile, ",");
				}
			}
		}
		
		struct Movie* dump = m;
		m = m->next;
		
		free(dump->categories);
		free(dump->trimmed);
		free(dump);
	}
	fclose(newfile);
	
	
	//LIST OUT ALL THREADS AND TOTAL
	/*printf("TIDS of all spawned thread: ");
	struct Thread* t = tlist;
	while(t != NULL){
		if (t->next == NULL){
			printf("%d\n", t->tid);
		}else{
			printf("%d, ", t->tid);
		}
		t = t->next;
	}
	printf("Total number of threads: %d\n", totalThreads);*/
	return 0;
	
}