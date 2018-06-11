
#pragma once
#include <afxwin.h>
#include <iostream>
#include <vector>
#include <map>
#include <Winioctl.h>
#include <list>
#include <fstream>
#include <string>
#include <tchar.h>
using namespace std;

typedef struct _name_cur {
	CString filename;
	DWORDLONG pfrn;
}Name_Cur;

typedef struct _pfrn_name {
	DWORDLONG pfrn;
	CString filename;
}Pfrn_Name;

typedef struct FileInfo {
	CString fileName;
	CString path;
	string creationTime;
}FileInfo;

typedef map<DWORDLONG, Pfrn_Name> Frn_Pfrn_Name_Map;

class cmpStrStr {
public:
	cmpStrStr(bool uplow, bool inorder) {
		this->uplow = uplow;
		this->isOrder = inorder;
	}
	~cmpStrStr() {};
	bool cmpStrFilename(CString str, CString filename);
	bool infilename(CString &strtmp, CString &filenametmp);
	bool cmpStrFiletime(string time, CString path);
	string getCreationTime() {return creationTime;}
private:
	bool uplow;
	bool isOrder;
	string creationTime;
};


class Volume {
public:
	Volume(char vol) {
		this->vol = vol;
		hVol = NULL;
		path = L"";
	}
	~Volume() {}

	bool getHandle();
	bool createUSN();
	bool getUSNInfo();
	bool getUSNJournal();
	bool deleteUSN();

	bool initVolume() {
		if (
			getHandle() &&
			createUSN() &&
			getUSNInfo() &&
			getUSNJournal() &&
			deleteUSN()) {
			return true;
		}
		else {
			return false;
		}
	}

	bool isIgnore(vector<string>* pignorelist) {
		string tmp = CW2A(path);
		for (vector<string>::iterator it = pignorelist->begin();
		it != pignorelist->end(); ++it) {
			size_t i = it->length();
			if (!tmp.compare(0, i, *it, 0, i)) {
				return false;
			}
		}
		return true;
	}
	vector<FileInfo> findFile(CString str, string time, cmpStrStr& cmpstrstr, vector<string>* pignorelist);
	CString getPath(DWORDLONG frn, CString &path);
public:	
	vector<FileInfo> rightFile;
private:
	char vol;
	HANDLE hVol;
	vector<Name_Cur> VecNameCur;		
	Name_Cur nameCur;
	Pfrn_Name pfrnName;
	Frn_Pfrn_Name_Map frnPfrnNameMap;	
	CString path;
	FileInfo fileinfo;
	string creationTime;
	USN_JOURNAL_DATA ujd;
	CREATE_USN_JOURNAL_DATA cujd;
};

CString Volume::getPath(DWORDLONG frn, CString &path) {
	Frn_Pfrn_Name_Map::iterator it = frnPfrnNameMap.find(frn);
	if (it != frnPfrnNameMap.end()) {
		if (0 != it->second.pfrn) {
			getPath(it->second.pfrn, path);
		}
		path += it->second.filename;
		path += (_T("\\"));
	}

	return path;
}

vector<FileInfo> Volume::findFile(CString str, string time, cmpStrStr& cmpstrstr, vector<string>* pignorelist) {
	for (vector<Name_Cur>::const_iterator cit = VecNameCur.begin();
	cit != VecNameCur.end(); ++cit) {
		if (cmpstrstr.cmpStrFilename(str, cit->filename)) {		
			fileinfo.path = L"";
			fileinfo.fileName = L"";
			path.Empty();
			getPath(cit->pfrn, path);
			fileinfo.path=path;
			fileinfo.fileName = cit->filename;
			path += cit->filename;
			if (isIgnore(pignorelist)) {
				continue;
			}
			if (cmpstrstr.cmpStrFiletime(time, path)) {
				fileinfo.creationTime = cmpstrstr.getCreationTime();
				rightFile.push_back(fileinfo);
			}
		}
	}
	return rightFile;
}

bool cmpStrStr::cmpStrFilename(CString str, CString filename) {
	int pos = 0;
	int end = str.GetLength();
	while (pos < end) {
		pos = str.Find(_T(' '));
		CString strtmp;
		if (pos == -1) {
			strtmp = str;
			pos = end;
		}
		else {
			strtmp = str.Mid(0, pos - 0);
		}
		int filePos = filename.ReverseFind(_T('.'));
		if (filePos < 1) {
			return false;
		}
		else {
			int l = filename.GetLength();
			CString strtmp1 = filename.Mid(filePos + 1, l - filePos);
			if (infilename(strtmp, strtmp1)) {
				return true;
			}
			str.Delete(0, pos);
			str.TrimLeft(' ');
		}
	}
	return false;
}

bool cmpStrStr::infilename(CString &strtmp, CString &filename) {
	CString filenametmp(filename);
	int pos;
	if (!uplow) {
		filenametmp.MakeLower();
		pos = filename.Compare(strtmp);
	}
	else {
		pos = filename.Compare(strtmp);
	}
	if (pos != 0) {
		return false;
	}
	if (!isOrder) {
		filename.Delete(0, pos + 1);
	}
	return true;
}

bool cmpStrStr::cmpStrFiletime(string time, CString path1) {
	WIN32_FILE_ATTRIBUTE_DATA    attr = {0};     
	GetFileAttributesEx(path1,
		GetFileExInfoStandard,
		&attr);       
	SYSTEMTIME createTime = {0};
	GetSystemTime(&createTime);
	FILETIME createFiletime = attr.ftCreationTime;
	FILETIME localTme = { 0 };
	FileTimeToLocalFileTime(&createFiletime, &localTme);
	FileTimeToSystemTime(&localTme, &createTime);
	CHAR buf[15] = { 0 };
	sprintf(buf, "%04d%02d%02d%02d%02d%02d", createTime.wYear,
		createTime.wMonth,
		createTime.wDay,
		createTime.wHour,
		createTime.wMinute,
		createTime.wSecond);
	string fileTime(buf);
	if (fileTime.compare(time) > 0) {
		creationTime = fileTime;
		return true;
	}
	else {
		return false;
	}
}

