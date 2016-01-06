#ifdef _DEBUG
#pragma warning(disable : 4786)
#endif
#include <stdio.h>
#include <set>
#include <map>
#define START 0
#define SIZE  1024
typedef struct Partition{
	unsigned __int64 id;      // ������
	unsigned __int64 addr;    // ��ʼ��ַ
	unsigned __int64 space;   // ��С(2^i)
	bool             stat;    // ״̬
	/*bool operator< (const Partition &p) const { // ����<�����
		return addr < p.addr; // ��С��������
	};*/ // һ�ַ������ڽṹ��������<���������һ�����Զ���ȽϺ�������()�������������ʱ����ȽϺ�����һ����
	     // ��������Ϊ���ϵͳ�㷨�в�����Ҫ����<����������Բ��ú�һ�ַ���
}Partition;
struct cmp { // ���ڵ�����ڵ�����Ҫ���һ����ļ�ʱ������0(false)����֮Ϊ1(true)��������STL�ڲ�Դ�����������ġ�
	bool operator()(const Partition &p1, const Partition &p2) const {
		return p1.addr < p2.addr;
	}
};
Partition temp = {0, START, SIZE, false};
std::set<Partition, cmp> Pftable;             // <��ʼ��ַ,struct>
std::set<Partition, cmp>::iterator Pfit;
std::multimap<unsigned, Partition> Pbtable;   // <��,struct>
std::multimap<unsigned, Partition>::iterator Pbit;
int FirstFit_Alloc(unsigned k) {
	Pfit = Pftable.begin();
	while((Pfit->space < k || Pfit->stat==true) && Pfit!=Pftable.end()) Pfit++;
	if(Pfit==Pftable.end()) return 1; // û�ҵ�
	if(Pfit->space==k && Pfit->stat==false) {
		Pfit->stat = true;
		return 0;
	}
	else if(Pfit->space>k && Pfit->stat==false) {
		unsigned __int64 new_space = Pfit->space - k;
		Pfit->stat = true;
		temp.id = Pfit->id;
		temp.addr = Pfit->addr;
		temp.space = k;
		temp.stat = true;
		Pftable.erase(Pfit);
		std::pair<std::set<Partition, cmp>::iterator,bool> pr = Pftable.insert(temp);
		Pfit = pr.first;
		temp.id = 0;
		temp.addr = Pfit->addr + Pfit->space;
		temp.space = new_space;
		temp.stat = false;
		Pftable.insert(temp);
		return 0;
	}
	else return -1;
}
int FirstFit_Free(unsigned __int64 addr) {
	temp.addr = addr;
	Pfit = Pftable.find(temp);
	if(Pfit==Pftable.end()) return 1; // û�ҵ�
	std::set<Partition, cmp>::iterator tmp = Pfit;
	while(++tmp != Pftable.end()) {
		if(tmp->stat==true) break;
		temp.id = Pfit->id;
		temp.addr = Pfit->addr;
		temp.space = Pfit->space + tmp->space;
		temp.stat = false;
		Pftable.erase(Pfit);
		Pftable.erase(tmp);
		std::pair<std::set<Partition, cmp>::iterator,bool> pr = Pftable.insert(temp);
		tmp = Pfit = pr.first;
	}
	tmp = Pfit;
	while(tmp-- != Pftable.begin()) {
		if(tmp->stat==true) break;
		temp.id = tmp->id;
		temp.addr = tmp->addr;
		temp.space = Pfit->space + tmp->space;
		temp.stat = false;
		Pftable.erase(Pfit);
		Pftable.erase(tmp);
		std::pair<std::set<Partition, cmp>::iterator,bool> pr = Pftable.insert(temp);
		tmp = Pfit = pr.first;
	}
	Pfit->stat = false;
	return 0;
}
int BuddySystem_Alloc(unsigned k) {
	int i = 1;
	while(2<<i++ < k) ; // 2^(i-1) < k <= 2^i
	if((Pbit=Pbtable.lower_bound(i)) == Pbtable.end()) return 1; // û���㹻���
	while(Pbit->second.stat == true) {
		Pbit++;
		if(Pbit == Pbtable.end()) return 2; // ����Ķ���ʹ����
	}
	if(i == Pbit->first) { // ��ֱ�ӷ���
		Pbit->second.stat = true;
	}
	else if(Pbit->first > i) { // �ȷֿ��ٷ���
		Pbit->second.stat = true; // ����Pbit����
		for(unsigned j=Pbit->first-1 ; j>=i ; j--) {
			temp.id = 0;
			temp.space = 1 << j;
			temp.addr = Pbit->second.addr + temp.space;
			temp.stat = false;
			Pbtable.insert(std::pair<unsigned, Partition>(j,temp));
			if(j==i) {
				temp.id = 0;
				temp.space = 1 << j;
				temp.addr = Pbit->second.addr;
				temp.stat = true;
				Pbtable.erase(Pbit); // ɾ��Pbit����
				Pbit = Pbtable.insert(std::pair<unsigned, Partition>(j,temp));
			}
		}
	} else return -1;
	return 0;
}
void find_next(std::multimap<unsigned, Partition>::iterator &dst) {
	std::multimap<unsigned, Partition>::iterator tmp = dst;
	if(dst->second.addr+dst->second.space == START+SIZE) dst = Pbtable.end();
	else {
		for(tmp=Pbtable.begin() ; tmp!=Pbtable.end() ; tmp++) {
			if(dst->second.addr+dst->second.space==tmp->second.addr) {
				dst = tmp;
				break;
			}
		}
	}
	//printf("N:%u\t%I64u\t%I64u\t%d\n",dst->first,dst->second.addr,dst->second.space,dst->second.stat);
}
void find_prev(std::multimap<unsigned, Partition>::iterator &dst) {
	std::multimap<unsigned, Partition>::iterator tmp = dst;
	if(dst->second.addr == START) dst = Pbtable.end();
	else {
		for(tmp=Pbtable.begin() ; tmp!=Pbtable.end() ; tmp++) {
			if(tmp->second.addr+tmp->second.space==dst->second.addr) {
				dst = tmp;
				break;
			}
		}
	}
	//printf("P:%u\t%I64u\t%I64u\t%d\n",dst->first,dst->second.addr,dst->second.space,dst->second.stat);
}
unsigned __int64 Dichotomy(unsigned __int64 addr, unsigned __int64 space) {
	unsigned __int64 begin = START, end = START+SIZE, tmp;
	while(end-begin >= space<<1) {
		tmp = begin+(end-begin>>1);
		if(end-begin == space<<1) {
			if(begin==addr) return addr+space;
			else return begin;
		}
		if(tmp <= addr) begin = tmp;
		if(begin==addr) return addr+space;
		if(tmp >= addr+space) end = tmp;
		if(end==addr+space) return addr-space;
	}
	return 0;
}
int BuddySystem_Free(unsigned a, unsigned __int64 addr) {
	unsigned __int64 s;
	unsigned i = 0;
	char merge = 0;
	std::multimap<unsigned, Partition>::iterator prev;
	std::multimap<unsigned, Partition>::iterator next;
	if((Pbit=Pbtable.lower_bound(a)) == Pbtable.end()) return 1; // û�ҵ�key
	while(addr!=Pbit->second.addr) { // ���ҵ�Ҫ�ͷŵ�ָ��Pbit
		Pbit++;
		if(a != Pbit->first || Pbit == Pbtable.end()) return 2; // û�ҵ�address
	}
	while(1) {
		merge = 0;
		Pbit->second.stat = false;
		next = prev = Pbit;
		i = Pbit->first;
		/*while(1) { // �����඼�ܺϲ�ʱ������ǰ�ϲ�(merge=-1)�������ϲ�(merge=1),�Ժϲ���ķ������Ϊ׼
			find_prev(prev);
			find_next(next);
			if(prev==Pbtable.end() || prev->first!=i || prev->second.stat==true) { // ���ܼ�����ǰ�ϲ�
				if(next == Pbtable.end() || next->first!=i || next->second.stat==true) return 0;
				merge = 1;
				break;
			}
			if(next==Pbtable.end() || next->first!=i || next->second.stat==true) { // ���ܼ������ϲ�
				merge = -1;
				break;
			}
			i++;
		}*/
		find_prev(prev);
		find_next(next);
		s = Dichotomy(Pbit->second.addr, Pbit->second.space);
		if(prev!=Pbtable.end() && prev->first==i && prev->second.stat==false) {
			if(s==prev->second.addr) merge = -1;
		}
		if(next!=Pbtable.end() && next->first==i && next->second.stat==false) {
			if(s==next->second.addr) merge = 1;
		}
		if(!merge) return 0;
		next = prev = Pbit;
		if(merge<0) find_prev(prev);
		else find_next(next);
		if(prev!=Pbtable.end() && next!=Pbtable.end() && prev->first==next->first && prev->second.stat==false && next->second.stat==false) {
			int x = Pbit->first;
			prev->second.stat = true;
			next->second.stat = true;
			temp.id = prev->second.id;
			temp.addr = prev->second.addr;
			temp.space = prev->second.space << 1;
			temp.stat = false;
			Pbtable.erase(prev);
			Pbtable.erase(next);
			Pbit = Pbtable.insert(std::pair<unsigned, Partition>(x+1,temp));
		}
	}
}
void main(int argc,char* argv[]) {
	Pftable.insert(temp);
	Pbtable.insert(std::pair<unsigned, Partition>(10,temp));
	while(1) {
		unsigned b,k;
		scanf("%d %d",&b,&k);
		if(b==0) printf("Alloc return : %d.\n",FirstFit_Alloc(k));
		else printf("Free return : %d.\n",FirstFit_Free(k));
		for(Pfit=Pftable.begin() ; Pfit!=Pftable.end() ; Pfit++)
			printf("%I64u\t%I64u\t%I64u\t%d\n",Pfit->id,Pfit->addr,Pfit->space,Pfit->stat);
		/*if(b==0) printf("Alloc return : %d.\n",BuddySystem_Alloc(k));
		else printf("Free return : %d.\n",BuddySystem_Free(b,k));
		for(Pbit=Pbtable.begin() ; Pbit!=Pbtable.end() ; Pbit++) {
			printf("%I64u\t%I64u\t%I64u\t%d\n",Pbit->second.id,Pbit->second.addr,Pbit->second.space,Pbit->second.stat);
		}*/
	}
}