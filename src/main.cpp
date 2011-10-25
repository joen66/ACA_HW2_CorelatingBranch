#include<iostream>
#include<set>
#include<iomanip>
#include<map>
#include<cmath>
#include<algorithm>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;
class Predictor{
	public:
		virtual bool getPrediction(int pc)	=0;
		virtual void pushResult(int pc, bool result) =0;
		virtual bool getPush(int pc, bool result) =0;
};

struct AssignColumn{
	int pc;
	int whichM;
	AssignColumn(int tpc, int twhichM):pc(tpc), whichM(twhichM){}
};

struct CmpColumn{
	bool operator()(AssignColumn a, AssignColumn b){
		if(a.pc < b.pc)
			return true;
		else if(a.pc > b.pc)
			return false;
		else
			if(a.whichM < b.whichM)
				return true;
			else
				return false;
	}
};
class BTB{
	struct Entry{
		int brPC,target;
		Entry(int tbrPC, int ttarget): brPC(tbrPC), target(ttarget){};
	};
	vector<Entry> btb;
	protected:
	inline int trans(int pc){ return pc % btb.size(); }
	public:
	static const int NOADDR = -1;
	static const int CONFLICT_ADDR = -2;
	BTB(int NOEntry): btb(NOEntry, Entry(NOADDR,NOADDR) ){};
	inline int getTargetAddress(int brPC){
		if(btb[ trans(brPC) ].brPC == NOADDR){
			return NOADDR;
		}
		else if(btb[ trans(brPC) ].brPC !=  brPC)
			return CONFLICT_ADDR;
		else
			return btb[ trans(brPC) ].target;
	} 
	inline void setTargetAddress(int brPC, int target){
		btb[ trans(brPC) ].brPC = brPC;
		btb[ trans(brPC) ].target = target;
	}
	inline int getSet(int brPC,int target){
		int returnAddr = getTargetAddress(brPC);
		setTargetAddress(brPC, target);
		return returnAddr;
	}
	inline int getNOEntry(){ return btb.size(); }
};
class Corelating:  public Predictor {
	map< AssignColumn, char , CmpColumn> predBit;
	const int m,n;
	int lastResult;
	protected:
	int BHTtrans(int pc){
		int lowBits= log(predBit.size())/log(2) - m;
		int mask= pow(2,lowBits), a, b;
		a= pc&(mask-1);
		b= (lastResult << lowBits) | a;
		return b;
	}
	public:
	Corelating(int tm, int tn):m(tm), n(tn), lastResult(0){};
	void setLastResult(int tlastResult){ lastResult = tlastResult; }
	inline int getLastResult(){ return lastResult; }
	inline int getM()const { return m; }
	inline int getN()const { return n; }
	bool getPrediction(int pc){
		AssignColumn assignColumn(pc, getLastResult() );
		if( predBit.find( assignColumn ) == predBit.end() ){
			predBit.insert( pair<AssignColumn, char>(assignColumn , 0) );
		}
		return predBit[assignColumn];
	}
	void pushResult(int pc, bool result){
		AssignColumn assignColumn(pc, getLastResult() );
		if( predBit.find( assignColumn ) == predBit.end() ){
			predBit.insert( pair<AssignColumn, char>(assignColumn , 0));
		}
		if(result)
			predBit[assignColumn] = predBit[assignColumn] + 1 <4 ? predBit[assignColumn] + 1 : predBit[assignColumn];
		else
			predBit[assignColumn] = predBit[assignColumn] - 1 >-1 ? predBit[assignColumn] - 1 : predBit[assignColumn];
			
	}
	bool getPush(int pc, bool result){
		int pred = getPrediction(pc);
		pushResult(pc, result);
		return pred;
	}
};

class BranchPredictor{
	public:
	struct BranchInfo{
		int pc, correct, totalTimes;
		BranchInfo():pc(BTB::NOADDR), correct(0), totalTimes(0){}
	};
	private:
	map<int, BranchInfo> brInfoMap;
	map<int, int> brConflict;
	set<int> brConflictAddr;
	BranchInfo msb;
	Predictor& ptor;
	BTB& btb;
	int predCorrect, btbHit, amount, compulsoryMiss, totalMissSeen;
	
	public:
	BranchPredictor(Predictor& tptor, BTB& tbtb): ptor(tptor), btb(tbtb), predCorrect(0), btbHit(0), amount(0),compulsoryMiss(0) , totalMissSeen(0){}

