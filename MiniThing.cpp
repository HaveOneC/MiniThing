
#include "Volume.h"

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
		pro = "Being statistics the files of";
		pro += pvol[i];
		pro += " disk......";
		for (int nn = 0; nn <= (i + 1) * 100; nn += 5) {
			printf("%s%d%%\n", pro.c_str(),nn);
		}
	}
	printf("Statistical completion ^_^\n");
	return 0;
}

void MiniThing::Find(string time, string filepath, CString extensions)
{
	//CString lpszStringBuf(_T("dll"));
	CString lpszStringBuf(extensions);
	lpszStringBuf.TrimLeft();	
	lpszStringBuf.TrimRight();
	if (lpszStringBuf.GetLength() == 0) {
		return;
	}
	else if (lpszStringBuf.GetLength() < 2) {
		printf("can't less than 2 chars!\n");
		return;
	}
	cmpStrStr cmpstrstr(true,false);
	vector<string>* pignorepath = initdata.getIgnorePath();
	//ofstream fout("file.txt");
	ofstream fout(filepath);
	for (list<Volume>::iterator lvolit = initdata.volumelist.begin();
	lvolit != initdata.volumelist.end(); ++lvolit) {
		lvolit->findFile(lpszStringBuf, time, cmpstrstr, pignorepath);
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
		string help= "For example:\n\tminiThing.exe 20180606010101 \"C:\\windows\\file.txt\" \"dll txt ini\"\n\n";
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






