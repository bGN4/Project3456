#include <stdio.h>
#include <stdlib.h>
#include <vector>
typedef struct pcb{
	unsigned PID;
	unsigned ati;
	unsigned sti;
}PCB;
int cmp_by_ati(const void *a,const void *b) {
	PCB *x = (PCB*)a,*y = (PCB*)b;
	return x->ati - y->ati;
}
void spf(std::vector<PCB> *p){
	qsort(p->begin(),p->size(),sizeof(p->at(0)),&cmp_by_ati);
	unsigned num = 0,time = 0;
	std::vector<bool> sign(p->size(),false);
	for(int k=0;k<p->size();k++){
		unsigned nti = -1;
		printf("%d\t",p->at(num).PID);
		time += p->at(num).sti;
		sign[num] = true;
		for(int i=1;i<p->size();i++){
			if(sign[i]==true) continue;
			if(time < p->at(i).ati) break;
			else if(p->at(i).sti < nti){
				nti = p->at(i).sti;
				num = i;
			}
		}
	}
}
void rr(std::vector<PCB> *p){
	qsort(p->begin(),p->size(),sizeof(p->at(0)),&cmp_by_ati);
	unsigned siz = p->size(),num = -1,sum = 0;
	std::vector<unsigned> sign(p->size(),-1);
	for(int j=0;j<p->size();j++) sum+=p->at(j).sti;
	for(unsigned time=0;time<sum;time++){
		if(siz > 0){
			for(int i=0;i<p->size();i++){
				if(sign.at(i) < -1) continue;
				if(time < p->at(i).ati) break;
				sign.at(i) = p->at(i).sti;
				siz--;
			}
		}
LABEL1:
		num = (num<4) ? num+1 : 0;
		if(sign.at(num) != -1 && sign.at(num) != 0){
			printf("%d\t",p->at(num).PID);
			sign.at(num)--;
		}else goto LABEL1;
	}
}
void main(){
	std::vector<PCB> pcbv;
	PCB pcbx[] = {{1,0,3},{2,2,6},{3,4,4},{4,6,5},{5,8,2}};
	for(int i=0;i<5;i++) pcbv.push_back(pcbx[i]);
	spf(&pcbv);
	printf("\n");
	rr(&pcbv);
}