	void next(int pc, int target, int result){
		bool pred = ptor.getPush(pc, result);
		if( brInfoMap.find(pc) == brInfoMap.end() ){
			++compulsoryMiss;
		}
		if(pred == result){
			++predCorrect;
			++brInfoMap[pc].correct;
		}

		int btbTarget = btb.getSet(pc,target);
		if(btbTarget != BTB::CONFLICT_ADDR && btbTarget != BTB::NOADDR){
			++btbHit;
		}
		++amount;
		++brInfoMap[pc].totalTimes;
		map<int, int>::iterator iter = brConflict.find( pc%btb.getNOEntry() );
		if( iter == brConflict.end()){
			brConflict[ pc%btb.getNOEntry() ] = pc;
			++totalMissSeen;
		}
		else if(iter->second != pc ){
			++totalMissSeen; //brConflictAddr.insert(pc);
			iter->second = pc;
		}

	}
	BranchInfo getMSB(){
		int maxTotalTimes=0;
		for(map<int, BranchInfo>::iterator iter= brInfoMap.begin(); iter!=brInfoMap.end(); ++iter){
			if( (iter->second).totalTimes > maxTotalTimes){
				msb.pc = iter->first;
				maxTotalTimes = msb.totalTimes = (iter->second).totalTimes;
				msb.correct = (iter->second).correct;
			}
		}
		return msb;
	}
	inline int getPredCorrect(){ return predCorrect; }
	inline int getBTBHit(){ return btbHit; }
	inline int getAmount(){ return amount; }
	inline int getCompulsoryMiss(){ return compulsoryMiss; }
	inline int getTotalMissSeen(){ return totalMissSeen; }
	inline void setPredCorrect(int tpredCorrect){ predCorrect=tpredCorrect; }
	inline void setAmount(int tamount){ amount=tamount; }
	inline void setBTBHit(int tbtbHit){ btbHit=tbtbHit; }

};
int main(int argc, char* argv[]){
	int NOEntry, pc, target, result;
	const int m=2, n=1;
	if(argc<2){
		cout<<"Please input the number of entries: ";
		cin>>NOEntry;
	}
	else if(argc>2){
		cerr<<"Too many arguments"<<endl;
		exit(-1);
	}
	else{
		NOEntry = atoi( argv[1] );
	}
	Corelating cb(m, n);
	BTB btb(NOEntry);
	BranchPredictor bp(cb, btb);
	// bt - branch table
	ifstream fin("history.txt");
	while(fin>> hex >> pc >> target >> dec >> result){
		bp.next(pc, target, result);
	}
	/*
	fin.clear();
	fin.seekg(0, ios::beg);
	bp.setAmount(0);
	bp.setCorrect(0);
	while(fin>> hex >> pc >> target >> dec >> result){
		bp.next(pc, target, result);
	}
	*/
	BranchPredictor::BranchInfo msb = bp.getMSB();
	cout<< setiosflags(ios::fixed) << setprecision(1) ;
	cout<< "Exercise 2.13(a)" << endl;
	cout<< "\tNumber of hits BTB: " << bp.getBTBHit() << ". Total brs: "<< bp.getAmount() << ". Hit rate: " <<
		( double( bp.getBTBHit() ) /  bp.getAmount() ) * 100 <<"%"<<endl;
	cout<< "Exercise 2.13(b)" << endl;
	cout<< "\tIncorrect predictions: " << bp.getAmount()-bp.getPredCorrect() << " of " << bp.getAmount() << ", or " <<
		( (double(bp.getAmount()) - bp.getPredCorrect() ) / bp.getAmount() ) *100 << "%" <<endl;
	cout<< "Exercise 2.13(c)" << endl;
	cout<< "\t $ sort -n history.txt | uniq -c | sort -n | tail -1"<<endl;
	cout<< "\t先以address of branch instruction排序，再利用uniq算出同樣的有幾個，再以此結果排序，取最大的那個輸出" << endl;
	//    634 0x4012c681	 0x4012c6b3	0
	cout<< "\tMost significant branch is seen " << msb.totalTimes << " times, out of " << bp.getAmount() << " total brs ;"
		<< ( double(msb.totalTimes) / bp.getAmount() ) *100 << "%" << endl;
	cout<< "\tMS branch=0x"<< hex << msb.pc << dec << ", correct predictions=" << msb.correct << " (of " <<
		msb.totalTimes << " total correct preds)  or " << (double(msb.correct) / msb.totalTimes) *100 << "%" <<endl;
	cout<< "Exercise 2.1.3(d)" <<endl;
	cout<< "\tTotal unique branches (1 miss per br compulsory): " << bp.getCompulsoryMiss() <<"."<<endl;
	cout<< "\tTotal misses seen: " << bp.getTotalMissSeen() <<". So total capacity misses = total misses - compulsory misses = " 
		<< bp.getTotalMissSeen() - bp.getCompulsoryMiss() <<"."<<endl;

	fin.clear();
	fin.seekg(0, ios::beg);
	bp.setAmount(0);
	bp.setPredCorrect(0);
	bp.setBTBHit(0);
	while(fin>> hex >> pc >> target >> dec >> result){
		bp.next(pc, target, result);
	}

	cout<< "Exercise 2.13 (e)" <<endl;

	cout<< "\tNumber of hits BTB: " << bp.getBTBHit() << ". Total brs: "<< bp.getAmount() << ". Hit rate: " <<
		( double( bp.getBTBHit() ) /  bp.getAmount() ) * 100 <<"%"<<endl;
	cout<< "\tIncorrect predictions: " << bp.getAmount()-bp.getPredCorrect() << " of " << bp.getAmount() << ", or " <<
		( (double(bp.getAmount()) - bp.getPredCorrect() ) / bp.getAmount() ) *100 << "%" <<endl;

	cout<< "Exercise 2.13 (f)" <<endl;
	cout<< "\tBTB Length   mispredict rate  miss rate" << endl;
	for(int i=1 ; i<=64 ; i*=2){
		fin.clear();
		fin.seekg(0, ios::beg);

		Corelating tcb(m, n);
		BTB tbtb(i);
		BranchPredictor tbp(tcb, tbtb);

		while(fin>> hex >> pc >> target >> dec >> result){
			tbp.next(pc, target, result);
		}
		cout<< setiosflags(ios::fixed) << setprecision(1) ;
		cout<<'\t'<< setw(10) << i << setw(17) << ( (double(tbp.getAmount()) - tbp.getPredCorrect() ) / tbp.getAmount() ) *100 << "%"   << setw(11) << ( (double(tbp.getAmount()) - tbp.getBTBHit() ) / tbp.getAmount() ) *100 << "%" <<endl;
	
	}
}
