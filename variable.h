/***In version check***/
const string VERSION="1.0.1";
const unsigned int VERSION_ID=10001;	//01 00 01

const LPCWSTR update_path=L"https://raw.githubusercontent.com/xuxiaocheng0201/XuxiaochengsDataBase/main/AutoCopy/version_check.cfg";
const LPCWSTR unzip_path=L"https://raw.githubusercontent.com/xuxiaocheng0201/AutoCopy/main/unzip.exe";

unsigned int newest_version_id=VERSION_ID;
string newest_version_url="unknow";
unsigned int recommended_version_id=VERSION_ID;
string recommended_version_url="unknow";

bool load_urlmon_dll_succeed=0;
HINSTANCE urlmon_dll;
typedef int(*DownloadFunction)(LPUNKNOWN,LPCWSTR,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);
DownloadFunction DownloadFromURLToFile;

Configuration update_config;


/***In Configuration***/

Configuration config;
/*Time: MAX_INT>=24.8 Day*/
unsigned long long int TIME_OF_DETECT_INTERVAL=10000;
unsigned long long int TIME_OF_CREATE_PTHREAD=10;
unsigned long long int TIME_OF_WAIT_CPU=1000;
/*Useage: max=100(%) min=0(%)*/
unsigned int MAX_CPU_USEAGE=80;
unsigned int MIN_CPU_USEAGE=80;
/*Pthread num: MAX_PTHREAD>=NOW_PTHREAD>=MIN_PTHREAD>=0*/
unsigned int NOW_PTHREAD=10;
unsigned int MAX_PTHREAD=10;
unsigned int MIN_PTHREAD=0;

bool USE_USB_SERIAL_INSTEAT_OF_TIMES=0;
string LOG_LEVEL="ALL";

bool version_use_newest=0;

int GET_hwndDOS_LEVEL=2;
/*
	* GET_hwndDOS_LEVEL==0: use hwndDOS!=NULL
	* GET_hwndDOS_LEVEL==1: use hwndDOS!=NULL and GetLastError==0
	* GET_hwndDOS_LEVEL==2: use hwndDOS!=NULL and ask for the user
	* GET_hwndDOS_LEVEL==3: use hwndDOS!=NULL, GetLastError==0 and then ask for the user
*/
int COPY_MODE=7;
/*
	* abs(COPY_MODE)==1: Use pthread with xcopy
	* abs(COPY_MODE)==2: Use pthread with xcopy after xcopy
	* abs(COPY_MODE)==3: Use pthread with xcopy before xcopy
	* abs(COPY_MODE)==4: Use pthread with CopyFile() (include <windows.h>)
	* abs(COPY_MODE)==5: Use pthread with CopyFile() (include <windows.h>) after xcopy
	* abs(COPY_MODE)==6: Use pthread with CopyFile() (include <windows.h>) before xcopy
	* abs(COPY_MODE)==7: Use pthread with copy_file() (include "head_libs/copy_file.h")
	* abs(COPY_MODE)==8: Use pthread with copy_file() (include "head_libs/copy_file.h") after xcopy
	* abs(COPY_MODE)==9: Use pthread with copy_file() (include "head_libs/copy_file.h") before xcopy
	* abs(COPY_MODE)==10: Only use xcopy
	* COPY_MODE>=0: get file list (COPY_MODE==0: only get file list)
*/
bool SORT_LIST=0;

string input="A:",output="C:\\UBackUp\\temp";
bool disappear=0,now_dis=disappear;


/***In process***/

bool input_is_u=1;
bool folder_isnot_exist=1,EXITS=0;
HWND hwndDOS=NULL;
unsigned int numbers=1,times=numbers;
vector<string>files_all;
	//				path             size   time     md5
vector<vector<pair<string,pair<pair<size_t,string>,string> > > >all;
	//all[0]=temp  all[n]=folder_n
vector<string>need;
map<string,bool>need_f;
get_cpu_useage getCpuUseage;
Configuration run_config;

string GetLastError_string_turn;
stringstream GetLastError_stream;
string hwndDOS_string;
stringstream hwndDOS_stream;

disk_information disk;
	//target  times
map<string,unsigned int>mapping;


/***In pthread and thread***/

HANDLE thread_stone_handle=NULL;
DWORD thread_stone_tid=0;
//void *stone_all(void *rags);
string thread_stone_input=input;
bool thread_stone_exit=1;

HANDLE thread_cpu_handle=NULL;
DWORD thread_cpu_tid=0;
//void *change_MAX_PTHREAD_based_on_CpuUseage(void *rags);
double cpu=0;
bool thread_cpu_should_finished=1;
bool thread_cpu_exit=1;

HANDLE thread_detected_run_configuration_handle=NULL;
DWORD thread_detected_run_configuration_tid=0;
//void *detected_run_configuration(void *rags);
bool thread_detected_run_configuration_should_finished=1;
bool thread_detected_run_configuration_exit=1;

double_link<pthread_t> pthread_compare_files_tids;
//void *compare_files(void *rags);
unsigned int pthread_compare_files_i=0;
string pthread_compare_files_input=input;
string pthread_compare_files_output;//=output+turn_string(times);
bool pthread_compare_files_release=1;

double_link<pthread_t> pthread_copy_file_tids;
//void *copy_file(void *rags);
unsigned int pthread_copy_file_i=0;
string pthread_copy_file_input=input;
string pthread_copy_file_output;//=output+turn_string(times);
bool pthread_copy_file_release=1;

