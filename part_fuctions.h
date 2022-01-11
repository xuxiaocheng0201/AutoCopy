/***define***/
inline void ENVIRONMENT_PREPARE();
inline void UPDATE_CHECK();
inline void CHECK_OTHER_RUNNING();
inline void GET_HWNDDOS();
inline void GET_NUMBERS();
inline void GET_TIMES();
inline void READ_CONFIG();
inline void READ_SAVED_FILES_LISTS();
inline void READ_MAPPINGS();
inline void COMPAIE_FILES_LISTS();
inline void COPY_FILES_LISTS();

/***fuction***/
inline void ENVIRONMENT_PREPARE()
{
	std::ios::sync_with_stdio(false);
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
	return;
}

inline void UPDATE_CHECK()
{
	if(!file_exists("urlmon.dll"))
	{
		print("WARN","[UPDATE_CHECK]Couldn't find urlmon.dll. Skip version check.");
		return;
	}
	if(getCRC32("urlmon.dll")!="C62DCFF1")
	{
		print("ERROR","[UPDATE_CHECK]The crc32 of urlmon.dll is wrong! It's "+getCRC32("urlmon.dll")+" but it should be C62DCFF1.");
		return;
	}
	if(getSHA1("urlmon.dll")!="314DA1DE7F818AB3ACF4B562CD54B56641D5F217")
	{
		print("ERROR","[UPDATE_CHECK]The sha1 of urlmon.dll is wrong! It's "+getSHA1("urlmon.dll")+" but it should be 314DA1DE7F818AB3ACF4B562CD54B56641D5F217.");
		return;
	}
	urlmon_dll=LoadLibrary("urlmon.dll");
	if(urlmon_dll==NULL)
	{
		FreeLibrary(urlmon_dll);
		print("ERROR","[UPDATE_CHECK]Failed to link to urlmon.dll. Skip version check.");
		return;
	}
	load_urlmon_dll_succeed=1;
	DownloadFromURLToFile=(DownloadFunction)GetProcAddress(urlmon_dll,"URLDownloadToFileW");
	if(DownloadFromURLToFile(NULL,update_path,L"version_check.cfg",0,NULL)!=S_OK)
	{
		print("ERROR","[UPDATE_CHECK]Download version_check.cfg failed. Skip version check.");
		return;
	}
	update_config.clear();
	update_config.read("version_check.cfg");
	for(unsigned int i=0;i<update_config.size();i++)
	{
		if(update_config.get(i,CONFIGURATION_NAME)==VariableName(newest_version_id) && update_config.get(i,CONFIGURATION_TYPE)=="U")
			newest_version_id=turn_unsigned_int(update_config.get(i,CONFIGURATION_VALUE));
		if(update_config.get(i,CONFIGURATION_NAME)==VariableName(recommended_version_id) && update_config.get(i,CONFIGURATION_TYPE)=="U")
			recommended_version_id=turn_unsigned_int(update_config.get(i,CONFIGURATION_VALUE));
		if(update_config.get(i,CONFIGURATION_NAME)==VariableName(newest_version_url) && update_config.get(i,CONFIGURATION_TYPE)=="S")
			newest_version_url=update_config.get(i,CONFIGURATION_VALUE);
		if(update_config.get(i,CONFIGURATION_NAME)==VariableName(recommended_version_url) && update_config.get(i,CONFIGURATION_TYPE)=="S")
			recommended_version_url=update_config.get(i,CONFIGURATION_VALUE);
	}
	print("INFO","[UPDATE_CHECK]Current version id is "+turn_string(VERSION_ID)+". Newest version id is "+turn_string(newest_version_id)+". Recommend version id is "+turn_string(recommended_version_id)+".");
	unsigned int target_id=recommended_version_id;
	string target_url_string=recommended_version_url;
	if(version_use_newest)
	{
		target_id=newest_version_id;
		target_url_string=newest_version_url;
	}
	print("DEBUG","[UPDATE_CHECK]Target version id is "+turn_string(target_id)+".");
	if(target_url_string=="unknow")
	{
		print("ERROR","[UPDATE_CHECK]target_url get failed. Skip version update.");
		return;
	}
	LPCWSTR target_url=turn_LPCWSTR(target_url_string);
	if(target_id<VERSION_ID)
	{
		print("WARN","[UPDATE_CHECK]Now version is newer than target version. Skip version update.");
		return;
	}
	if(target_id==VERSION_ID)
	{
		print("INFO","[UPDATE_CHECK]Now version is the target version. Skip version update.");
		return;
	}
	print("INFO","Download update.zip.");
	if(DownloadFromURLToFile(NULL,target_url,L"update.zip",0,NULL)!=S_OK)
	{
		print("ERROR","[UPDATE_CHECK]Download update.zip failed. Skip version update.");
		return;
	}
	if(!file_exists("unzip.exe"))
	{
		print("WARN","[UPDATE_CHECK]Couldn't find unzip.exe. Try to download it.");
		if(DownloadFromURLToFile(NULL,unzip_path,L"unzip.exe",0,NULL)!=S_OK)
		{
			print("ERROR","[UPDATE_CHECK]Download unzip.exe failed. Skip version update.");
			return;
		}
	}
	if(getCRC32("unzip.exe")!="004AE69D")
	{
		print("ERROR","[UPDATE_CHECK]The crc32 of unzip.exe is wrong! It's "+getCRC32("unzip.exe")+" but it should be 004AE69D. Try to download it.");
		if(DownloadFromURLToFile(NULL,unzip_path,L"unzip.exe",0,NULL)!=S_OK)
		{
			print("ERROR","[UPDATE_CHECK]Download unzip.exe failed. Skip version update.");
			return;
		}
	}
	if(getSHA1("unzip.exe")!="A3E7BAF0557CB42D3D7668A73FC56C1F2AA23104")
	{
		print("ERROR","[UPDATE_CHECK]The sha1 of unzip.exe is wrong! It's "+getSHA1("unzip.exe")+" but it should be A3E7BAF0557CB42D3D7668A73FC56C1F2AA23104. Try to download it.");
		if(DownloadFromURLToFile(NULL,unzip_path,L"unzip.exe",0,NULL)!=S_OK)
		{
			print("ERROR","[UPDATE_CHECK]Download unzip.exe failed. Skip version update.");
			return;
		}
	}
	print("INFO","Unzip update.zip to folder update.");
	system("unzip.exe update.zip -d update");
	print("INFO","Run update.exe. Now exit this process.");
	ShellExecute(NULL,"open","update\\update","","update",!disappear);
	FreeLibrary(urlmon_dll);
	EXITS=1;
	return;
}