bool Volume::getHandle() {
	CString lpFileName(_T("\\\\.\\c:"));
	lpFileName.SetAt(4, vol);
	hVol = CreateFile(lpFileName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_READONLY,
		NULL);
	if (INVALID_HANDLE_VALUE != hVol) {
		return true;
	}
	else {
		return false;
		exit(1);
	}
}

bool Volume::createUSN() {
	cujd.MaximumSize = 0;   
	cujd.AllocationDelta = 0; 

	DWORD br;
	if (
		DeviceIoControl(hVol,
			FSCTL_CREATE_USN_JOURNAL,      
			&cujd,           
			sizeof(cujd),        
			NULL,                         
			0,                            
			&br,    
			NULL) 
		) {
		return true;
	}
	else {
		return false;
	}
}


bool Volume::getUSNInfo() {
	DWORD br;
	if (
		DeviceIoControl(hVol,
			FSCTL_QUERY_USN_JOURNAL,
			NULL,          
			0,              
			&ujd,    
			sizeof(ujd), 
			&br, 
			NULL)
		) {
		return true;
	}
	else {
		return false;
	}
}

bool Volume::getUSNJournal() {
	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = ujd.FirstUsn;
	med.HighUsn = ujd.NextUsn;

	CString tmp(_T("C:"));
	tmp.SetAt(0, vol);
	frnPfrnNameMap[0x5000000000005].filename = tmp;
	frnPfrnNameMap[0x5000000000005].pfrn = 0;

#define BUF_LEN 0x10000	
	CHAR Buffer[BUF_LEN];
	DWORD usnDataSize;
	PUSN_RECORD UsnRecord;
	int USN_counter = 0;
	while (0 != DeviceIoControl(hVol,
		FSCTL_ENUM_USN_DATA,
		&med,
		sizeof(med),
		Buffer,
		BUF_LEN,
		&usnDataSize,
		NULL))
	{
		DWORD dwRetBytes = usnDataSize - sizeof(USN);
		UsnRecord = (PUSN_RECORD)(((PCHAR)Buffer) + sizeof(USN));

		while (dwRetBytes>0) {
			CString CfileName(UsnRecord->FileName, UsnRecord->FileNameLength / 2);
			pfrnName.filename = nameCur.filename = CfileName;
			pfrnName.pfrn = nameCur.pfrn = UsnRecord->ParentFileReferenceNumber;
			VecNameCur.push_back(nameCur);
			frnPfrnNameMap[UsnRecord->FileReferenceNumber] = pfrnName;
			DWORD recordLen = UsnRecord->RecordLength;
			dwRetBytes -= recordLen;
			UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) + recordLen);
		}
		med.StartFileReferenceNumber = *(USN *)&Buffer;
	}
	return true;
}

bool Volume::deleteUSN() {
	DELETE_USN_JOURNAL_DATA dujd;
	dujd.UsnJournalID = ujd.UsnJournalID;
	dujd.DeleteFlags = USN_DELETE_FLAG_DELETE;
	DWORD br;
	if (DeviceIoControl(hVol,
		FSCTL_DELETE_USN_JOURNAL,
		&dujd,
		sizeof(dujd),
		NULL,
		0,
		&br,
		NULL)
		) {
		CloseHandle(hVol);
		return true;
	}
	else {
		CloseHandle(hVol);
		return false;
	}
}

class InitData {
public:

	bool isNTFS(char c);

	list<Volume> volumelist;
	UINT initvolumelist(LPVOID vol) {
		char c = (char)vol;
		Volume volume(c);
		volume.initVolume();
		volumelist.push_back(volume);

		return 1;
	}
	UINT init(LPVOID lp) {

#ifdef TEST
		for (i = j = 0; i<MAXVOL; ++i) {
#else
		for (i = j = 0; i<26; ++i) {
#endif
			cvol = i + 'A';
			if (isNTFS(cvol)) {
				vol[j++] = cvol;
			}
		}
		return true;
		}

	int getJ() {
		return j;
	}
	char * getVol() {
		return vol;
	}

	vector<string>* getIgnorePath() {
		ignorepath.clear();
		ifstream fin("config.ini");
		string tmp;
		while (getline(fin, tmp)) {
			ignorepath.push_back(tmp);
		}
		return &ignorepath;
	}

private:
	char vol[26];
	char cvol;
	int i, j;
	vector<string> ignorepath;
};

bool InitData::isNTFS(char c) {
	char lpRootPathName[] = ("c:\\");
	lpRootPathName[0] = c;
	char lpVolumeNameBuffer[MAX_PATH];
	DWORD lpVolumeSerialNumber;
	DWORD lpMaximumComponentLength;
	DWORD lpFileSystemFlags;
	char lpFileSystemNameBuffer[MAX_PATH];

	if (GetVolumeInformationA(
		lpRootPathName,
		lpVolumeNameBuffer,
		MAX_PATH,
		&lpVolumeSerialNumber,
		&lpMaximumComponentLength,
		&lpFileSystemFlags,
		lpFileSystemNameBuffer,
		MAX_PATH
		)) {
		if (!strcmp(lpFileSystemNameBuffer, "NTFS")) {
			return true;
		}
	}
	return false;
}

InitData initdata;