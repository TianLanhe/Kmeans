#include "strmyrecord.h"

#include <string>
#include <ostream>
#include <istream>
#include <sstream>

using namespace std;

int strMyRecord::_stringToInt(const string& str) {
	std::stringstream s(str);
	int ret;
	s >> ret;
	return ret;
}

BYTE getProtolType(const std::string& str) {
	if (str == "tcp")
		return 1;
	else if (str == "udp")
		return 2;
	else if (str == "icmp")
		return 3;
	else
		return 0;
}

BYTE getService(const std::string& str) {
	const static char *services[67] = { "http","smtp","finger","domain_u","auth","telnet",
		"ftp","eco_i","ntp_u","ecr_i","other","private","pop_3","ftp_data","rje","time","mtp","link",
		"remote_job","gopher","ssh","name","whois","domain","login","imap4","daytime","ctf","nntp",
		"shell","IRC","nnsp","http_443","exec","printer","efs","courier","uucp","klogin","kshell",
		"echo","discard","systat","supdup","iso_tsap","hostnames","csnet_ns","pop_2","sunrpc",
		"uucp_path","netbios_ns","netbios_ssn","netbios_dgm","sql_net","vmnet","bgp","Z39_50",
		"ldap","netstat","urh_i","X11","urp_i","pm_dump","tftp_u","tim_i","red_i","icmp" };
	for (int i = 0; i < 67; ++i)
		if (str == services[i])
			return i + 1;
	return 0;
}

BYTE getStatusFlag(const std::string& str) {
	const static char *flags[11] = { "SF", "S1", "REJ", "S2", "S0", "S3", "RSTO", "RSTR", "RSTOS0", "OTH", "SH" };
	for (int i = 0; i < 11; ++i)
		if (str == flags[i])
			return i + 1;
	return 0;
}

BYTE getLabel(const std::string& str) {
	// 将 40 种标签分成 5 种类型，分别是 1：正常，2：DOS(拒绝服务攻击)，3：R2L(来自于远程主机的非法入侵)，4：U2R(非法获得本地主机的超级用户权限)，5：Probing(监视与探测)
	const static char *labels[5][16] = { { "normal." },//1：正常
	{ "back.","land.","neptune.","pod.","smurf.","teardrop."/*后面的自己判断类型*/,"apache2.","mailbomb.","udpstorm.","processtable." },//2：DOS(拒绝服务攻击)
	{ "ftp_write.","guess_passwd.","imap.","multihop.","phf.","spy.","warezmaster.","warezclient."/*后面的自己判断类型*/,"snmpguess.","snmpgetattack.","sendmail.","httptunnel.","worm.","xlock.","xsnoop.","named." },//3：R2L(来自于远程主机的非法入侵)
	{ "buffer_overflow.","loadmodule.","perl.","rootkit."/*后面的自己判断类型*/,"xterm.","ps.","sqlattack." },//4：U2R(非法获得本地主机的超级用户权限)
	{ "ipsweep.","nmap.","portsweep.","satan."/*后面的自己判断类型*/,"mscan.","saint." }//5：Probing(监视与探测)
	};

	// 下面这些攻击是未知的类型，自己判断
	//,"snmpgetattack.","httptunnel.","ps.","apache2.","xterm.","xsnoop.","xlock."
	//,"worm.","udpstorm.","sqlattack.","snmpguess.","sendmail.","saint.","processtable.","mailbomb.","named.","mscan."

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 16; ++j)
			if (labels[i][j] && str == labels[i][j])
				return i + 1;
	return 0;
}

string getLabelName(BYTE iLabel) {
	static const char* strs[6] = { "unknown", "normal","dos","r2l","u2r","probe" };
	return strs[iLabel];
}

ostream& operator<<(ostream & out, const strMyRecord & record) {
	if (out) {
		out << int(record.iProtocolType);
		out << ',' << int(record.iService);
		out << ',' << int(record.iStatusFlag);
		out << ',' << int(record.iSrcBytes);
		out << ',' << int(record.iDestBytes);
		out << ',' << int(record.iFailedLogins);
		out << ',' << int(record.iNumofRoot);
		out << ',' << int(record.iCount);
		out << ',' << int(record.iSvrCount);
		out << ',' << int(record.iRerrorRate);
		out << ',' << int(record.iSameSrvRate);
		out << ',' << int(record.iDiffSrvRate);
		out << ',' << int(record.iDstHostSrvCount);
		out << ',' << int(record.iDstHostSameSrvRate);
		out << ',' << int(record.iDstHostDiffSrvRate);
		out << ',' << int(record.iDstHostSameSrcPortRate);
		out << ',' << int(record.iDstHostSrvDiffHostRate);
		out << ',' << int(record.iDstHostSrvSerrorRate);
		out << ',' << int(record.iLabel);
	}
	return out;
}