inline void CHECK_OTHER_RUNNING()
{
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
}

inline void GET_HWNDDOS()
{
	print("INFO","[GET_HWNDDOS]Start to find hwndDOS.");
	SetLastError(0);
	hwndDOS=GetConsoleWindow();
	print("DEBUG","[GET_HWNDDOS]hwndDOS is "+turn_string_hwndDOS()+".");
	if(check_hwndDOS())
		return;
	print("WARN","[GET_HWNDDOS]GetConsoleWindow failed.");
	print("DEBUG","[GET_HWNDDOS][GetConsoleWindow]GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
	SetLastError(0);
	print("INFO","[GET_HWNDDOS]Try using FindWindow.");
	hwndDOS=FindWindow(NULL,TEXT("AutoCopy"));
	print("DEBUG","[GET_HWNDDOS]hwndDOS is "+turn_string_hwndDOS()+".");
	if(check_hwndDOS())
		return;
	print("WARN","[GET_HWNDDOS]FindWindow failed.");
	print("DEBUG","[GET_HWNDDOS][FindWindow]GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
	SetLastError(0);
	print("INFO","[GET_HWNDDOS]Try using GetForegroundWindow. Please enter any key on me.");
	MessageBox(NULL,TEXT("Please enter any key on AutoCopy.exe."),TEXT("ERROR"),MB_OK|MB_ICONWARNING);
	SetLastError(0);
	cmd("pause");
	hwndDOS=GetForegroundWindow();
	print("DEBUG","[GET_HWNDDOS]hwndDOS is "+turn_string_hwndDOS()+".");
	if(check_hwndDOS())
		return;
	print("FAULT","[GET_HWNDDOS]GetForegroundWindow failed.");
	print("DEBUG","[GET_HWNDDOS][GetForegroundWindow]GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
	SetLastError(0);
	hwndDOS=NULL;
	print("DEBUG","[GET_HWNDDOS]Changed hwndDOS to NULL.");
	if(disappear==1)
	{
		print("FAULT","[GET_HWNDDOS/detected_run_configuration][hwndDOS] is NULL, process exits.");
		EXITS=1;
	}
	else
		print("ERROR","[GET_HWNDDOS]Don't change [disappear] to true in this run time, or process will exit.");
	return;
}

inline void GET_NUMBERS()
{
	print("INFO","[GET_NUMBERS]Start to read [numbers].");
	if(!file_exists(output+"\\AutoCopy.txt"))
	{
		print("WARN","[GET_NUMBERS]Can't find AutoCopy.txt.");
		cmd("md \""+output+"\" >nul");
		ofstream fout((output+"\\AutoCopy.txt").c_str());
		if(fout)
		{
			fout<<numbers<<endl;
			fout.close();
			print("INFO","[GET_NUMBERS]Created a new AutoCopy.txt with [numbers]="+turn_string(numbers)+" in it.");
		}
		else
			print("ERROR","[GET_NUMBERS]Open file failed.");
	}
	else
	{
		ifstream fin((output+"\\AutoCopy.txt").c_str());
		if(fin)
		{
			fin>>numbers;
			fin.close();
		}
		else
			print("ERROR","[GET_NUMBERS]Open file failed.");
	}
	print("INFO","[GET_NUMBERS]Process knew [numbers]="+turn_string(numbers)+".");
	return;
}

inline void GET_TIMES()
{
	print("INFO","[GET_TIMES]Start to read [times].");
	if(input_is_u)
	{
		disk.set_path(input[0]);
		print("DEBUG","[GET_TIMES]The serial of the USB flash drive is "+disk.serial+".");
		if(mapping[disk.serial]==0)
		{
			print("INFO","[GET_TIMES]It is a new USB flash drive.");
			mapping[disk.serial]=numbers;
			times=numbers;
			{
				print("INFO","[GET_TIMES]Update new mapping.txt");
				FILE *fout=fopen((output+"\\mapping.txt").c_str(),"at");
				if(fout==NULL)
					print("ERROR","[GET_TIMES]Open file failed.");
				else
				{
					fprintf(fout,/*"%g %g\n"*/"%s %u\n",disk.serial.c_str(),times);
					fclose(fout);
				}
			}
			{
				print("INFO","[GET_TIMES]Write new AutoCopy.txt.");
				ofstream fout((output+"\\AutoCopy.txt").c_str());
				if(fout)
				{
					fout<<++numbers<<endl;
					fout.close();
				}
				else
					print("ERROR","[GET_TIMES]Open file failed.");
			}
		}
		else
		{
			print("INFO","[GET_TIMES]It is an old USB flash drive.");
			times=mapping[disk.serial];
		}
	}
	else
		if(!file_exists(input+"\\ac\\AutoCopy\\This_folder.txt"))
		{
			print("INFO","[GET_TIMES]Find a new folder.");
			print("INFO","[GET_TIMES]Creating and writing This_folder.txt.");
			cmd("md \""+input+"\\ac\\AutoCopy\"");
			vector<string> temp;
			temp.clear();
			get_files(input+"\\ac",temp,"temp.txt");
			if(!temp.size())
				cmd("attrib +h +s \""+input+"\\ac\"");
			temp.clear();
			get_files(input+"\\ac\\AutoCopy",temp,"temp.txt");
			if(!temp.size())
				cmd("attrib +h +s \""+input+"\\ac\\AutoCopy\"");
			ofstream fout1((input+"\\ac\\AutoCopy\\This_folder.txt").c_str());
			if(fout1)
			{
				fout1<<numbers<<endl;
				fout1.close();
			}
			else
				print("ERROR","[GET_TIMES]Open file failed.");
			cmd("attrib +r +h +s \""+input+"\\ac\\AutoCopy\\This_folder.txt\"");
			print("INFO","[GET_TIMES]Created a new This_folder.txt with [numbers]="+turn_string(numbers)+" in it.");
			times=numbers;
			print("INFO","[GET_TIMES]Write new AutoCopy.txt.");
			ofstream fout2((output+"\\AutoCopy.txt").c_str());
			if(fout2)
			{
				fout2<<++numbers<<endl;
				fout2.close();
			}
			else
				print("ERROR","[GET_TIMES]Open file failed.");
		}
		else
		{
			ifstream fin((input+"\\ac\\AutoCopy\\This_folder.txt").c_str());
			if(fin)
			{
				fin>>times;
				fin.close();
			}
			else
			{
				print("ERROR","[GET_TIMES]Open file failed. Turn to "+turn_string(numbers)+".");
				times=numbers++;
			}
			if(times<=0)
			{
				print("ERROR","[GET_TIMES][times] is wrong. It is "+turn_string(times)+". Turn to "+turn_string(numbers)+".");
				times=numbers++;
			}
		}
	print("INFO","[GET_TIMES]Process knew the [times]="+turn_string(times)+".");
	vector<pair<string,pair<pair<size_t,string>,string > > >all_temp;
	all_temp.clear();
	while(all.size()<=times)
		all.push_back(all_temp);
	return;
}

inline void READ_CONFIG()
{
	print("INFO","[READ_CONFIG]Start to read configuration.");
	config.clear();
	if(!file_exists(config_path) || !config.read())
	{
		print("WARN","[READ_CONFIG]Can't find config.txt or read failed.");
		config.add(VariableName(input),"复制文件的路径。如果USE_USB_SERIAL_INSTREAT_OF_TIMES为0，则程序会自动在input/ac/AutoCopy/This_folder.txt文件中保存[times]信息，请勿删除此文件！（默认隐藏+系统+只读）（请注意路径的合法性，必须使用绝对路径，末尾不带/或\\）","S",input);
		config.add(VariableName(output),"输出文件的路径。会在output/AutoCopy.txt保存下一个新文件夹数。具体U盘文件会复制至output/[times]。（注意！！！此路径不能含有空格等字符，否则会出错，导致文件混乱，建议使用_字符或首字母大写代替。请注意路径的合法性，必须使用绝对路径，末尾不带/或\\）","S",output);
		config.add(VariableName(version_use_newest),"自动更新版本的模式。（false：使用推荐版本；true：使用最新版本）","B",turn_string(version_use_newest));
		config.add(VariableName(GET_hwndDOS_LEVEL),"获取hwndDOS（控制台句柄）的检查级别（此部分详见config说明.txt）","I",turn_string(GET_hwndDOS_LEVEL));
		config.add(VariableName(disappear),"程序运行时是否隐藏。（false：不隐藏；true：隐藏）","B",turn_string(disappear));
		config.add(VariableName(LOG_LEVEL),"日志输出级别（仅限INFO/WARN/ERROR/DEBUG/FAULT/ALL），一般情况下，DEBUG和ALL的作用是一样的","S",LOG_LEVEL);
		config.add(VariableName(MAX_CPU_USEAGE),"CPU占用最小值。当CPU占用率（百分制）大于MAX_CPU_USEAGE时，就会减少NOW_PTHREAD，即减少最多线程数，减缓复制速度，防止电脑过卡操作不流畅（最大值为100，最小值为0）","U",turn_string(MAX_CPU_USEAGE));
		config.add(VariableName(MIN_CPU_USEAGE),"CPU占用最大值。当CPU占用率（百分制）小于MIN_CPU_USEAGE时，就会增大NOW_PTHREAD，即增加最多线程数，加快复制速度，尽可能使用CPU（最大值为100，最小值为0）","U",turn_string(MIN_CPU_USEAGE));
		config.add(VariableName(MAX_PTHREAD),"最大线程数。NOW_PTHREAD的最大值不超过MAX_PTHREAD（最小值为NOW_PTHREAD）","U",turn_string(MAX_PTHREAD));
		config.add(VariableName(MIN_PTHREAD),"最小线程数。NOW_PTHREAD的最小值不超过MIN_PTHREAD（最大值为NOW_PTHREAD，最小值为0）","U",turn_string(MIN_PTHREAD));
		config.add(VariableName(NOW_PTHREAD),"程序启动时的默认线程数。如果程序启动Change_NOW_PTHREAD_base_on_CpuUseage线程失败，就会固定使用NOW_PTHREAD作为运行线程数（最大值为MAX_PTHREAD，最小值为MIN_PTHREAD）","U",turn_string(NOW_PTHREAD));
		config.add(VariableName(TIME_OF_WAIT_CPU),"更新CPU占用率的时间间隔。Change_NOW_PTHREAD_base_on_CpuUseage线程的更新时间（不建议过小，可能会出现除法BUG；不建议太大，程序退出时需要等待此线程先退出）","U",turn_string(TIME_OF_WAIT_CPU));
		config.add(VariableName(TIME_OF_CREATE_PTHREAD),"创建进程组的等待时间（当NOW_PTHREAD不为0时等待）。单位：毫秒(ms)（1000ms=1s）（最小值为0）","U",turn_string(TIME_OF_CREATE_PTHREAD));
		config.add(VariableName(USE_USB_SERIAL_INSTEAT_OF_TIMES),"是否使用U盘特征码代替写入U盘的[times]。警告：未完成功能！！！（true：使用；false：不使用）","B",turn_string(USE_USB_SERIAL_INSTEAT_OF_TIMES));
		config.add(VariableName(TIME_OF_DETECT_INTERVAL),"检测U盘是否插入或拔出的时间间隔。单位：毫秒(ms)（1000ms=1s）（最小值为0）","U",turn_string(TIME_OF_DETECT_INTERVAL));
		config.add(VariableName(SORT_LIST),"是否给list文件排序（true：需要；false：不需要）","B",turn_string(SORT_LIST));
		config.add(VariableName(COPY_MODE),"复制文件的模式。（此部分详见config说明.txt）","I",turn_string(COPY_MODE));
		if(config.write())
			print("INFO","[READ_CONFIG]Created a new config.txt with default configuration in it.");
		else
			print("ERROR","[READ_CONFIG]Can't create a new config.txt.");
	}
	else
	{
		config.fix();
		for(unsigned int i=0;i<config.size();i++)
		{
			if(config.get(i,CONFIGURATION_NAME)==VariableName(input) && config.get(i,CONFIGURATION_TYPE)=="S")
				input=config.get(i,CONFIGURATION_VALUE);
			if(config.get(i,CONFIGURATION_NAME)==VariableName(output) && config.get(i,CONFIGURATION_TYPE)=="S")
				output=config.get(i,CONFIGURATION_VALUE);
			if(config.get(i,CONFIGURATION_NAME)==VariableName(version_use_newest) && config.get(i,CONFIGURATION_TYPE)=="B")
				version_use_newest=turn_int(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(GET_hwndDOS_LEVEL) && config.get(i,CONFIGURATION_TYPE)=="I")
				GET_hwndDOS_LEVEL=turn_int(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(disappear) && config.get(i,CONFIGURATION_TYPE)=="B")
				disappear=turn_bool(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(LOG_LEVEL) && config.get(i,CONFIGURATION_TYPE)=="S")
				LOG_LEVEL=turn_bool(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(MAX_CPU_USEAGE) && config.get(i,CONFIGURATION_TYPE)=="U")
				MAX_CPU_USEAGE=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(MIN_CPU_USEAGE) && config.get(i,CONFIGURATION_TYPE)=="U")
				MIN_CPU_USEAGE=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(MAX_PTHREAD) && config.get(i,CONFIGURATION_TYPE)=="U")
				MAX_PTHREAD=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(MIN_PTHREAD) && config.get(i,CONFIGURATION_TYPE)=="U")
				MIN_PTHREAD=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(NOW_PTHREAD) && config.get(i,CONFIGURATION_TYPE)=="U")
				NOW_PTHREAD=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(TIME_OF_WAIT_CPU) && config.get(i,CONFIGURATION_TYPE)=="U")
				TIME_OF_WAIT_CPU=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(TIME_OF_CREATE_PTHREAD) && config.get(i,CONFIGURATION_TYPE)=="U")
				TIME_OF_CREATE_PTHREAD=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(USE_USB_SERIAL_INSTEAT_OF_TIMES) && config.get(i,CONFIGURATION_TYPE)=="B")
				USE_USB_SERIAL_INSTEAT_OF_TIMES=turn_bool(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(TIME_OF_DETECT_INTERVAL) && config.get(i,CONFIGURATION_TYPE)=="U")
				TIME_OF_DETECT_INTERVAL=turn_unsigned_long_long(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(SORT_LIST) && config.get(i,CONFIGURATION_TYPE)=="B")
				SORT_LIST=turn_bool(config.get(i,CONFIGURATION_VALUE));
			if(config.get(i,CONFIGURATION_NAME)==VariableName(COPY_MODE) && config.get(i,CONFIGURATION_TYPE)=="I")
				COPY_MODE=turn_int(config.get(i,CONFIGURATION_VALUE));
		}
	}
	if(LOG_LEVEL!="INFO" && LOG_LEVEL!="WARN" && LOG_LEVEL!="ERROR" && LOG_LEVEL!="DEBUG" && LOG_LEVEL!="FAULT" && LOG_LEVEL!="ALL")
	{
		LOG_LEVEL="ALL";
		print("WARN","[READ_CONFIG][LOG_LEVEL] is illegal. Changing it to ALL");
	}
	if(MIN_CPU_USEAGE>MAX_CPU_USEAGE)
	{
		print("WARN","[READ_CONFIG][MIN_CPU_USEAGE] is bigger than [MAX_CPU_USEAGE]. Swaping them.");
		swap(MAX_CPU_USEAGE,MIN_CPU_USEAGE);
	}
	if(MAX_CPU_USEAGE>100)
	{
		print("WARN","[READ_CONFIG][MAX_CPU_USEAGE] is too big. Changing it to 100.");
		MAX_CPU_USEAGE=100;
	}
	if(MIN_CPU_USEAGE>100)
	{
		print("WARN","[READ_CONFIG][MIN_CPU_USEAGE] is too big. Changing it to 100.");
		MIN_CPU_USEAGE=100;
	}
	if(MIN_PTHREAD>MAX_PTHREAD)
	{
		print("WARN","[READ_CONFIG][MIN_PTHREAD] is bigger than [MAX_PTHREAD]. Swaping them.");
		swap(MAX_PTHREAD,MIN_PTHREAD);
	}
	if(MAX_PTHREAD==0)
	{
		print("WARN","[READ_CONFIG][MAX_PTHREAD]=0. Changing it to 1.");
		MAX_PTHREAD=1;
	}
	if(NOW_PTHREAD<MIN_PTHREAD)
	{
		print("WARN","[READ_CONFIG][NOW_PTHREAD] is too small. Changing it to [MIN_PTHREAD].");
		NOW_PTHREAD=MIN_PTHREAD;
	}
	if(NOW_PTHREAD>MAX_PTHREAD)
	{
		print("WARN","[READ_CONFIG][NOW_PTHREAD] is too big. Changing it to [MAX_PTHREAD].");
		NOW_PTHREAD=MAX_PTHREAD;
	}
	if(GET_hwndDOS_LEVEL>3 || GET_hwndDOS_LEVEL<0)
	{
		print("WARN","[READ_CONFIG][GET_hwndDOS_LEVEL] is not in range. Changing it to default.");
		GET_hwndDOS_LEVEL=2;
	}
	if(COPY_MODE>10 || COPY_MODE<-10)
	{
		print("WARN","[READ_CONFIG][COPY_MODE] is not in range. Changing it to default.");
		COPY_MODE=7;
	}
	if(check_path(input)==0)
	{
		print("WARN","[READ_CONFIG][input] is not legal. Changing it to default. Don't add '/' or '\\' on the end.");
		input="A:";
	}
	if(check_path(output)==0)
	{
		print("WARN","[READ_CONFIG][output] is not legal. Changing it to default. Don't add '/' or '\\' on the end.");
		output="C:\\UBackUp\\temp";
	}
	for(unsigned int i=0;i<output.size();i++)
		if(output[i]==' ')
		{
			print("WARN","[READ_CONFIG]Blank in [output]. Deleting all blank characters.");
			output=string_dislodge(output,STRING_DISLODGE_BLANK);
			break;
		}
	print("INFO","[READ_CONFIG]Process knew: [input]=\""+input+"\" [output]=\""+output+"\\[time]\""+
				" [MAX_CPU_USEAGE]="+turn_string(MAX_CPU_USEAGE)+" [MIN_CPU_USEAGE]="+turn_string(MIN_CPU_USEAGE)+
				" [NOW_PTHREAD]="+turn_string(NOW_PTHREAD)+" [MAX_PTHREAD]="+turn_string(MAX_PTHREAD)+" [MIN_PTHREAD]="+turn_string(MIN_PTHREAD)+
				" [TIME_OF_DETECT_INTERVAL]="+turn_string(TIME_OF_DETECT_INTERVAL)+
				" [TIME_OF_CREATE_PTHREAD]="+turn_string(TIME_OF_CREATE_PTHREAD)+
				" [TIME_OF_WAIT_CPU]="+turn_string(TIME_OF_WAIT_CPU)+
				" [USE_USB_SERIAL_INSTEAT_OF_TIMES]="+turn_string(USE_USB_SERIAL_INSTEAT_OF_TIMES)+
				" [GET_hwndDOS_LEVEL]="+turn_string(GET_hwndDOS_LEVEL)+
				" [COPY_MODE]="+turn_string(COPY_MODE)+
				" [SORT_LIST]="+turn_string(SORT_LIST)+
				" [disappear]="+turn_string(disappear)+
				". Now rewriting it.");
	if(!config.write())
		print("ERROR","[READ_CONFIG]Can't rewrite the configuration.");
	input_is_u=((input.size()==2 && input[1]==':')&&USE_USB_SERIAL_INSTEAT_OF_TIMES);
	print("DEBUG","[READ_CONFIG][input_is_u]="+turn_string(input_is_u));
	return;
}

