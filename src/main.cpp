#include<iostream>
#include<cmath>
#include<algorithm>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;

class Predictor{
	public:
		virtual bool getPrediction(int pc)  =0;
		virtual void pushResult(int pc, char result) =0;
		virtual bool getPush(int pc, char result) =0;
		virtual int getM() const =0;
		virtual int getN() const =0;
};


class Corelating:  public Predictor {
	vector<char> predBit; // prediction Bit- predction
	const int m,n;
	int lastResult;
	protected:
		int trans(int pc){
			int lowBits= log(predBit.size())/log(2) - m;
			int mask= pow(2,lowBits), a, b;
			a= pc&(mask-1);
			b= (lastResult << lowBits) | a;
			return b;
		}
	public:
		Corelating(int NOEntry, int tm, int tn):lastResult(0), m(tm) , n(tn), predBit(NOEntry*tn){
		};
		bool setLastResult(int tlastResult){ lastResult = tlastResult; }
		inline int getLastResult(){ return lastResult; }
		inline int getM()const { return m; }
		inline int getN()const { return n; }
		bool getPrediction(int pc){
			int column=trans(pc);
			vector<char> thisTime( predBit.begin()+column*n , predBit.begin()+(column+1)*n );
			switch( thisTime[0] ){
				case 0:
					return false;
					break;
				case 1:
					return true;
					break;
				default:
					return false;
			}
			return thisTime[0]==0? false:true;
		}
		void pushResult(int pc, char result){
			int bitBegin = trans(pc) * n;
			for(int i=bitBegin; i<bitBegin+n-1 ;++i ){
				predBit[i]=predBit[i+1];
			}
			predBit[ bitBegin+n-1 ] = result;
			if(result){
				lastResult = lastResult+1 <4 ? lastResult+1: lastResult;
			}
			else{
				lastResult = lastResult-1 >=0 ? lastResult-1: lastResult;
			}
		}
		bool getPush(int pc, char result){
			int pred = getPrediction(pc);
			pushResult(pc, result);
			return pred;
		}
};

class BranchPredictor{
	Predictor& ptor;
	int correct, amount;
	public:
		BranchPredictor(Predictor& tptor): ptor(tptor), correct(0), amount(0){}

		bool next(int pc, int target, int result){
			bool pred = ptor.getPush(pc, result);
			increTimes( pred==result );
			return pred==result;
		}
		inline void increTimes(bool a){ 
			++amount;
			if(a)
				++correct;
		}
		inline int getCorrect(){ return correct; }
		inline int getAmount(){ return amount; }
};
int main(int argc, char* argv[]){
	int NOEntry, pc, target, result, pred;
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
	Corelating cb(NOEntry, m, n);
	BranchPredictor bp(cb);
	// bt - branch table
	ifstream fin("history.txt");
	while(fin>> hex >> pc >> target >> dec >> result){
		pred = bp.next(pc, target, result);
	}
	cout << bp.getAmount() << " " << bp.getCorrect() <<endl;
}
