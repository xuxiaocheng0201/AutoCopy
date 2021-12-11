/***define***/
inline void Preparing();
inline void Exiting();

/***fuction***/
inline void Preparing()
{
	static bool run_once=0;
	if(run_once)
		return;
	run_once=1;
	
	if(!logger_cache_arrange())
		logger("FAULT","Arrange logs cache error.");
	
	std::ios::sync_with_stdio(false);
	if((file_exists("run_config.txt") || file_exists("temp.txt") || file_exists("disk_information.temp")) && process_exists(TEXT("AutoCopy"))!=0)
	{
		cout<<"There is some other run_config.txt or temp.txt or disk_information.temp in the run path."<<endl;
		cout<<"Please make sure no AutoCopy.exe else on this path is running and then run clear.bat."<<endl;
		cout<<"Enter any key to exit."<<endl;
		MessageBox(NULL,TEXT("Please make sure no AutoCopy.exe else on this path is running and then run clear.bat."),TEXT("WARNING"),MB_OK|MB_ICONWARNING);
		system("pause");
		EXITS=1;
		return;
	}
	system("clear.bat");
	system("md log");
	system("md lists");
	system("cls");
	SYSTEMTIME LogCreateTime;
	GetLocalTime(&LogCreateTime);
	log_path="log\\log(Year"+turn_string(LogCreateTime.wYear)+"Month"+turn_string(LogCreateTime.wMonth)+"Week"+turn_string(LogCreateTime.wDayOfWeek)+"Day"+turn_string(LogCreateTime.wDay)+"Hour"+turn_string(LogCreateTime.wHour)+"Minute"+turn_string(LogCreateTime.wMinute)+"Second"+turn_string(LogCreateTime.wSecond)+"Millisecond"+turn_string(LogCreateTime.wMilliseconds)+").txt";
	config_path="config.txt";
	disk_information_temp_delete=0;
	SetLastError(0);
	Sleep(500);
	
	search_plug("add\\before_READ_CONFIG.exe");
	READ_CONFIG();
	if(EXITS)
		return;
	
	search_plug("add\\before_GET_HWNDDOS.exe");
	GET_HWNDDOS();
	if(EXITS)
		return;
	
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
	return;
}

inline void Exiting()
{
	static bool run_once=0;
	if(run_once)
		return;
	run_once=1;
	
	if(thread_stone_handle!=NULL)
	{
		print("INFO","Waiting for stone_all exits.");
		while(!thread_stone_exit)
			Sleep(1);
		CloseHandle(thread_stone_handle);
	}
	
	search_plug("add\\before_CLOSE_change_MAX_PTHREAD_based_on_CpuUseage.exe");
	thread_cpu_should_finished=1;
	if(thread_cpu_handle!=NULL)
	{
		print("INFO","Waiting for change_MAX_PTHREAD_based_on_CpuUseage exits.");
		while(!thread_cpu_exit)
			Sleep(1);
		CloseHandle(thread_cpu_handle);
	}
	
	search_plug("add\\before_CLOSE_detected_run_configuration.exe");
	thread_detected_run_configuration_should_finished=1;
	if(thread_detected_run_configuration_handle!=NULL)
	{
		print("INFO","Waiting for detected_run_configuration exits.");
		while(!thread_detected_run_configuration_exit)
			Sleep(1);
		CloseHandle(thread_detected_run_configuration_handle);
	}
	
	search_plug("add\\before_Exit.exe");
	cmd("clear.bat");
	
	if(!logger_cache_arrange())
		logger("FAULT","Arrange logs cache error.");
	return;
}

