// QSearchDlg.cpp : 实现文件
//


#include "Volume.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQSearchDlg 对话框
class CQSearchDlg {
public:
	CQSearchDlg() {};	// 标准构造函数
	~CQSearchDlg() {};
	// 菜单设置
public:

	UINT realThread(LPVOID pParam);
	int num;
	void Find();
};

UINT CQSearchDlg::realThread(LPVOID pParam) {

	char *pvol = initdata.getVol();
	
	for (int i = 0; i<num; ++i) {
		initdata.initvolumelist((LPVOID)pvol[i]);
		string pro = "";
		pro = "正在统计";
		pro += pvol[i];
		pro += "盘文件...";
		//m_pro.SetPos(i+1);
		printf("%s%d%%d\n",pro,i,num);
	}
	printf("over\n");
	return 0;
}

void CQSearchDlg::Find()
{
	
	// TODO: 在此添加控件通知处理程序代码
	//	WaitForSingleObject(pThread, INFINITE);
	//	delete pThread;


	CString lpszStringBuf(_T("dll"));
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

	// CString -> string
	// string str(CW2A( (LPCTSTR)lpszStringBuf) );


	// TODO: 在此添加 
	// 遍历c d e盘
	cmpStrStr cmpstrstr(true,false);
	vector<string>* pignorepath = initdata.getIgnorePath();
	string time = "20180610010101";
	ofstream fout("file.txt");
	for (list<Volume>::iterator lvolit = initdata.volumelist.begin();
	lvolit != initdata.volumelist.end(); ++lvolit) {
		// c d e volumelist
		lvolit->findFile(lpszStringBuf, time, cmpstrstr, pignorepath);
		

		// 在ListBox中显示
		for (vector<CString>::iterator vstrit = lvolit->rightFile.begin();
		vstrit != lvolit->rightFile.end(); ++vstrit) {
			fout << CW2A(*vstrit) << "\n";
		}
		lvolit->rightFile.clear();	
	}
	fout.close();
}

int main()
{
	CQSearchDlg q;
	initdata.init(NULL);
	q.num = initdata.getJ();
	q.realThread(NULL);
	q.Find();
	getchar();
	return 0;
}






