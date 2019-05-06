#include <stdio.h>
#include <math.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
void selectionsort(int arr[],int l,int r)
{
	int i,j,t,minpos;
	for(i=l;i<=r;i++)
	{
		minpos=i;
		for(j=i+1;j<=r;j++)
		{
			if(arr[j]<arr[minpos])
				minpos=j;
		}
		t=arr[i];
		arr[i]=arr[minpos];
		arr[minpos]=t;
	}
}	
void merge(int arr[],int l,int r)
{
	int left[1000],right[1000],i,j,p=l;
	for(i=l;i<=(l+r)/2;i++)
		left[i-l]=arr[i];
	for(i=(l+r)/2+1;i<=r;i++)
		right[i-(l+r)/2-1]=arr[i];
	i=0;
	j=0;
	while(i<((l+r)/2-l+1) && j<(r-(l+r)/2) )
	{
		if(left[i]<=right[j])
		{
			arr[p]=left[i];
			i++;
			p++;
		}
		else
		{
			arr[p]=right[j];
			j++;
			p++;
		}
	}
	while(i<(l+r)/2-l+1)
	{
		arr[p]=left[i];
		i++;
		p++;
	}
	while(j<r-(l+r)/2)
	{
		arr[p]=right[j];
		j++;
		p++;
	}
}
void mergesort(int arr[],int l,int r)
{
	if(l>=r)
		return ;
	if(r-l+1<=5)
	{
		selectionsort(arr,l,r);
		return;
	}
	int lpid,rpid;
	lpid=fork();
	if(lpid<0)
	{
		perror("Fork failed\n");
		exit(1);
	}
	else if(lpid==0)
	{
		mergesort(arr,l,(l+r)/2);
		exit(0);
	}
	else
	{
		rpid=fork();
		if(rpid<0)
		{
			perror("Fork failed\n");
			exit(1);
		}
		else if(rpid==0)
		{
			mergesort(arr,(l+r)/2+1,r);
			exit(0);
		}
	}
	int status;

	waitpid(lpid,&status,0);
	waitpid(rpid,&status,0);

	merge(arr,l,r);
}
int main()
{
	int shmid;
	key_t key = IPC_PRIVATE;

	int n,i;
	int *arr;
	scanf("%d",&n);

	shmid = shmget(key, sizeof(int)*n, IPC_CREAT | 0666);
	if(shmid==-1)
	{
		perror("shmget");
		exit(1);
	}
	arr=shmat(shmid,0,0);
	if(arr == (void *)-1){
		perror("Shmat failed");
		exit(1);
	}

	for(i=0;i<n;i++)
		scanf("%d",&arr[i]);

	mergesort(arr,0,n-1);

	for(i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");

	if (shmdt(arr) == -1)
	{
		perror("shmdt");
		exit(1);
	}

	if (shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		perror("shmctl");
		exit(1);
	}
	return 0;
}
