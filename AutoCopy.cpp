#include <bits/stdc++.h>
using namespace std;
#include <windows.h>
#include <dbt.h>

#define CHECK_FILE_CHOOSE_MD5
#define CHECK_FILE_CHOOSE_SHA1
#define CHECK_FILE_CHOOSE_CRC32
ostream& link_data_output(ostream& os,pthread_t data){
	return os<<"tid: "<<data<<endl;
}

#include "head_libs/logger.h"
#include "head_libs/process_exists.h"
#include "head_libs/pthread_lives.h"
#include "head_libs/configuration.h"
#include "head_libs/get_files.h"
#include "head_libs/double_link.h"
#include "head_libs/get_file_size.h"
#include "head_libs/get_file_time.h"
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

int main()
{
	ENVIRONMENT_PREPARE();
	
	CHECK_OTHER_RUNNING();
	if(EXITS)
		return 0;
	
	search_plug("add\\before_READ_CONFIG.exe");
	READ_CONFIG();
	if(EXITS)
		return 0;
	
	search_plug("add\\before_UPDATE_CHECK.exe");
	UPDATE_CHECK();
	if(EXITS)
		return 0;
	
	search_plug("add\\before_GET_HWNDDOS.exe");
	GET_HWNDDOS();
	if(EXITS)
		return 0;
	
	search_plug("add\\before_GET_NUMBERS.exe");
	GET_NUMBERS();
	
	search_plug("add\\before_READ_SAVED_FILES_LISTS.exe");
	READ_SAVED_FILES_LISTS();
	
	if(input_is_u)
	{
		search_plug("add\\before_READ_MAPPINGS.exe");
		READ_MAPPINGS();
	}
	
	search_plug("add\\before_CALL_change_MAX_PTHREAD_based_on_CpuUseage.exe");
	print("INFO","Call change_MAX_PTHREAD_based_on_CpuUseage.");
	thread_cpu_exit=0;
	thread_cpu_should_finished=0;
	thread_cpu_handle=CreateThread((LPSECURITY_ATTRIBUTES)NULL,0,(LPTHREAD_START_ROUTINE)change_MAX_PTHREAD_based_on_CpuUseage,(LPVOID)NULL,0,&thread_cpu_tid);
	if(thread_cpu_handle==NULL)
	{
		print("WARN","Call change_MAX_PTHREAD_based_on_CpuUseage error. [tid]="+turn_string((unsigned int)thread_cpu_tid));
		print("INFO","Use changeless NOW_PTHREAD.");
	}
	else
		SetThreadPriority(thread_cpu_handle,THREAD_PRIORITY_ABOVE_NORMAL);
	
	search_plug("add\\before_CALL_detected_run_configuration.exe");
	print("INFO","Call detected_run_configuration.");
	thread_detected_run_configuration_exit=0;
	thread_detected_run_configuration_should_finished=0;
	thread_detected_run_configuration_handle=CreateThread((LPSECURITY_ATTRIBUTES)NULL,0,(LPTHREAD_START_ROUTINE)detected_run_configuration,(LPVOID)NULL,0,&thread_detected_run_configuration_tid);
	if(thread_detected_run_configuration_handle==NULL)
	{
		print("WARN","Call detected_run_configuration error. [tid]="+turn_string((unsigned int)thread_detected_run_configuration_tid));
		print("INFO","Use normal detected_run_configuration().");
	}
	
	while(1)
	{
		folder_isnot_exist=1;
		search_plug("add\\before_WAIR_EXISTS.exe");
		print("INFO","Start to wait for folder exists(Insert USB flash drive).");
		while(1)
		{
			print_loop("INFO","Have a check.");
			if(thread_detected_run_configuration_handle==NULL)
				detected_run_configuration(NULL);
			if(EXITS)
				break;
			files_all.clear();
			get_files(input,files_all,"temp.txt");
			if(files_all.size())
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
		
		search_plug("add\\before_XCOPY_once.exe");
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
		
		search_plug("add\\before_XCOPY_twice.exe");
		if(!folder_isnot_exist && (abs(COPY_MODE)==3 || abs(COPY_MODE)==6 || abs(COPY_MODE)==9))
			cmd("xcopy \""+input+"\" \""+output+"\\"+turn_string(times)+"\" /e /v /c /i /f /l /g /h /k /o /x /y");
		
		if(COPY_MODE>=0)
		{
			search_plug("add\\before_CLOSE_stone_all.exe");
			if(thread_stone_handle!=NULL)
			{
				print("INFO","Waiting for stone_all exits.");
				while(!thread_stone_exit)
					Sleep(1);
				CloseHandle(thread_stone_handle);
			}
			print("INFO","Stone_all exits.");
		}
		
		search_plug("add\\before_WAIR_NOT_EXISTS.exe");
		print("INFO","Start to wait for folder does not exist(Pull out USB flash drive).");
		while(1)
		{
			print_loop("INFO","Have a check.");
			if(thread_detected_run_configuration_handle==NULL)
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
				files_all.clear();
				get_files(input,files_all,"temp.txt");
				if(!files_all.size())
					break;
				print("FAULT","This_folder.txt is deleted. [times]="+turn_string(times));
				break;
			}
		}
		if(EXITS)
			break;
		print("INFO","Detected that the folder does not exist.");
	}
	
	thread_cpu_should_finished=1;
	thread_detected_run_configuration_should_finished=1;
	
	if(thread_stone_handle!=NULL)
	{
		print("INFO","Waiting for stone_all exits.");
		while(!thread_stone_exit)
			Sleep(1);
		CloseHandle(thread_stone_handle);
	}
	
	search_plug("add\\before_CLOSE_change_MAX_PTHREAD_based_on_CpuUseage.exe");
	if(thread_cpu_handle!=NULL)
	{
		print("INFO","Waiting for change_MAX_PTHREAD_based_on_CpuUseage exits.");
		while(!thread_cpu_exit)
			Sleep(1);
		CloseHandle(thread_cpu_handle);
	}
	
	search_plug("add\\before_CLOSE_detected_run_configuration.exe");
	if(thread_detected_run_configuration_handle!=NULL)
	{
		print("INFO","Waiting for detected_run_configuration exits.");
		while(!thread_detected_run_configuration_exit)
			Sleep(1);
		CloseHandle(thread_detected_run_configuration_handle);
	}
	
	if(load_urlmon_dll_succeed)
		FreeLibrary(urlmon_dll);
	
	search_plug("add\\before_EXIT.exe");
	cmd("clear.bat");
	return 0;
}

