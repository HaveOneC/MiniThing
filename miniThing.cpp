
#include "Volume.h"
#include <tchar.h>
class MiniThing {
public:
	MiniThing() {};	
	~MiniThing() {};
public:
	UINT realThread(LPVOID pParam);
	int num;
	void Find(string time, string filepath, CString);
};

UINT MiniThing::realThread(LPVOID pParam) {

	char *pvol = initdata.getVol();
	
	for (int i = 0; i<num; ++i) {
		initdata.initvolumelist((LPVOID)pvol[i]);
		string pro = "";
		pro = "正在统计";
		pro += pvol[i];
		pro += "盘文件...";
		//m_pro.SetPos(i+1);
		//printf("%s %d\%\n",pro.c_str());
		for (int nn = 0; nn <= (i + 1) * 100; nn += 5) {
			printf("%s%d%%\n", pro.c_str(),nn);
		}
	}
	printf("统计完毕^_^\n");
	return 0;
}

void MiniThing::Find(string time, string filepath, CString extensions)
{
	//CString lpszStringBuf(_T("dll"));
	CString lpszStringBuf(extensions);
	// 得到输入的文件名称
	lpszStringBuf.TrimLeft();	// 以防全部是空格
	lpszStringBuf.TrimRight();
	if (lpszStringBuf.GetLength() == 0) {
		return;
	}
	else if (lpszStringBuf.GetLength() < 2) {
		printf("can't less than 2 chars!\n");
		return;
	}
	// 遍历c d e盘
	cmpStrStr cmpstrstr(true,false);
	vector<string>* pignorepath = initdata.getIgnorePath();
	//ofstream fout("file.txt");
	ofstream fout(filepath);
	for (list<Volume>::iterator lvolit = initdata.volumelist.begin();
	lvolit != initdata.volumelist.end(); ++lvolit) {
		// c d e volumelist
		lvolit->findFile(lpszStringBuf, time, cmpstrstr, pignorepath);
		// 在ListBox中显示
		for (vector<FileInfo>::iterator vstrit = lvolit->rightFile.begin();
		vstrit != lvolit->rightFile.end(); ++vstrit) {
			fout <<vstrit->creationTime <<":"<< CW2A(vstrit->fileName) << "\n";
		}
		lvolit->rightFile.clear();	
	}
	fout.close();
}

int main(int argc, char** argv)
{
	if (argc == 1) {
		string help= "For example:miniThing 20180606010101 \"C:\\windows\\file.txt\" \"dll txt ini\"\n\n";
		printf("%s",help);
	}else if(argc==4){
		MiniThing q;
		initdata.init(NULL);
		q.num = initdata.getJ();
		q.realThread(NULL);
		q.Find(argv[1],argv[2],argv[3]);	
	}
	getchar();
	return 0;
}






