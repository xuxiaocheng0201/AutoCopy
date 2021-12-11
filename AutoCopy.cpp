#include <bits/stdc++.h>
using namespace std;
#include <windows.h>
#include <dbt.h>

#define CHECK_FILE_CHOOSE_MD5
#define CHECK_FILE_CHOOSE_SHA1
ostream& link_data_output(ostream& os,pthread_t data){
	return os<<"tid: "<<data<<endl;
}

#include "head_libs/logger_cache.h"
#include "head_libs/process_exists.h"
#include "head_libs/pthread_lives.h"
#include "head_libs/configuration.h"
#include "head_libs/get_files.h"
#include "head_libs/double_link.h"
#include "head_libs/get_file_size.h"
#include "head_libs/get_cpu_useage.h"
#include "head_libs/string_operate/check_path.h"
#include "head_libs/check_file.h"
#include "head_libs/copy_file.h"
#include "head_libs/GetLastError_string.h"
#include "head_libs/get_drivers.h"
#include "head_libs/disk_information.h"

#include "variable.h"
#include "basic_fuctions.h"
#include "pthread_fuctions.h"
#include "part_fuctions.h"
#include "fuctions.h"

int main()
{
	Preparing();
	if(EXITS)
		return 0;
	while(1)
	{
		folder_isnot_exist=1;
		search_plug("add\\before_WAIR_EXISTS.exe");
		print("INFO","Start to wait for folder exists(Insert USB flash drive).");
		while(1)
		{
			print_loop("INFO","Have a check.");
			if(thread_cpu_handle==NULL)
				detected_run_configuration(NULL);
			if(EXITS)
				break;
			getFiles_all.clear();
			get_files(input,getFiles_all,"temp.txt");
			if(getFiles_all.size())
			{
				folder_isnot_exist=0;
				break;
			}
			print_loop("INFO","Detected that the folder does not exist.");
			Sleep(TIME_OF_DETECT_INTERVAL);
		}
		if(EXITS)
			break;
		print("INFO","Detected that the folder exists.");
		search_plug("add\\before_GET_TIMES.exe");
		GET_TIMES();
		
		if(COPY_MODE>=0)
		{
			search_plug("add\\before_OPEN_STONE_ALL.exe");
			print("INFO","Call stone_all.");
			thread_stone_exit=0;
			thread_stone_input=input;
			thread_stone_handle=CreateThread((LPSECURITY_ATTRIBUTES)NULL,0,(LPTHREAD_START_ROUTINE)stone_all,(LPVOID)NULL,0,&thread_stone_tid);
			if(thread_stone_handle==NULL)
			{
				print("WARN","Call stone_all error. [tid]="+turn_string((unsigned int)thread_stone_tid));
				print("INFO","Using main function now.");
				stone_all(NULL);
			}
			else
				SetThreadPriority(thread_stone_handle,THREAD_PRIORITY_HIGHEST);
		}
		
		search_plug("add\\before_CREATE_FOLDERS.exe");
		print("INFO","Create new folders on output.");
		cmd("xcopy \""+input+"\" \""+output+"\\"+turn_string(times)+"\" /t /e /k /o /x /h /y /c /i");
		
		search_plug("add\\before_XCOPY_one.exe");
		if(!folder_isnot_exist && (abs(COPY_MODE)==2 || abs(COPY_MODE)==5 || abs(COPY_MODE)==8 || abs(COPY_MODE)==10))
			cmd("xcopy \""+input+"\" \""+output+"\\"+turn_string(times)+"\" /e /v /c /i /f /l /g /h /k /o /x /y");
		
		search_plug("add\\before_COMPAIE_FILES_LISTS.exe");
		if(!folder_isnot_exist && (1<=abs(COPY_MODE) && abs(COPY_MODE)<=9))
			COMPAIE_FILES_LISTS();
		if(EXITS)
			break;
		search_plug("add\\before_COPY_FILES_LISTS.exe");
		if(!folder_isnot_exist && (1<=abs(COPY_MODE) && abs(COPY_MODE)<=9) && need.size())
			COPY_FILES_LISTS();
		if(EXITS)
			break;
		
		search_plug("add\\before_XCOPY_two.exe");
		if(!folder_isnot_exist && (abs(COPY_MODE)==3 || abs(COPY_MODE)==6 || abs(COPY_MODE)==9))
			cmd("xcopy \""+input+"\" \""+output+"\\"+turn_string(times)+"\" /e /v /c /i /f /l /g /h /k /o /x /y");
		
		search_plug("add\\before_CLOSE_stone_all.exe");
		if(thread_stone_handle!=NULL)
		{
			print("INFO","Waiting for stone_all exits.");
			while(!thread_stone_exit)
				Sleep(1);
			CloseHandle(thread_stone_handle);
		}
		print("INFO","Stone_all exits.");
		
		search_plug("add\\before_WAIR_NOT_EXISTS.exe");
		print("INFO","Start to wait for folder does not exist(Pull out USB flash drive).");
		while(1)
		{
			print_loop("INFO","Have a check.");
			if(thread_cpu_handle==NULL)
				detected_run_configuration(NULL);
			if(EXITS)
				break;
			if(input_is_u)
			{
				bool found=0;
				vector<driver_information> drivers=get_drivers();
				for(unsigned int i=0;i<drivers.size();i++)
					if(drivers[i].disk==input[0])
					{
						found=1;
						break;
					}
				if(found)
				{
					print_loop("INFO","Detected that the folder still exists.");
					Sleep(TIME_OF_DETECT_INTERVAL);
					continue;
				}
				break;
			}
			else
			{
				if(file_exists(input+"\\ac\\AutoCopy\\This_folder.txt"))
				{
					print_loop("INFO","Detected that the folder still exists.");
					Sleep(TIME_OF_DETECT_INTERVAL);
					continue;
				}
				getFiles_all.clear();
				get_files(input,getFiles_all,"temp.txt");
				if(!getFiles_all.size())
					break;
				print("ERROR","This_folder.txt is deleted. [times]="+turn_string(times));
				special_logger("error.txt","ERROR","[times]="+turn_string(times)+" This_folder.txt doesn't exist.");
				break;
			}
		}
		if(EXITS)
			break;
		print("INFO","Detected that the folder does not exist.");
	}
	Exiting();
	return 0;
}