inline void READ_SAVED_FILES_LISTS()
{
	try
	{
		print("INFO","[READ_SAVED_FILES_LISTS]Start to read saved_file_lists.");
		all.clear();
		vector<pair<string,pair<pair<size_t,string>,string> > >temp;
		string in_temp;
		for(unsigned int i=0;i<numbers;i++)
		{
			temp.clear();
			if(i==0)
			{
				all.push_back(temp);
				continue;
			}
			print("INFO","[READ_SAVED_FILES_LISTS]Read lists\\latest_list_"+turn_string(i)+".txt");
			ifstream fin(("lists\\latest_list_"+turn_string(i)+".txt").c_str());
			if(!fin)
			{
				print("ERROR","[READ_SAVED_FILES_LISTS]Open file error.");
				all.push_back(temp);
				continue;
			}
			fin>>in_temp;fin>>in_temp;//all
			unsigned int orders=turn_unsigned_int(in_temp);
			for(unsigned int j=0;j<orders;j++)
			{
				string md5,SIZE,time,path;
				fin>>in_temp;fin>>in_temp;	//order
				fin>>in_temp;fin>>md5;		//md5
				fin>>in_temp;fin>>in_temp;	//time
				time=in_temp;
				for(unsigned int k=0;k<2;k++)
				{
					fin>>in_temp;
					time+=" "+in_temp;
				}
				fin>>in_temp;fin>>SIZE;		//size
				fin>>in_temp;fin.get();		//name
				fin.get();					//"
				path.clear();
				char in_temp;
				while(1)
				{
					in_temp=fin.get();
					if(in_temp=='\n')
						break;
					path+=in_temp;
				}
				path.erase(path.size()-1,1);//"
				temp.push_back(make_pair(path,make_pair(make_pair(turn_unsigned_int(SIZE),time),md5)));
			}
			fin.close();
			all.push_back(temp);
			print("DEBUG","[READ_SAVED_FILES_LISTS]Readed "+turn_string(i+1)+" file"+((i==0)?".":"s."));
			Sleep(0);
		}
		print("INFO","[READ_SAVED_FILES_LISTS]Read saved_file_lists finished.");
	}
	catch(...)
	{
		print("FAULT","[READ_SAVED_FILES_LISTS]Unknow error.");
	}
	return;
}

