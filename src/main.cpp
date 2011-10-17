#include<iostream>
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
	int m,n;
	int lastResult;
	int trans(int pc){
		int lowerBits=4;
		string pcstr = itoa(pc, lowerBits);
		string lastResultStr = itoa(lastResult, m);
		string str=lastResultStr + pcstr;
		cout << pc <<" "<<pcstr << " " << lastResultStr<< " " <<str<<endl;
		return atoi(str);
	}
	public:
		History(int NOEntry):lastResult(0), m(2) , n(1), predBit(NOEntry*1){
		}
		bool getPrediction(int pc){
			int whichM=trans(pc);
			vector<char> thisTime( predBit.begin()+whichM*n , predBit.begin()+(whichM+1)*n );
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
		}
		void pushResult(int whichM, char result){
			for(int i=whichM*n; i<whichM*(n+1) ;++i ){
				predBit[i]=predBit[i+1];
			}
			predBit[ whichM*(n+1)-1 ] = result;
		}
};

class CorelatingBranch{
	History history;
	int m, n,NOEntry;
	int accuracy, amount;
	public:
		CorelatingBranch(int tNOEntry): m(2), n(1),history(tNOEntry), accuracy(0), amount(0), NOEntry(tNOEntry){}
		int next(int pc, int target, int result){
			history.getPrediction( pc );
		}
};
int main(int argc, char* argv[]){
	int noEntry;
	if(argc<2){
		cout<<"Please input the number of entries: ";
		cin>>noEntry;
	}
	else if(argc>2){
		cerr<<"Too many arguments"<<endl;
		exit(-1);
	}
	else{
		noEntry = atoi( argv[1] );
	}
	CorelatingBranch cb(noEntry);
	// bt - branch table
	int right=0, amount=0;
	// right- the number of correct branches
	// amount- the number of branches
	string str;
	ifstream fin("history.txt");
	while(getline(fin, str)){
		istringstream sin(str);
		int pc, target, result;
		sin >> hex >> pc >> target >>dec >>result;

		cb.next(pc, target, result);
		cin >>pc;
	}
}
