#ifdef _DEBUG
#pragma warning(disable : 4786)
#endif
#include <stdio.h>
#include <set>
#include <map>
#define START 0
#define SIZE  1024
typedef struct Partition{
	unsigned __int64 id;      // 分区号
	unsigned __int64 addr;    // 起始地址
	unsigned __int64 space;   // 大小(2^i)
	bool             stat;    // 状态
	/*bool operator< (const Partition &p) const { // 重载<运算符
		return addr < p.addr; // 从小到大排序
	};*/ // 一种方法是在结构体里重载<运算符，另一种是自定义比较函数重载()运算符并在声明时加入比较函数这一参数
	     // 在这里因为伙伴系统算法中并不需要重载<运算符，所以采用后一种方法
}Partition;
struct cmp { // 当节点键大于等于所要查找或插入的键时，返回0(false)，反之为1(true)，这是由STL内部源代码所决定的。
	bool operator()(const Partition &p1, const Partition &p2) const {
		return p1.addr < p2.addr;
	}
};
Partition temp = {0, START, SIZE, false};
std::set<Partition, cmp> Pftable;             // <起始地址,struct>
std::set<Partition, cmp>::iterator Pfit;
std::multimap<unsigned, Partition> Pbtable;   // <幂,struct>
std::multimap<unsigned, Partition>::iterator Pbit;
int FirstFit_Alloc(unsigned k) {
	Pfit = Pftable.begin();
	while((Pfit->space < k || Pfit->stat==true) && Pfit!=Pftable.end()) Pfit++;
	if(Pfit==Pftable.end()) return 1; // 没找到
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
	if(Pfit==Pftable.end()) return 1; // 没找到
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
	if((Pbit=Pbtable.lower_bound(i)) == Pbtable.end()) return 1; // 没有足够大的
	while(Pbit->second.stat == true) {
		Pbit++;
		if(Pbit == Pbtable.end()) return 2; // 够大的都在使用中
	}
	if(i == Pbit->first) { // 可直接分配
		Pbit->second.stat = true;
	}
	else if(Pbit->first > i) { // 先分块再分配
		Pbit->second.stat = true; // 锁定Pbit分区
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
				Pbtable.erase(Pbit); // 删除Pbit分区
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
	if((Pbit=Pbtable.lower_bound(a)) == Pbtable.end()) return 1; // 没找到key
	while(addr!=Pbit->second.addr) { // 先找到要释放的指针Pbit
		Pbit++;
		if(a != Pbit->first || Pbit == Pbtable.end()) return 2; // 没找到address
	}
	while(1) {
		merge = 0;
		Pbit->second.stat = false;
		next = prev = Pbit;
		i = Pbit->first;
		/*while(1) { // 当两侧都能合并时决定向前合并(merge=-1)还是向后合并(merge=1),以合并后的分区最大为准
			find_prev(prev);
			find_next(next);
			if(prev==Pbtable.end() || prev->first!=i || prev->second.stat==true) { // 不能继续向前合并
				if(next == Pbtable.end() || next->first!=i || next->second.stat==true) return 0;
				merge = 1;
				break;
			}
			if(next==Pbtable.end() || next->first!=i || next->second.stat==true) { // 不能继续向后合并
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