inline void READ_MAPPINGS()
{
	try
	{
		print("INFO","[READ_MAPPINGS]Start to read mapping.");
		mapping.clear(); 
		ifstream fin((output+"\\mapping.txt").c_str());
		if(!fin)
		{
			print("ERROR","[READ_MAPPINGS]Open file error.");
			return;
		}
		string serial,TIMES;
		while(fin>>serial>>TIMES)
			mapping[serial]=turn_unsigned_int(TIMES);
		fin.close();
	}
	catch(...)
	{
		print("FAULT","[READ_MAPPINGS]Unknow error.");
	}
	return;
}

inline void COMPAIE_FILES_LISTS()
{
	try
	{
		print("INFO","[COMPAIE_FILES_LISTS]Compare saved files list and new files list.");
		need.clear();
		need_f.clear();
		pthread_compare_files_tids.Clear();
		for(unsigned int k=0;k<files_all.size();)
		{
			while(pthread_compare_files_tids.length()<NOW_PTHREAD && k<files_all.size())
			{
				if(folder_exist(input,times))
				{
					folder_isnot_exist=1;
					break;
				}
				if(EXITS)
					break;
				pthread_compare_files_i=k++;
				pthread_compare_files_input=input;
				pthread_compare_files_output=output+"\\"+turn_string(times);
				pthread_compare_files_release=0;
				pthread_t tid;
				int res=pthread_create(&tid,NULL,compare_files,NULL);
				if(res)
				{
					print("WARN","[COMPAIE_FILES_LISTS]Pthread compare_file create error. [res]="+turn_string(res)+". [k]="+turn_string(k)+".");
					print("INFO","[COMPAIE_FILES_LISTS]Using main function.");
					compare_files(NULL);
					continue;
				}
				pthread_compare_files_tids.push_front(tid);
				while(1)
				{
					if(pthread_compare_files_release)
						break;
					Sleep(1);
				}
			}
			if(folder_isnot_exist)
				break;
			if(EXITS)
				break;
			for(unsigned int i=0;i<pthread_compare_files_tids.length();i++)
				if(pthread_lives(pthread_compare_files_tids[i])!=PTHREAD_ALIVE)
					pthread_compare_files_tids.pop_index(i--);
			if(NOW_PTHREAD)
				Sleep(TIME_OF_CREATE_PTHREAD);
			else
				Sleep(TIME_OF_WAIT_CPU);
		}
		while(pthread_compare_files_tids.length())
			pthread_join(pthread_compare_files_tids.pop_front(),NULL);
		need_f.clear();
		print("INFO","[COMPAIE_FILES_LISTS]Compare files finished.");
		print("DEBUG","[COMPAIE_FILES_LISTS]Need copy files: "+turn_string(need.size()));
	}
	catch(...)
	{
		print("FAULT","[COMPAIE_FILES_LISTS]Unknow error.");
	}
	return;
}