istream& operator >> (istream &in, strMyRecord &record) {
	if (in) {
		stringstream s;
		string line;
		string word;
		in >> line;
		s.str(line);

		getline(s, word, ',');	// 按逗号分隔，一个字段一个字段读取
		record.iProtocolType = record._stringToInt(word);

		getline(s, word, ',');
		record.iService = record._stringToInt(word);

		getline(s, word, ',');
		record.iStatusFlag = record._stringToInt(word);

		getline(s, word, ',');
		record.iSrcBytes = record._stringToInt(word);

		getline(s, word, ',');
		record.iDestBytes = record._stringToInt(word);

		getline(s, word, ',');
		record.iFailedLogins = record._stringToInt(word);

		getline(s, word, ',');
		record.iNumofRoot = record._stringToInt(word);

		getline(s, word, ',');
		record.iCount = record._stringToInt(word);

		getline(s, word, ',');
		record.iSvrCount = record._stringToInt(word);

		getline(s, word, ',');
		record.iRerrorRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iSameSrvRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iDiffSrvRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iDstHostSrvCount = record._stringToInt(word);

		getline(s, word, ',');
		record.iDstHostSameSrvRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iDstHostDiffSrvRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iDstHostSameSrcPortRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iDstHostSrvDiffHostRate = record._stringToInt(word);

		getline(s, word, ',');
		record.iDstHostSrvSerrorRate = record._stringToInt(word);

		if (getline(s, word, ','))
			record.iLabel = record._stringToInt(word);
		else
			record.iLabel = 0;
	}
	return in;
}

unsigned int __global_record_id = 1;
strMyRecord::strMyRecord():m_id(__global_record_id++) { }

strMyRecord::strMyRecord(const strMyRecord& record) : m_id(__global_record_id++) {
	for (int i = 0; i < GetFieldNum(); ++i) {
		SetFieldValue(i, record.GetFieldValue(i));
		SetLabel(record.GetLabel());
	}
}

strMyRecord::strMyRecord(strMyRecord&& record){
	for (int i = 0; i < GetFieldNum(); ++i) {
		SetFieldValue(i, record.GetFieldValue(i));
		SetLabel(record.GetLabel());
	}
	m_id = record.m_id;
}


int strMyRecord::GetFieldNum() const {
	return 18;
}

std::string strMyRecord::GetFieldName(int index) const {
	switch (index)
	{
	case 0: return "ProtocolType";
	case 1: return "Service";
	case 2: return "StatusFlag";
	case 3: return "SrcBytes";
	case 4: return "DestBytes";
	case 5: return "FailedLogins";
	case 6: return "NumofRoot";
	case 7: return "Count";
	case 8: return "SvrCount";
	case 9: return "RerrorRate";
	case 10: return "SameSrvRate";
	case 11: return "DiffSrvRate";
	case 12: return "DstHostSrvCount";
	case 13: return "DstHostSameSrvRate";
	case 14: return "DstHostDiffSrvRate";
	case 15: return "DstHostSameSrcPortRate";
	case 16: return "DstHostSrvDiffHostRate";
	case 17: return "DstHostSrvSerrorRate";
	default:
		throw(string("not this field"));
		break;
	}
}

int strMyRecord::GetFieldValue(int index) const {
	switch (index)
	{
	case 0: return iProtocolType;
	case 1: return iService;
	case 2: return iStatusFlag;
	case 3: return iSrcBytes;
	case 4: return iDestBytes;
	case 5: return iFailedLogins;
	case 6: return iNumofRoot;
	case 7: return iCount;
	case 8: return iSvrCount;
	case 9: return iRerrorRate;
	case 10: return iSameSrvRate;
	case 11: return iDiffSrvRate;
	case 12: return iDstHostSrvCount;
	case 13: return iDstHostSameSrvRate;
	case 14: return iDstHostDiffSrvRate;
	case 15: return iDstHostSameSrcPortRate;
	case 16: return iDstHostSrvDiffHostRate;
	case 17: return iDstHostSrvSerrorRate;
	default:
		throw(string("not this field"));
		break;
	}
}

int strMyRecord::GetLabel() const {
	return iLabel;
}

void strMyRecord::SetLabel(int value) {
	iLabel = value;
}

void strMyRecord::SetFieldValue(int index, int value) {
	switch (index)
	{
	case 0: iProtocolType = value; break;
	case 1: iService = value; break;
	case 2: iStatusFlag = value; break;
	case 3: iSrcBytes = value; break;
	case 4: iDestBytes = value; break;
	case 5: iFailedLogins = value; break;
	case 6: iNumofRoot = value; break;
	case 7: iCount = value; break;
	case 8: iSvrCount = value; break;
	case 9: iRerrorRate = value; break;
	case 10: iSameSrvRate = value; break;
	case 11: iDiffSrvRate = value; break;
	case 12: iDstHostSrvCount = value; break;
	case 13: iDstHostSameSrvRate = value; break;
	case 14: iDstHostDiffSrvRate = value; break;
	case 15: iDstHostSameSrcPortRate = value; break;
	case 16: iDstHostSrvDiffHostRate = value; break;
	case 17: iDstHostSrvSerrorRate = value; break;
	default:
		throw(string("not this field"));
		break;
	}
}
