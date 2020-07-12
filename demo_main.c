
#include <stdio.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <math.h>

/********************Queue related logic*********************/
//strengh the queue struct for the program
#define N 10  //define the maxsize of the queue
typedef double data_t;   //The data type of the data elements in the queue
typedef struct  
{  
     data_t data[N][4]; //Use arrays as storage space for queues and is B,P,AP AND AB  
     int front,rear; //Pointer indicating head position and tail position
	//But we can’t use front when we use it, it is always 0;
     //rear = N-1 means the queue is full, rear<N-1 means the queue is not full
}sequeue_t; 

// create an empty swqueue
sequeue_t *CreateEmptySequeue()  
{  
    sequeue_t *queue; //name of the variable of type 'sequeu_t' structure is queue
    queue = (sequeue_t *)malloc(sizeof(sequeue_t));  
    if (NULL == queue) return NULL;  
    memset(queue,0,sizeof(sequeue_t));
    queue->front  = 0;  
    queue->rear = -1;
  
    return queue;  
}  

//  destroy a sequeue
void DestroySequeue(sequeue_t *queue)  
{  
    if (NULL != queue)   
    {  
        free(queue);  
    }  
}  

//see if a sequeue is empty or not
int EmptySequeue(sequeue_t *queue)  
{  
	if (queue->rear == -1)
		return 1;
	else 
		return 0; 
}  

int FullSequeue(sequeue_t *queue)  
{  
	if (queue->rear == N-1)
		return 1;
	else 
		return 0;
}  

void ClearSequeue(sequeue_t *queue)  
{  
    if (NULL == queue) return;  
      
    queue->front = queue->rear = 0;  
  
    return;  
} 

//Join the queue and add to the last position in the queue
//ENQUEUE
int NewEnQueue(sequeue_t *queue, data_t *x)
{
	int i;

	if (NULL == queue) return -1;
	//if(1 == FullSequeue(queue)) return -1;
	if (FullSequeue(queue) == 1)
		return -1;


	int count = queue->rear+1;
	queue->rear = queue->rear+1;  //End of queue+1
	for(i=0; i<4;i++){
		queue->data[count][i] = x[i];
	}
	return 0;
}



//Dequeue. The return value is the value of dequeue
data_t * NewDeQueue(sequeue_t *queue)
{
	int i;
	data_t *values = (data_t *)malloc(4*sizeof(data_t));

	if(NULL ==queue) return NULL;

	if(1 == EmptySequeue(queue)) return NULL;

	memcpy((char *)values, (char *)queue->data[0],4*sizeof(data_t));
	//printf("the dequeue->data is %lf,%lf,%lf,%lf\n\n", values[0],values[1],values[2],values[3]);
	int count = queue->rear;
	for(i=1; i< count+1; i++){
		queue->data[i-1][0] = queue->data[i][0];
		queue->data[i-1][1] = queue->data[i][1];
		queue->data[i-1][2] = queue->data[i][2];
		queue->data[i-1][3] = queue->data[i][3];
	}
	queue->rear=queue->rear-1;  //End of queue-1
	return values;
}

