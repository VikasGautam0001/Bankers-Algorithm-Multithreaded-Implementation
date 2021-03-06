#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
pthread_mutex_t lockResources;
pthread_cond_t condition;
// Function declaration
bool getSafeSeq();
int nResources,
    nProcesses;
int *resources;
int **allocated;
int **maxRequired;
int **need;
int *safeSeq;
int nProcessRan = 0;

void* processCode(void *arg) {
        int p = *((int *) arg);

        pthread_mutex_lock(&lockResources);

        
        while(p != safeSeq[nProcessRan])
                pthread_cond_wait(&condition, &lockResources);

        printf("\n--> Process %d", p+1);
        printf("\n\tAllocated : ");
        for(int i=0; i<nResources; i++)
                printf("%3d", allocated[p][i]);

        printf("\n\tNeeded    : ");
        for(int i=0; i<nResources; i++)
                printf("%3d", need[p][i]);

        printf("\n\tAvailable : ");
        for(int i=0; i<nResources; i++)
                printf("%3d", resources[i]);

        printf("\n"); sleep(1);

        printf("\tResource Allocated!");
        printf("\n"); sleep(1);
        printf("\tResource released!");
        printf("\n");
	for(int i=0; i<nResources; i++)
                resources[i] += allocated[p][i];

        printf("\n\tNow Available : ");
        for(int i=0; i<nResources; i++)
                printf("%3d", resources[i]);
        printf("\n\n");

        sleep(1);


        nProcessRan++;
        pthread_cond_broadcast(&condition);
        pthread_mutex_unlock(&lockResources);
	pthread_exit(NULL);
}

int main() {
        srand(time(NULL));
        printf("\nNumber of processes? ");
        scanf("%d", &nProcesses);
        printf("\nNumber of resources? ");
        scanf("%d", &nResources);

        resources = (int *)malloc(nResources * sizeof(*resources));
        printf("\nCurrently Available resources :\n\n");
        for(int i=0; i<nResources; i++){
               printf(“Resource R%d :” , i+1); 
    scanf("%d", &resources[i]);
       }
        allocated = (int **)malloc(nProcesses * sizeof(*allocated));
        for(int i=0; i<nProcesses; i++)
                allocated[i] = (int *)malloc(nResources * sizeof(**allocated));

        maxRequired = (int **)malloc(nProcesses * sizeof(*maxRequired));
        for(int i=0; i<nProcesses; i++)
                maxRequired[i] = (int *)malloc(nResources * sizeof(**maxRequired));

        printf("\n");
        for(int i=0; i<nProcesses; i++) {
                printf("\nResource allocated to process %d : \n", i+1);
                for(int j=0; j<nResources; j++){
               	printf(“Resource R%d :”, j+1);
           scanf("%d", &allocated[i][j]);

}
        }
        printf("\n");

        for(int i=0; i<nProcesses; i++) {
                printf("\nMaximum resource required by process %d  \n", i+1);
                for(int j=0; j<nResources; j++){
           		printf(“Resource R%d :”, j+1);   
          scanf("%d", &maxRequired[i][j]);
   }
        }
        printf("\n");

	
        need = (int **)malloc(nProcesses * sizeof(*need));
        for(int i=0; i<nProcesses; i++)
                need[i] = (int *)malloc(nResources * sizeof(**need));

        for(int i=0; i<nProcesses; i++)
                for(int j=0; j<nResources; j++)
                        need[i][j] = maxRequired[i][j] - allocated[i][j];


	safeSeq = (int *)malloc(nProcesses * sizeof(*safeSeq));
        for(int i=0; i<nProcesses; i++) safeSeq[i] = -1;

        if(!getSafeSeq()) {
                printf("\nThe processes leads the system to a unsafe state.\n\n");
                exit(-1);
        }

        printf("\n\nSafe Sequence Found : ");
        for(int i=0; i<nProcesses; i++) {
                printf("%-3d", safeSeq[i]+1);
        }

        printf("\nExecuting Processes...\n\n");
        sleep(1);

	pthread_t processes[nProcesses];
        pthread_attr_t attr;
        pthread_attr_init(&attr);

	int processNumber[nProcesses];
	for(int i=0; i<nProcesses; i++) processNumber[i] = i;

        for(int i=0; i<nProcesses; i++)
                pthread_create(&processes[i], &attr, processCode, (void *)(&processNumber[i]));

        for(int i=0; i<nProcesses; i++)
                pthread_join(processes[i], NULL);

        printf("\nAll Processes Completed\n");

        free(resources);
        for(int i=0; i<nProcesses; i++) {
                free(allocated[i]);
                free(maxRequired[i]);
		free(need[i]);
        }
        free(allocated);
        free(maxRequired);
	free(need);
        free(safeSeq);
	return 0;
}
bool getSafeSeq() {
        int tempRes[nResources];
        for(int i=0; i<nResources; i++) tempRes[i] = resources[i];

        bool finished[nProcesses];
        for(int i=0; i<nProcesses; i++) finished[i] = false;
        int nfinished=0;
        while(nfinished < nProcesses) {
                bool safe = false;

                for(int i=0; i<nProcesses; i++) {
                        if(!finished[i]) {
                                bool possible = true;
                                for(int j=0; j<nResources; j++)
                                        if(need[i][j] > tempRes[j]) {
                                                possible = false;
                                                break;}
                              if(possible) {
                                        for(int j=0; j<nResources; j++)
                                                tempRes[j] += allocated[i][j];
                                        safeSeq[nfinished] = i;
                                        finished[i] = true;
                                        ++nfinished;
                                        safe = true;
                                }
                        }
                }
                if(!safe) {
                        for(int k=0; k<nProcesses; k++) safeSeq[k] = -1;
                        return false; 
                }
        }
        return true; 
}
