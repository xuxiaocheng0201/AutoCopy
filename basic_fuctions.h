/***define***/
inline string turn_string_GetLastError();
inline string turn_string_hwndDOS();
inline string turn_string_time(SYSTEMTIME time);
inline bool compare_time(string a,string b);
inline bool check_hwndDOS();
inline bool folder_exist(string input,unsigned int times);
inline void print(string logLevel,string printMessage);
inline void print_loop(string logLevel,string printMessage);
inline void cmd(string Order);
inline bool search_plug(string plug_path);

/***Function***/
inline string turn_string_GetLastError()
{
	GetLastError_string_turn.clear();
	GetLastError_stream.clear();
	GetLastError_stream<<GetLastError();
	GetLastError_stream>>GetLastError_string_turn;
	return GetLastError_string_turn;
}

inline string turn_string_hwndDOS()
{
	hwndDOS_string.clear();
	hwndDOS_stream.clear();
	hwndDOS_stream<<hwndDOS;
	hwndDOS_stream>>hwndDOS_string;
	return hwndDOS_string;
}

inline string turn_string_time(SYSTEMTIME time)
{
	string t="";
	t=t+turn_string(time.wYear)+'-';
	t=t+turn_string(time.wMonth)+'-';
	t=t+turn_string(time.wDay)+' ';
	t=t+turn_string(time.wHour)+':';
	t=t+turn_string(time.wMinute)+':';
	t=t+turn_string(time.wSecond)+' ';
	t=t+turn_string(time.wMilliseconds);
	return t;
}

inline bool compare_time(string a,string b)
{
	// TODO
	return 0;
}

inline bool check_hwndDOS()
{
	if(hwndDOS!=NULL)
	{
		if(GET_hwndDOS_LEVEL==0)
			return 1;
		if(GET_hwndDOS_LEVEL==1 && GetLastError()==0)
			return 1;
		if(GET_hwndDOS_LEVEL==2 || (GET_hwndDOS_LEVEL==3 && GetLastError()==0))
		{
			print("INFO","[check_hwndDOS]Messaging users for permit.");
			if(MessageBox(NULL,TEXT(("Choose whether run ahead. The hwndDOS is "+turn_string_hwndDOS()).c_str()),TEXT("UNKNOW"),MB_YESNO|MB_ICONWARNING)!=IDYES)
				EXITS=1;
			if(EXITS)
			{
				print("FAULT","[GET_HWNDDOS]Failed.");
				MessageBox(NULL,TEXT("Please run AutoCopy.exe again or change GET_hwndDOS_LEVEL in configuration!"),TEXT("FAULT"),MB_OK|MB_ICONERROR);
			}
			return 1;
		}
	}
	return 0;
}

inline bool folder_exist(string input,unsigned int times)
{
	if(!file_exists(input+"\\ac\\AutoCopy\\This_folder.txt"))
	{
		print("ERROR","The folder disappeared unexpectedly.");
		return 1;
	}
	return 0;
}

inline void print(string logLevel,string printMessage)
{
	if(LOG_LEVEL=="DEBUG" && !(logLevel=="DEBUG" || logLevel=="FAULT" || logLevel=="ERROR" || logLevel=="WARN" || logLevel=="INFO"))
		return;
	if(LOG_LEVEL=="FAULT" && (logLevel=="DEBUG"))
		return;
	if(LOG_LEVEL=="ERROR" && (logLevel=="DEBUG" || logLevel=="FAULT"))
		return;
	if(LOG_LEVEL=="WARN" && (logLevel=="DEBUG" || logLevel=="FAULT" || logLevel=="ERROR"))
		return;
	if(LOG_LEVEL=="INFO" && (logLevel=="DEBUG" || logLevel=="FAULT" || logLevel=="ERROR" || logLevel=="WARN"))
		return;
	ofstream out("CON");
	if(out)
	{
		out<<printMessage<<endl;
		out.close();
	}
	else
		logger("ERROR","Open CON error.");
	logger(logLevel,printMessage);
	return;
}

inline void print_loop(string logLevel,string printMessage)
{
	if(LOG_LEVEL=="DEBUG" && !(logLevel=="DEBUG" || logLevel=="FAULT" || logLevel=="ERROR" || logLevel=="WARN" || logLevel=="INFO"))
		return;
	if(LOG_LEVEL=="FAULT" && (logLevel=="DEBUG"))
		return;
	if(LOG_LEVEL=="ERROR" && (logLevel=="DEBUG" || logLevel=="FAULT"))
		return;
	if(LOG_LEVEL=="WARN" && (logLevel=="DEBUG" || logLevel=="FAULT" || logLevel=="ERROR"))
		return;
	if(LOG_LEVEL=="INFO" && (logLevel=="DEBUG" || logLevel=="FAULT" || logLevel=="ERROR" || logLevel=="WARN"))
		return;
	ofstream out("CON");
	if(out)
	{
		out<<printMessage<<endl;
		out.close();
	}
	if(!disappear)
		logger(logLevel,printMessage);
	return;
}

inline void cmd(string Order)
{
	print("CMD",Order);
	try
	{
		system(Order.c_str());
	}
	catch(...)
	{
		print("FAULT","[CMD]Unknow error");
	}
	return;
}

inline bool search_plug(string plug_path)
{
	if(file_exists(plug_path))
	{
		print("INFO","Detected that "+plug_path+" exists.");
		print("DEBUG","The md5 of the plug is "+getMD5(plug_path)+".");
		print("DEBUG","The sha1 of the plug is "+getSHA1(plug_path)+".");
		print("DEBUG","The crc32 of the plug is "+getCRC32(plug_path)+".");
		try
		{
			cmd(plug_path);
		}
		catch(...)
		{
			print("FAULT","[search_plug]Unknow error");
		}
		return 1;
	}
	return 0;
}

