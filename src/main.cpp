#include<iostream>
#include<cmath>
#include<algorithm>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;
string itoa(int num, int bits){
	string str;
	while(bits){
		str += '0'+ num%2;
		num /= 2;
		--bits;
	}
	return string(str.rbegin(),str.rend());
}
int atoi(string str){
	int num=0,power=1;
	for(int i=str.size()-1; i>=0 ;--i){
		num += (str[i]-'0')*power;
		power *= 2;
	}
	return num;
}
class History{
	vector<char> predBit; // prediction Bit- predction
	const int m,n;
	int lastResult;
	int trans(int pc){
		int lowBits= log(predBit.size())/log(2) - m;
		int mask= pow(2,lowBits), a, b;
		a= pc&(mask-1);
		b= (lastResult << lowBits) | a;
		return b;

		/*
		int lowerBits=4;
		string pcstr = itoa(pc, lowerBits);
		string lastResultStr = itoa(lastResult, m);
		string str=lastResultStr + pcstr;
		return atoi(str);
		*/
	}
	public:
		History(int NOEntry):lastResult(0), m(2) , n(1), predBit(NOEntry*1){
		}
		bool getPrediction(int pc){
			int column=trans(pc);
<<<<<<< HEAD
=======
			/*
			cout<<column<<endl;
			for(int i=0; i<predBit.size();++i){
				cout<<predBit[i]+0;
				if(i%5==0)
					cout<<endl;
			}
			cout<<endl;
			*/
>>>>>>> 72231636a442822c1bf428f6e1ca2a595f21f89e
			vector<char> thisTime( predBit.begin()+column*n , predBit.begin()+(column+1)*n );
			/*
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
			*/
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
		int getLastResult(){ return lastResult; }
};

class CorelatingBranch{
	History history;
	int m, n,NOEntry;
	int correct, amount;
	public:
		CorelatingBranch(int tNOEntry): m(2), n(1),history(tNOEntry), correct(0), amount(0), NOEntry(tNOEntry){}

		int next(int pc, int target, int result){
			bool pred = history.getPrediction( pc );
<<<<<<< HEAD
			history.pushResult(pc, result);
			increTimes( pred==result );
=======
			if( pred==result ){
				//cout<<pred<<" "<<history.getLastResult()<<endl;
				history.pushResult(pc, result);
				increTimes(true);
			}
			else{
				//cout<<pred<<" "<<history.getLastResult()<<endl;
				history.pushResult(pc, result);
				increTimes(false);
			}
>>>>>>> 72231636a442822c1bf428f6e1ca2a595f21f89e
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
	int NOEntry;
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
	CorelatingBranch cb(NOEntry);
	// bt - branch table
	int right=0, amount=0;
	// right- the number of correct branches
	// amount- the number of branches
	string str;
	ifstream fin("history.txt");
	while(getline(fin, str)){
		istringstream sin(str);
		int pc, target, result, pred;
		sin >> hex >> pc >> target >>dec >>result;

		pred = cb.next(pc, target, result);
	}
	cout<<cb.getAmount()<<" "<<cb.getCorrect()<<endl;
}
