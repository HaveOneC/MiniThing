// QSearchDlg.cpp : ʵ���ļ�
//


#include "Volume.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQSearchDlg �Ի���
class CQSearchDlg {
public:
	CQSearchDlg() {};	// ��׼���캯��
	~CQSearchDlg() {};
	// �˵�����
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
		pro = "����ͳ��";
		pro += pvol[i];
		pro += "���ļ�...";
		//m_pro.SetPos(i+1);
		printf("%s%d%%d\n",pro,i,num);
	}
	printf("over\n");
	return 0;
}

void CQSearchDlg::Find()
{
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//	WaitForSingleObject(pThread, INFINITE);
	//	delete pThread;


	CString lpszStringBuf(_T("dll"));
	// �õ�������ļ�����
	lpszStringBuf.TrimLeft();	// �Է�ȫ���ǿո�
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


	// TODO: �ڴ���� 
	// ����c d e��
	cmpStrStr cmpstrstr(true,false);
	vector<string>* pignorepath = initdata.getIgnorePath();
	string time = "20180610010101";
	ofstream fout("file.txt");
	for (list<Volume>::iterator lvolit = initdata.volumelist.begin();
	lvolit != initdata.volumelist.end(); ++lvolit) {
		// c d e volumelist
		lvolit->findFile(lpszStringBuf, time, cmpstrstr, pignorepath);
		

		// ��ListBox����ʾ
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






