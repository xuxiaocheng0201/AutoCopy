/***define***/
void *detected_run_configuration(void *rags);
bool compare_by_name(const pair<string,pair<pair<size_t,string>,string> >&a,const pair<string,pair<pair<size_t,string>,string> >&b);
void *stone_all(void *rags);
void *change_MAX_PTHREAD_based_on_CpuUseage(void *rags);
void *compare_files(void *rags);
void *copy_file(void *rags);

/***function***/
void *detected_run_configuration(void *rags)
{
	thread_detected_run_configuration_exit=0;
	while(1)
	{
		if(thread_detected_run_configuration_should_finished)
			break;
		print_loop("INFO","[detected_run_configuration]Start to read run configuration.");
		run_config.clear();
		if(!file_exists("run_config.txt") || !run_config.read("run_config.txt"))
		{
			print("WARN","[detected_run_configuration]Can't find run_config.txt or read failed.");
			run_config.add(VariableName(disappear),"程序运行时是否隐藏。（false：不隐藏；true：隐藏）","B",turn_string(disappear));
			run_config.add(VariableName(EXITS),"是否退出进程。（false：不退出；true：退出）","B",turn_string(EXITS));
			if(run_config.write("run_config.txt"))
				print("INFO","[detected_run_configuration]Created a new run_config.txt with current configuration ([disappear]="+turn_string(disappear)+" [EXITS]="+turn_string(EXITS)+") in it.");
			else
				print("ERROR","[detected_run_configuration]Can't rewrite the run_config.txt.");
		}
		else
		{
			run_config.fix();
			for(unsigned int i=0;i<run_config.size();i++)
			{
				if(run_config.get(i,CONFIGURATION_NAME)==VariableName(disappear) && run_config.get(i,CONFIGURATION_TYPE)=="B")
					disappear=turn_bool(run_config.get(i,CONFIGURATION_VALUE));
				if(run_config.get(i,CONFIGURATION_NAME)==VariableName(EXITS) && run_config.get(i,CONFIGURATION_TYPE)=="B")
					EXITS=turn_bool(run_config.get(i,CONFIGURATION_VALUE));
			}
		}
		if(EXITS)
		{
			print("INFO","[detected_run_configuration]Process knew: Exits.");
			break;
		}
		if(disappear==now_dis)
		{
			print_loop("INFO","[detected_run_configuration][disappear] didn't change.");
			Sleep(TIME_OF_DETECT_INTERVAL);
			continue;
		}
		print("INFO","[detected_run_configuration]Process knew: "+(string)(disappear?"Disappear.":"Appear."));
		if(disappear)
		{
			if(hwndDOS==NULL)
			{
				print("WARN","[detected_run_configuration][hwndDOS] is NULL, ignore it.");
				Sleep(TIME_OF_DETECT_INTERVAL);
				continue;
			}
			print("INFO","[detected_run_configuration]Disappearing.");
			ShowWindow(hwndDOS,SW_HIDE);
			if(GetLastError())
			{
				print("ERROR","[detected_run_configuration]ShowWindow failed in hide.");
				print("DEBUG","[detected_run_configuration][ShowWindow]GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
				SetLastError(0);
			}
			else
				now_dis=1;
		}
		else
		{
			if(hwndDOS==NULL)
			{
				print("WARN","[detected_run_configuration][hwndDOS] is NULL, ignore it.");
				Sleep(TIME_OF_DETECT_INTERVAL);
				continue;
			}
			print("INFO","[detected_run_configuration]Appearing.");
			ShowWindow(hwndDOS,SW_SHOW);
			if(GetLastError())
			{
				print("ERROR","[detected_run_configuration]ShowWindow failed in show.");
				print("DEBUG","[detected_run_configuration][ShowWindow]GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
				SetLastError(0);
			}
			else
				now_dis=0;
		}
		Sleep(TIME_OF_DETECT_INTERVAL);
	}
	thread_detected_run_configuration_exit=1;
	return NULL;
}

bool compare_by_name(const pair<string,pair<pair<size_t,string>,string> >&a,const pair<string,pair<pair<size_t,string>,string> >&b)
{
	if(a.first.size()==b.first.size())
	{
		for(unsigned i=0;i<a.first.size();i++)
			if(a.first[i]!=b.first[i])
				return a.first[i]<b.first[i];
		if(a.second.first.first==b.second.first.first)
		{
			if(a.second.first.second==b.second.first.second)
			{
				for(unsigned i=0;i<a.second.second.size();i++)
					if(a.second.second[i]!=b.second.second[i])
						return a.second.second[i]<b.second.second[i];
				return 0;
			}
			return compare_time(a.second.first.second,b.second.first.second);
		}
		return a.second.first.first<b.second.first.first;
	}
	return a.first.size()<b.first.size();
}

void *stone_all(void *rags)
{
	try
	{
		thread_stone_exit=0;
		string input=thread_stone_input;
		bool _return=0;
		all[0].clear();
		unsigned int succeed_md5=0,succeed_size=0;
		print("INFO","[stone_all]Store the file list in cache.");
		for(unsigned int i=0;i<files_all.size();i++)
		{
			if(folder_exist(input,times))
			{
				_return=1;
				break;
			}
			size_t size=get_file_size(files_all[i]);
			string time=turn_string_time(get_file_time(files_all[i]));
			string md5=getMD5(files_all[i]);
			if(size)
				succeed_size++;
			else
				if(get_file_size_quick(files_all[i])==0)
					succeed_size++;
			if(md5!="error")
				succeed_md5++;
			all[0].push_back(make_pair(files_all[i],make_pair(make_pair(size,time),md5)));
		}
		all[times]=all[0];
		if(_return)
		{
			thread_stone_exit=1;
			return NULL;
		}
		Sleep(0);
		if(SORT_LIST)
		{
			print("INFO","[stone_all]Sort file list.");
			sort(all[times].begin(),all[times].end(),compare_by_name);
		}
		print("INFO","[stone_all]Store file list in \"lists\\latest_list_"+turn_string(times)+".txt\".");
		ofstream fout(("lists\\latest_list_"+turn_string(times)+".txt").c_str());
		if(fout)
		{
			fout<<"all: "<<all[times].size()<<endl;
			for(unsigned int i=0;i<all[times].size();i++)
				fout<<"order: "<<i+1<<" MD5: "<<all[times][i].second.second<<" time: "<<all[times][i].second.first.second<<" size: "<<all[times][i].second.first.first<<" path: \""<<all[times][i].first<<"\""<<endl;
			fout<<"all: "<<all[times].size()<<" getMD5 succeed: "<<succeed_md5<<" getSIZE succeed: "<<succeed_size<<endl;
			fout.close();
		}
		else
			print("ERROR","[stone_all]Open file error.");
		print("INFO","[stone_all]Store the file list finished.");
	}
	catch(...)
	{
		print("FAULT","[stone_all]Unknow error.");
	}
	thread_stone_exit=1;
	return NULL;
}

void *change_MAX_PTHREAD_based_on_CpuUseage(void *rags)
{
	thread_cpu_exit=0;
	while(1)
	{
		if(thread_cpu_should_finished)
			break;
		cpu=getCpuUseage.CpuUseage();
		if(cpu>MAX_CPU_USEAGE && NOW_PTHREAD>MIN_PTHREAD)
		{
			NOW_PTHREAD--;
			print("INFO","[change_MAX_PTHREAD_based_on_CpuUseage]cpu="+turn_string(cpu)+"% --NOW_PTHREAD="+turn_string(NOW_PTHREAD));
		}else 
		if(cpu<MIN_CPU_USEAGE && NOW_PTHREAD<MAX_PTHREAD)
		{
			NOW_PTHREAD++;
			print("INFO","[change_MAX_PTHREAD_based_on_CpuUseage]cpu="+turn_string(cpu)+"% ++NOW_PTHREAD="+turn_string(NOW_PTHREAD));
		}
		Sleep(TIME_OF_WAIT_CPU);
	}
	thread_cpu_exit=1;
	return NULL;
}

void *compare_files(void *rags)
{
	pthread_compare_files_release=0;
	unsigned int i=pthread_compare_files_i;
	string input=pthread_compare_files_input;
	string output=pthread_compare_files_output;
	pthread_compare_files_release=1;
	string files="";
	for(unsigned int j=input.size();j<files_all[i].size();j++)
		files+=files_all[i][j];
	string logtitle="[compare_file_"+turn_string(i)+"]";
	try
	{
		print("INFO",logtitle+"Search \""+output+files+"\".");
		if(!file_exists(output+files))
		{
			print("INFO",logtitle+"Saved file does not exist. Recording.");
			if(need_f[files]==0)
			{
				need.push_back(files);
				need_f[files]=1;
			}
			return NULL;
		}
		print("INFO",logtitle+"Saved file exists. Comparing files.");
		string in_md5=getMD5(input+files),out_md5="";
		for(unsigned int i=0;i<all[times].size();i++)
			if(all[times][i].first==input+files)
			{
				out_md5=all[times][i].second.second;
				break;
			}
		if(out_md5=="")
			out_md5=getMD5(output+files);
		if(in_md5==out_md5)
		{
			print("INFO",logtitle+"Different files. Recording.");
			if(need_f[files]==0)
			{
				need.push_back(files);
				need_f[files]=1;
			}
			return NULL;
		}
		print("INFO",logtitle+"Same files.");
		return NULL;
	}
	catch(...)
	{
		print("FAULT",logtitle+"Unknow error.");
	}
	return NULL;
}

void *copy_file(void *rags)
{
	pthread_copy_file_release=0;
	unsigned int i=pthread_copy_file_i;
	string input=pthread_copy_file_input;
	string output=pthread_copy_file_output;
	pthread_copy_file_release=1;
	string logtitle="[copy_files_"+turn_string(i)+"]";
	try
	{
		print("INFO",logtitle+"Start to copy file \""+input+need[i]+"\" to \""+output+need[i]+"\".");
		bool hidden=0,systems=0;
		if(GetFileAttributes((input+need[i]).c_str()) & FILE_ATTRIBUTE_SYSTEM)
		{
			systems=1;
			print("WARN",logtitle+"This file is of system.");
			cmd("attrib -s \""+input+need[i]+"\"");
		}
		if(GetFileAttributes((input+need[i]).c_str()) & FILE_ATTRIBUTE_HIDDEN)
		{
			hidden=1;
			print("WARN",logtitle+"This file is hidden.");
			cmd("attrib -h \""+input+need[i]+"\"");
		}
		if(1<=abs(COPY_MODE) && abs(COPY_MODE)<=3)
		{
			cmd(("xcopy \""+input+need[i]+"\" \""+output+need[i]+"\" /v /c /q /g /h /k /o /x /y /j > nul").c_str());
//			if(res)
//			{
//				print("ERROR",logtitle+"Copy failed.");
//				print("DEBUG",logtitle+"[res]="+turn_string(res)+".");
//			}
		}
		if(4<=abs(COPY_MODE) && abs(COPY_MODE)<=6)
		{
			CopyFile((input+need[i]).c_str(),(output+need[i]).c_str(),FALSE);
			if(GetLastError())
			{
				print("ERROR",logtitle+"Copy failed.");
				print("DEBUG",logtitle+"GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
				SetLastError(0);
			}
		}
		if(7<=abs(COPY_MODE) && abs(COPY_MODE)<=9)
			if(!copy_file(input+need[i],output+need[i]))
				print("ERROR",logtitle+"Copy failed.");
		if(hidden)
			cmd("attrib +h \""+input+need[i]+"\"");
		if(systems)
			cmd("attrib +s \""+input+need[i]+"\"");
		print("INFO",logtitle+"Have already copied file \""+input+need[i]+"\" to \""+output+need[i]+"\".");
		return NULL;
	}
	catch(...)
	{
		print("FAULT",logtitle+"Unknow error.");
	}
	return NULL;
}

