/***define***/
inline string turn_string_GetLastError();
inline string turn_string_hwndDOS();
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
				special_logger("error.txt","ERROR","[GET_HWNDDOS]Failed.");
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
		special_logger("error.txt","ERROR","[times]="+turn_string(times)+" The folder disappeared unexpectedly.");
		return 1;
	}
	return 0;
}

inline void print(string logLevel,string printMessage)
{
	ofstream out("CON");
	if(out)
	{
		out<<printMessage<<endl;
		out.close();
	}
	else
		logger_cache_write("ERROR","Open CON error.");
	logger_cache_write(logLevel,printMessage);
	return;
}

inline void print_loop(string logLevel,string printMessage)
{
	ofstream out("CON");
	if(out)
	{
		out<<printMessage<<endl;
		out.close();
	}
	if(!dis)
		logger_cache_write(logLevel,printMessage);
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
		cmd(plug_path);
		return 1;
	}
	return 0;
}

