#include<pthread.h>
#include<semaphore.h>

// The maximum number of staff and chair number
#define NUMBER_OF_STAFF 8
#define NUMBER_OF_CHAIRS 3

//chair count in unit
int freeChairs = 3;
int seatNumber[3];
static int count = 0;

//Define the semaphores.
sem_t chairs_mutex;//for chairs
sem_t patient_mutex;
sem_t staff_mutex;

void patient(void *number)
{
    int mySeat;
    int num = *(int *)number;
    count++;

    printf("Patient %d arrived\n", num);

    sem_wait(&chairs_mutex);
    if (freeChairs == 1)        //if 1 free chair left
    {
        --freeChairs;            //deccrease the number of free chairs
        sem_post(&chairs_mutex); //Release seat lock
        sem_post(&staff_mutex);  //call staff for testing
        sem_wait(&patient_mutex);//join queue of patients
        sem_wait(&chairs_mutex); //lock seat against changes
        freeChairs=3;            //free 3 chairs
        sem_post(&chairs_mutex); //release seat lock
    }
    else if (freeChairs>=2)      //if 2 or 3 free chairs
    {
        --freeChairs;            //deccrease the number of free chairs
        sem_post(&chairs_mutex);
        if (freeChairs==1)       //if 1 free chair left after last patient
            printf("The last people, let's start!Please, pay attention to your social distance and hygiene; use a mask.\n");

    }
    else
    {
        sem_post(&chairs_mutex);
    }

    pthread_exit(0);

}

void staff(void *number)
{
    int num = *(int *)number;
    int myNext,P;

    while (1)
    {
        //wait for patient
        printf("The staff %d is vantilating the room\n",num);
        sem_wait(&staff_mutex); //queue of waiting staff
        sem_wait(&chairs_mutex); //lock chairs against chnages
        sem_post(&chairs_mutex);
        sem_post(&patient_mutex); //call patients
        printf("Testing in progress in %d\n",num);
        sleep(3);
    }
}
void randCome()
{
    int len = rand() % 5;
    //for differnet seed at every execution for random number generation
    srand(time(NULL));
    sleep(len);
}

int main()
{
    //define thread variables
    pthread_t stid[NUMBER_OF_STAFF];
    pthread_t ptid;

    // Initialize the semaphores with initial values...
    sem_init(&chairs_mutex, 0, 1);
    sem_init(&patient_mutex,0,0);
    sem_init(&staff_mutex,0,0);

    int Number[NUMBER_OF_STAFF];
    for (int i = 0; i < NUMBER_OF_STAFF; i++)
    {
        Number[i] = i;
    }

    // Create the staff threads
    for (int i = 0; i < NUMBER_OF_STAFF; i++)
    {
        pthread_create(&stid[i], NULL, staff, (void *)&Number[i]);
    }

    int i=0;
    // Create the patients.
    while(1)
    {
        pthread_create(&ptid, NULL, patient, (void *)&i);
        randCome();
        i++;
    }

    return 0;
}
