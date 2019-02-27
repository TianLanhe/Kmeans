#include "strmyrecord/strmyrecord.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int stringToInt(std::string str) {
	std::stringstream s(str);
	int ret;
	s >> ret;
	return ret;
}

double stringToDouble(std::string str) {
	std::stringstream s(str);
	double ret;
	s >> ret;
	return ret;
}

int main(int argc, char *argv[]) {
	// 命令行参数带待读取和处理的文件名
	if (argc != 2) {
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

	cout << "Start reading records from " << filename << ".." << endl;
	vector<strMyRecord> records;
	strMyRecord record;
	stringstream s;
	string line, word;
	// 读取一行
	while (in >> line) {
		s.clear();
		s.str(line);

		for (int i = 0; i < 42 && s; ++i) {
			getline(s, word, ',');	// 按逗号分隔，一个字段一个字段读取

			switch (i) {
			case 1:	//协议类型
				//record.iProtocolType = getProtolType(word);
				record.SetFieldValue(0, getProtolType(word));
				break;
			case 2:// 服务类型
				//record.iService = getService(word);
				record.SetFieldValue(1, getService(word));
				break;
			case 3:// 状态标志
				//record.iStatusFlag = getStatusFlag(word);
				record.SetFieldValue(2, getStatusFlag(word));
				break;
			case 4:// 源到目的字节数
				//record.iSrcBytes = stringToInt(word);
				record.SetFieldValue(3, stringToInt(word));
				break;
			case 5:// 目的到源字节数
				//record.iDestBytes = stringToInt(word);
				record.SetFieldValue(4, stringToInt(word));
				break;
			case 10://登录失败次数
				//record.iFailedLogins = stringToInt(word);
				record.SetFieldValue(5, stringToInt(word));
				break;
			case 15:// root用户权限存取次数
				//record.iNumofRoot = stringToInt(word);
				record.SetFieldValue(6, stringToInt(word));
				break;
			case 22:// 2s内连接相同主机数目
				//record.iCount = stringToInt(word);
				record.SetFieldValue(7, stringToInt(word));
			case 23:// 2s内连接相同端口数目
				//record.iSvrCount = stringToInt(word);
				record.SetFieldValue(8, stringToInt(word));
				break;
			case 26:// "REJ"错误的连接数比例
				//record.iRerrorRate = int(100 * stringToDouble(word));
				record.SetFieldValue(9, int(100 * stringToDouble(word)));
				break;
			case 28:// 连接到相同端口数比例
				//record.iSameSrvRate = int(100 * stringToDouble(word));
				record.SetFieldValue(10, int(100 * stringToDouble(word)));
				break;
			case 29:// 连接到不同端口数比例
				//record.iDiffSrvRate = int(100 * stringToDouble(word));
				record.SetFieldValue(11, int(100 * stringToDouble(word)));
				break;
			case 32:// 相同目的地相同端口连接数
				//record.iDstHostSrvCount = stringToInt(word);
				record.SetFieldValue(12, stringToInt(word));
				break;
			case 33:// 相同目的地相同端口连接数比例
				//record.iDstHostSameSrvRate = int(100 * stringToDouble(word));
				record.SetFieldValue(13, int(100 * stringToDouble(word)));
				break;
			case 34:// 相同目的地不同端口连接数比例
				//record.iDstHostDiffSrvRate = int(100 * stringToDouble(word));
				record.SetFieldValue(14, int(100 * stringToDouble(word)));
				break;
			case 35:// 相同目的地相同源端口连接数比例
				//record.iDstHostSameSrcPortRate = int(100 * stringToDouble(word));
				record.SetFieldValue(15, int(100 * stringToDouble(word)));
				break;
			case 36:// 不同主机连接相同端口比例
				//record.iDstHostSrvDiffHostRate = int(100 * stringToDouble(word));
				record.SetFieldValue(16, int(100 * stringToDouble(word)));
				break;
			case 38:// 不同主机连接相同端口比例
				//record.iDstHostSrvSerrorRate = int(100 * stringToDouble(word));
				record.SetFieldValue(17, int(100 * stringToDouble(word)));
				break;
			case 41:	//标签类型
				//record.iLabel = getLabel(word);
				record.SetLabel(getLabel(word));
				break;
			}
		}

		records.push_back(record);

		if (records.size() % 10000 == 0)
			cout << "---------------" << records.size() << " lines have read" << "---------------" << endl;
	}
	cout << "All records have read! Total " << records.size() << " records!" << endl;
	in.close();

	// 输出文件名在输出文件名的后面加"_datatreat"
	filename.append("_datatreat");

	ofstream out(filename.c_str());
	if (!out) {
		cout << "open output file " << filename << " error" << endl;
		return 1;
	}

	int count = 0;
	cout << "Start writing records into " << filename << "..." << endl;
	for (vector<strMyRecord>::const_iterator cit = records.begin(); cit != records.end(); ++cit) {
		out << *cit << endl;

		++count;
		if (count % 10000 == 0)
			cout << "---------------" << count << " lines have writen" << "---------------" << endl;
	}
	cout << "All records have writen!" << endl;

	return 0;
}