inline void COPY_FILES_LISTS()
{
	try
	{
		print("INFO","[COPY_FILES_LISTS]Start to copy recorded files.");
		pthread_compare_files_tids.Clear();
		for(unsigned int k=0;k<need.size();)
		{
			while(pthread_compare_files_tids.length()<NOW_PTHREAD && k<files_all.size())
			{
				if(folder_exist(input,times))
				{
					folder_isnot_exist=1;
					break;
				}
				if(EXITS)
					break;
				pthread_copy_file_i=k++;
				pthread_copy_file_input=input;
				pthread_copy_file_output=output+"\\"+turn_string(times);
				pthread_copy_file_release=0;
				pthread_t tid;
				int res=pthread_create(&tid,NULL,copy_file,NULL);
				if(res)
				{
					print("WARN","[COPY_FILES_LISTS]Pthread copy_files create error. [res]="+turn_string(res));
					print("INFO","[COPY_FILES_LISTS]Using main function.");
					copy_file(NULL);
					continue;
				}
				pthread_copy_file_tids.push_front(tid);
				while(1)
				{
					if(pthread_copy_file_release)
						break;
					Sleep(1);
				}
			}
			if(folder_isnot_exist)
				break;
			if(EXITS)
				break;
			for(unsigned int i=0;i<pthread_copy_file_tids.length();i++)
				if(pthread_lives(pthread_copy_file_tids[i])!=PTHREAD_ALIVE)
					pthread_copy_file_tids.pop_index(i--);
			if(NOW_PTHREAD)
				Sleep(TIME_OF_CREATE_PTHREAD);
			else
				Sleep(TIME_OF_WAIT_CPU);
		}
		while(pthread_copy_file_tids.length())
			pthread_join(pthread_copy_file_tids.pop_front(),NULL);
		print("INFO","[COPY_FILES_LISTS]Copy files finished.");
	}
	catch(...)
	{
		print("FAULT","[COPY_FILES_LISTS]Unknow error.");
	}
	return;
}

