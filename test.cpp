#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc,char *argv[]){
	// 参数没带文件名
	if(argc != 2){
		cerr << "Usage: " << argv[0] << " <filename>" << endl;
		return 1;
	}

	string filename = argv[1];

	ifstream in;
	in.open(filename.c_str());
	if (!in) {	// 文件打开错误
		cerr << "Open file " << filename << " error!" << endl;
		return 1;
	}

	int field;
	cin >> field;

	stringstream s;
	map<string,int> m;
	string line;
	string word;
	while (in >> line) {
		s.clear();
		s.str(line);

		for (int i = 0; i < 42 && s; ++i) {
			getline(s, word, ',');	// 按逗号分隔，一个字段一个字段读取

			if(i==field)
				++m[word];
		}
		//++m[line];
	}

	int count = 1;
	for(map<string,int>::iterator it=m.begin();it!=m.end();++it){
		cout << "(" << count++ << ")" <<  it->first << ": " << it->second << endl;
	}

	return 0;
}


