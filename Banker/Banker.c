#include <stdio.h>
#define  RES_NUM 3
#define  PRO_NUM 5
#define  PRO_REQ 3
#define  true    1
#define  false   0
typedef  char  bool;
unsigned Request[RES_NUM]             = {0,0,0};
unsigned Available[RES_NUM]           = {2,3,3};
unsigned Max[PRO_NUM][RES_NUM]        = {{5,5,9},{5,3,6},{4,0,11},{4,0,5},{4,2,4}};
unsigned Need[PRO_NUM][RES_NUM];
unsigned Allocation[PRO_NUM][RES_NUM] = {{2,1,2},{4,0,2},{4,0,5},{2,0,4},{3,1,4}};
unsigned Work[RES_NUM];
bool     Finish[PRO_NUM];
bool Security(unsigned *Sequence) {
	unsigned i,j;
	for(j=0;j<RES_NUM;j++) Work[j] = Available[j];
	for(i=0;i<PRO_NUM;i++) Finish[i] = false;
	for(i=0;i<PRO_NUM;i++) {
		if(Finish[Sequence[i]]) continue;
		for(j=0;j<RES_NUM;j++)
			if(!(Finish[Sequence[i]] = Need[Sequence[i]][j]<=Work[j])) break;
		if(Finish[Sequence[i]]) {
			for(j=0;j<RES_NUM;j++)
				Work[j] += Allocation[Sequence[i]][j];
		} else return false;
	}
	return true;
}
bool Perm(int n,bool *flag,int *Sequence){
	int i,j,x=0;
	if(n==0) return flag[PRO_NUM];
	for(i=0;i<PRO_NUM;i++) {
		if(flag[i]) continue;
		/////////////////////i的全排列/////////////////////////
		Sequence[PRO_NUM-n] = i;
		if(n==1) {
			if(Security(Sequence)) {
				flag[PRO_NUM] = true;
				for(j=0;j<PRO_NUM;j++) {
					printf("%d ",Sequence[j]+1);
				}
				printf("\n");
			}
		}
		/////////////////////i的全排列/////////////////////////
		flag[i] = true;
		Perm(n-1,flag,Sequence);
		flag[i] = false;
		if(++x==n) break;
	}
	return flag[PRO_NUM];
}
int banker(int i,unsigned *Request) {
	bool     flag[PRO_NUM+1] = {false};
	unsigned Sequence[PRO_NUM];
	int j;
	for(j=0;j<RES_NUM;j++) {
		if(Request[j]>Need[i][j]) return 2;
		if(Request[j]>Available[j]) return 1;
	}
	for(j=0;j<RES_NUM;j++) {
		Available[j] -= Request[j];
		Allocation[i][j] += Request[j];
		Need[i][j] -= Request[j];
	}
	if(Perm(PRO_NUM,flag,Sequence)) return 0;
	else {
		for(j=0;j<RES_NUM;j++) {
			Need[i][j] += Request[j];
			Allocation[i][j] -= Request[j];
			Available[j] += Request[j];	
		}
		return 1;
	}
}
int main(int argc, char *argv[]) {
	int i,j;
	for(i=0;i<PRO_NUM;i++)
		for(j=0;j<RES_NUM;j++)
			Need[i][j] = Max[i][j] - Allocation[i][j];
	switch(banker(PRO_REQ,Request)) {
		case 2:
			printf("Request > Need !\n");
			break;
		case 1:
			printf("Progress block !\n");
			break;
		case 0:
			printf("Successful !\n");
			break;
		default:
			printf("banker error !\n");
	}
	return 0;
}