void printQueue(sequeue_t *queue){
	int i,j;
	for(i=0;i<=queue->rear;i++){
		for(j=0; j<4; j++){
			printf("%lf ",queue->data[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");
}


void fprintQueue(sequeue_t *queue, FILE* fp1){
	int i,j;
	for(i=0;i<=queue->rear;i++){
		for(j=0; j<4; j++){
			fprintf(fp1,"%lf ",queue->data[i][j]);
		}
		fprintf(fp1, "\n");
	}
	fprintf(fp1, "\n\n");
}
/********************Queue related logic*********************/




/********************Network card data related functions*********************/
//The return value is long data[4],Represents the number of received and sent packets and bytes, respectively

long *my_ipconfig(char *ath0)  
{  
      
    int nDevLen = strlen(ath0);  
    if (nDevLen < 1 || nDevLen > 100)  
    {  
        printf("dev length too long\n");  
        return NULL;  
    }  
    int fd = open("/proc/net/dev", O_RDONLY | O_EXCL);  
    if (-1 == fd)  
    {  
        printf("/proc/net/dev not exists!\n");  
        return NULL;  
    }  
      
        char buf[1024*4];  
        lseek(fd, 0, SEEK_SET);  
        int nBytes = read(fd, buf, sizeof(buf)-1);   //get the fd data to buf, and ruturn total bytes
        if (-1 == nBytes)  
        {  
            perror("read error");  
            close(fd);  
            return NULL;  
        }  
        buf[nBytes] = '\0';    //proess the end
        //Returns the first pointer to ath0 
        char* pDev = strstr(buf, ath0);  
        if (NULL == pDev)  
        {  
            printf("don't find dev %s\n", ath0);  
            return NULL;  
        }  
        //have find the start point of network status
        char *p;  
        char *ifconfig_value;  
        int i = 0;  
        static long rx2_tx10[4];   //ues this value to save the two values we need. 
        /*Remove unnecessary fields such as spaces, tabs, line breaks, etc.*/  
        for (p = strtok(pDev, " \t\r\n"); p; p = strtok(NULL, " \t\r\n"))  
        {  
            i++;  
            ifconfig_value = (char*)malloc(20);  //20 bytes is enough for ifocnfig_value 
            strcpy(ifconfig_value, p);  
            /*The second field in the resulting string is to receive traffic*/  
            if(i == 2)  
            {  
                rx2_tx10[0] = atol(ifconfig_value);  //atol change char to long  
            }  
            if(i == 3)  
            {  
                rx2_tx10[1] = atol(ifconfig_value);  //atol change char to long  
            }  
            /*The tenth field in the resulting string is the sending traffic*/  
            if(i == 10)  
            {  
                rx2_tx10[2] = atol(ifconfig_value);  
            }  
             if(i == 11)  
            {  
                rx2_tx10[3] = atol(ifconfig_value);  
                break;  
            }  
            free(ifconfig_value);  
        }  
        return rx2_tx10;  
}  

/********************Network card data related functions*********************/





/********************Initialize metric related functions*********************/
//返回1s中时间 //Return time in 1s
double * Initial_Metrics(char *ethernet_name)
{


	long *ifconfig_result1;  
    long *ifconfig_result2;
    long tmp_value1[4];
    long tmp_value2[4];
    double metrics[4];
    double *values = (double *)malloc(4*sizeof(double));

    int i;
    //ifconfig_result is the info on NPC we need
    ifconfig_result1 = my_ipconfig(ethernet_name); 

    //the 1st tmp
    for(i=0;i<4;i++){
    	tmp_value1[i] = ifconfig_result1[i]/1.0;
    }

    sleep(2.5);  //STATS_PERIOD

    ifconfig_result2 = my_ipconfig(ethernet_name);
    for(i=0;i<4;i++){
    	tmp_value2[i] = ifconfig_result2[i]/1.0;
    }
    //calculate the metrics
    metrics[0] = tmp_value2[0]-tmp_value1[0];  //DIFF IN THE NUMBER OF PACKETS WITHTIN THAT TIME(2.5s)
    metrics[1] = tmp_value2[1]-tmp_value1[1];
    metrics[2] = (double)(tmp_value2[0]-tmp_value1[0])/(double)(tmp_value2[2]-tmp_value1[2]);
    metrics[3] = (double)(tmp_value2[1]-tmp_value1[1])/(double)(tmp_value2[3]-tmp_value1[3]);
    	
    memcpy(values,metrics,4*sizeof(double));

    return values;
}

/********************Initialize metric related functions*********************/



/********************Initialize predic_value related functions*********************/
double *initial_predic(sequeue_t *metrics)
{
	//Initialize the required variables
	int i,j;
	double lamda[N],tmp[4];
	double *predic_value = (double *)malloc(4*sizeof(double));
	sequeue_t *metrics_tmp = CreateEmptySequeue();
	memcpy(metrics_tmp, metrics, sizeof(sequeue_t)); 

	//printQueue(metrics_tmp);
	//Initialize lamda
	for(i=0;i<N;i++){
		lamda[i] = 1/(double)N;
	}

	//Calculate the current predic_value value

	for(j=0; j<N-1; j++){
		tmp[0] += lamda[j]*(metrics_tmp->data[j+1][0]-metrics_tmp->data[j][0]);
		tmp[1] += lamda[j]*(metrics_tmp->data[j+1][1]-metrics_tmp->data[j][1]);
		tmp[2] += lamda[j]*(metrics_tmp->data[j+1][2]-metrics_tmp->data[j][2]);
		tmp[3] += lamda[j]*(metrics_tmp->data[j+1][3]-metrics_tmp->data[j][3]);
	}
	predic_value[0] = metrics_tmp->data[N-1][0]+tmp[0]/(lamda[N-1]);
	predic_value[1] = metrics_tmp->data[N-1][1]+tmp[1]/(lamda[N-1]);
	predic_value[2] = metrics_tmp->data[N-1][2]+tmp[2]/(lamda[N-1]);
	predic_value[3] = metrics_tmp->data[N-1][3]+tmp[3]/(lamda[N-1]);


	//printf("the predic_value: %lf %lf %lf %lf\n", predic_value[0], predic_value[1], predic_value[2], predic_value[3]);
	return predic_value;
}

/********************Initialize predic_value related functions*********************/

/********************Initialize R_value related functions*********************/
double *initial_R(double *metrics_data, double *predic_data, double *metrics_mean_data)
{
	int i;
	double tmp1,tmp2;
	double *R_value = (double *)malloc(4*sizeof(double));
	/*
	for(i=0; i<4; i++){
		tmp1 = predic_data[i]-metrics_f_data[i];
		tmp2 = metrics_data[i]-metrics_f_data[i];
		if(fabs(tmp1-tmp2)<0.001)
			R_value[i] = 1.0;
		else if(predic_data[i] == metrics_f_data[i])
				R_value[i] = 0;
		else
		{
			//printf("the vp - vn = %lf %lf\t  the vn+1- vn = %lf %lf\n",predic_data[i],metrics_f_data[i],metrics_data[i],metrics_f_data[i]);
			R_value[i] = (predic_data[i]-metrics_f_data[i])/(metrics_data[i]-metrics_f_data[i]);
			//R_value[i] = ((predic_data[i]-metrics_f_data[i]) - (metrics_data[i] - metrics_f_data[i]))/metrics_f_data[i];
			if(R_value[i]<0)
				R_value[i]=-(R_value[i]);
		}
	}
	*/

	for(i=0; i<4; i++){
		tmp1 = predic_data[i]-metrics_mean_data[i];
		tmp2 = metrics_data[i]-metrics_mean_data[i];
			//printf("the vp - vn = %lf %lf\t  the vn+1- vn = %lf %lf\n",predic_data[i],metrics_f_data[i],metrics_data[i],metrics_f_data[i]);
			//R_value[i] = (predic_data[i]-metrics_f_data[i])/(metrics_data[i]-metrics_f_data[i]);
		R_value[i] = (predic_data[i]- metrics_data[i] )/metrics_mean_data[i];
		if(R_value[i]<0)
			R_value[i]=-(R_value[i]);
	}
	return R_value;
}

// from 0 to N
double *get_mean(sequeue_t *queue)
{
	double *mean = (double *)malloc(4*sizeof(double));
	int i,j;
	for(i=0; i<4; i++){
		mean[i] = 0;
	}
	for(i=0;i<4;i++){
		for(j=0;j<N;j++){
			mean[i] += queue->data[j][i];
		}
	}
	
}

/********************Initialize R_value related functions*********************/

int main(){

	//open file
	FILE *fp1;
	fp1 = fopen("paper_result.txt","w");
	//Final variable
	double *A,*V;
	A = (double *)malloc(4*sizeof(double));
	V = (double *)malloc(4*sizeof(double));


	//Variables needed for initialization
	int i,j;
	double lamda[N];
	double mean[4],std[4];
	double *metrics_data, *predic_data, *R_data, *metrics_f_data; //Processing a single data
	//queue
	sequeue_t *metrics, *predic_values, *R_values;

	metrics_data = (double *)malloc(4*sizeof(double));
	predic_data = (double *)malloc(4*sizeof(double));
	R_data = (double *)malloc(4*sizeof(double));
	metrics_f_data= (double *)malloc(4*sizeof(double));

	//initial lamda
	for(i=0;i<N;i++){
		lamda[i] = 1/(double)N;
	}

	//Initialize metrics
	//Initialize the queue of metrics
	metrics = CreateEmptySequeue();
	//Initialize N metrics_data
	for(i=0;i<N;i++){
		memcpy(metrics_data, Initial_Metrics("eth0"),4*sizeof(double));
		if (NewEnQueue(metrics, metrics_data)==-1) return -1; //Enqueue
		//printf("\t\t%lf %lf %lf %lf\n",metrics_data[0],metrics_data[1],metrics_data[2],metrics_data[3]);

	}
	printQueue(metrics);

	
	if(FullSequeue(metrics) == 0) return -1;
	//Make sure to fill the entire metrics.

	//Initialize predic_values
	//Initialize R_values
	predic_values = CreateEmptySequeue();
	R_values = CreateEmptySequeue();

	//Start to initialize the predic queue, at the same time should also initialize the R queue
	for(i=0;i<N;i++){
		//Get predic_data, which is the first value
		memcpy(predic_data,initial_predic(metrics),4*sizeof(double));
		if(i==0){
			printf("the 1st predic_value is %lf %lf %lf %lf\n\n",predic_data[0],predic_data[1],predic_data[2],predic_data[3]);
		}
		if (NewEnQueue(predic_values,predic_data)==-1) return -1; //Enqueue
		



		//Update the time and update the metrics queue
		memcpy(metrics_f_data,metrics_data,4*sizeof(double));
		memcpy(metrics_f_data,get_mean(metrics),4*sizeof(double));
		memcpy(metrics_data, Initial_Metrics("eth0"),4*sizeof(double));

		//Initialize the R queue

		printf("the %d values are :\n",i);
		memcpy(R_data,initial_R(metrics_data, predic_data,metrics_f_data),4*sizeof(double));
		if(NewEnQueue(R_values, R_data) == -1) return -1;

		if (NewDeQueue(metrics) == NULL)  return -1;


		if (NewEnQueue(metrics,metrics_data) == -1) return -1;

		//test****************
		fprintf(fp1,"the %d metrics\n",i);
		fprintQueue(metrics,fp1);
		//test*****************
		
		fprintf(fp1,"the %d predic_value\n",i);
		fprintQueue(predic_values,fp1);
		fprintf(fp1,"the %d R_value\n",i);
		fprintQueue(R_values,fp1);
	}


	//After the first update, metrics are the latest metrics, but neither R nor predic_value are updated, so if you need to compare, you need to find the last metric to compare
	//printQueue(metrics);
	//printQueue(predic_values);
	//printQueue(R_values);

	if(FullSequeue(predic_values)==1) 
		printf("predic initialization completed！\n");


	//Initialize mean
	for(i=0; i<N; i++){
		for(j=0; j<4; j++){
			mean[j]+=lamda[i]*R_values->data[i][j]; //First calculate the cumulative sum
		}
	}


	//Initialize std
	for(i=0;i<N;i++){
		for(j=0;j<4;j++){
			std[j] += (R_values->data[i][j]-mean[j])*(R_values->data[i][j]-mean[j]);
		}
	}
	for(i=0;i<4;i++){
		std[j] = sqrt(std[j]/N);
	}

	//printQueue(metrics);
	printQueue(predic_values);
	//printQueue(R_values);

	for(i=0;i<4;i++){
		printf("the mean and std is %lf, %lf\n", mean[i], std[i]);
	}
	
	//fclose(fp1);
	
//for test
	int count_debug = 0;
	int attack_flag = 0;
	int count_flag;

	//Start normal operation, update predic and R each time, update mean and std at the same time, and then compare A and V. If V>A appears, an alarm will be issued, otherwise, continue
	

	while(1){


		//double * Initial_Metrics(char *ethernet_name);
		memcpy(metrics_f_data,metrics_data,4*sizeof(double));
		memcpy(metrics_f_data,get_mean(metrics),4*sizeof(double));
		//Update metrics queue
		if ((metrics) == NULL)  return -1;
		memcpy(metrics_data, Initial_Metrics("eth0"),4*sizeof(double));

		//if (NewDeQueue(metrics) == NULL) return -1;

		//memcpy(tmp1,NewDeQueue(metrics),4*sizeof(double));
		//printf("the dequeue values is %lf,%lf,%lf,%lf\n\n",tmp1[0],tmp1[1],tmp1[2],tmp1[3]);
		
		if (NewDeQueue(metrics) == NULL)  return -1;
		if (NewEnQueue(metrics,metrics_data) == -1) return -1;
		//Update predic queue
		memcpy(predic_data,initial_predic(metrics),4*sizeof(double));

		//printf("%p, %p\n", predic_data, initial_predic(metrics));
		if (NewDeQueue(predic_values) == NULL) return -1;
		//printf("debug2\n");
		if (NewEnQueue(predic_values,predic_data) == -1) return -1;
		//Update R queue
		memcpy(R_data,initial_R(metrics_data, predic_data,metrics_f_data),4*sizeof(double));
		//calculate if there is a suspected attack.
		for(i=0; i<4; i++){
			if(R_data[i]>mean[i]+3*std[i]) {
				printf("warning!!!!!!!\n\n");
				attack_flag =1;
				count_flag = count_debug;
			}
		}	


		if(NewDeQueue(R_values) == NULL) return -1;
		if(NewEnQueue(R_values, R_data) == -1) return -1;
		if(attack_flag ==1 ){
			fprintQueue(R_values,fp1);
		}

		if(count_debug == count_flag+13){
			attack_flag = 0;
			fclose(fp1);
		}


		//initialize mean and std;

		for(j=0; j<4; j++){
			mean[j]=0; 
	    }
		//update mean
		for(i=0; i<N; i++){
			for(j=0; j<4; j++){
				mean[j]+=lamda[i]*R_values->data[i][j]; //First calculate the cumulative sum
			}
		}
		for(i=0;i<4;i++){
			mean[i] = mean[i];
		}

		//initialize std;
		for(j=0; j<4; j++){
			std[j]=0; 
		}	
	    //update std
		for(i=0;i<N;i++){
			for(j=0;j<4;j++){
				std[j] += (R_values->data[i][j]-mean[j])*(R_values->data[i][j]-mean[j]);
			}
		}
		for(i=0;i<4;i++){
			std[j] = sqrt(std[j]/N);
		}


		//

		//for(i=0;i<4;i++){
		//	printf("the mean and std is %lf, %lf\n", mean[i], std[i]);
		//}
		//printf("/****************************************************/\n");
		//printQueue(predic_values);
		//printQueue(metrics);
		//printQueue(R_values);

		count_debug++;

	}

 	return 0;
}
