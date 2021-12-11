/***define***/
void *detected_run_configuration(void *rags);
bool compare_all(pair<string,pair<size_t,pair<string,string> > >a,pair<string,pair<size_t,pair<string,string> > >b);
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
		bool old_dis=dis;
		run_config.clear();
		if(!file_exists("run_config.txt") || !run_config.read("run_config.txt"))
		{
			print("WARN","[detected_run_configuration]Can't find run_config.txt or read failed.");
			run_config.add(VariableName(dis),"程序运行时是否隐藏。（false：不隐藏；true：隐藏）","B",turn_string(dis));
			run_config.add(VariableName(EXITS),"是否退出进程。（false：不退出；true：退出）","B",turn_string(EXITS));
			if(run_config.write("run_config.txt"))
				print("INFO","[detected_run_configuration]Created a new run_config.txt with current configuration ([dis]="+turn_string(dis)+" [EXITS]="+turn_string(EXITS)+") in it.");
			else
				print("ERROR","[detected_run_configuration]Can't create a new run_config.txt.");
			if(dis==old_dis)
			{
				old_dis=!old_dis;
				print("DEBUG","[detected_run_configuration]To adapt function, changed [old_dis] to "+turn_string(old_dis)+". And it may log a error \"ShowWindow failed.\", please ignore it.");
			}
		}
		else
		{
			run_config.fix();
			for(unsigned int i=0;i<run_config.size();i++)
			{
				if(run_config.get(i,CONFIGURATION_NAME)==VariableName(dis) && run_config.get(i,CONFIGURATION_TYPE)=="B")
					dis=turn_bool(run_config.get(i,CONFIGURATION_VALUE));
				if(run_config.get(i,CONFIGURATION_NAME)==VariableName(EXITS) && run_config.get(i,CONFIGURATION_TYPE)=="B")
					EXITS=turn_bool(run_config.get(i,CONFIGURATION_VALUE));
			}
		}
		if(EXITS)
		{
			print("INFO","[detected_run_configuration]Process knew: Exits.");
			thread_detected_run_configuration_exit=1;
			return NULL;
		}
		if(dis==old_dis)
		{
			print_loop("INFO","[detected_run_configuration][dis] didn't change.");
			Sleep(TIME_OF_DETECT_INTERVAL);
			continue;
		}
		print("INFO","[detected_run_configuration]Process knew: "+(string)(dis?"Disappear.":"Appear."));
		if(dis)
		{
			if(hwndDOS==NULL)
			{
				print("FAULT","[detected_run_configuration][hwndDOS] is NULL, process exits.");
				EXITS=1;
				thread_detected_run_configuration_exit=1;
				return NULL;
			}
			print("INFO","[detected_run_configuration]Disappearing.");
			ShowWindow(hwndDOS,SW_HIDE);
		}
		else
		{
			print("INFO","[detected_run_configuration]Appearing.");
			ShowWindow(hwndDOS,SW_SHOW);
		}
		if(GetLastError())
		{
			print("ERROR","[detected_run_configuration]ShowWindow failed.");
			print("DEBUG","[detected_run_configuration][ShowWindow]GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
			SetLastError(0);
		}
		if(!logger_cache_arrange())
			logger("FAULT","Arrange logs cache error.");
		Sleep(TIME_OF_DETECT_INTERVAL);
	}
	thread_detected_run_configuration_exit=1;
	return NULL;
}

bool compare_all(pair<string,pair<size_t,pair<string,string> > >a,pair<string,pair<size_t,pair<string,string> > >b)
{
	if(a.first.size()==b.first.size())
	{
		for(unsigned i=0;i<a.first.size();i++)
			if(a.first[i]!=b.first[i])
				return a.first[i]<b.first[i];
		if(a.second.first==b.second.first)
		{
			for(unsigned i=0;i<a.second.second.first.size();i++)
				if(a.second.second.first[i]!=b.second.second.first[i])
					return a.second.second.first[i]<b.second.second.first[i];
			for(unsigned i=0;i<a.second.second.second.size();i++)
				if(a.second.second.second[i]!=b.second.second.second[i])
					return a.second.second.second[i]<b.second.second.second[i];
			return 0;
		}
		return a.second.first<b.second.first;
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
		unsigned int succeed_md5=0,succeed_sha1=0,succeed_size=0;
		print("INFO","[stone_all]Store the file list in cache.");
		for(unsigned int i=0;i<getFiles_all.size();i++)
		{
			if(folder_exist(input,times))
			{
				_return=1;
				break;
			}
			size_t size=get_file_size(getFiles_all[i]);
			string md5=getMD5(getFiles_all[i]),sha1=getSHA1(getFiles_all[i]);
			if(size)
				succeed_size++;
			if(md5!="error")
				succeed_md5++;
			if(sha1!="error")
				succeed_sha1++;
			all[0].push_back(make_pair(getFiles_all[i],make_pair(size,make_pair(md5,sha1))));
		}
		if(_return)
		{
			thread_stone_exit=1;
			return NULL;
		}
		Sleep(0);
		print("INFO","[stone_all]Compare file lists.");
		for(unsigned int i=0;i<all[times].size();i++)
		{
			unsigned int local=max(all[times].size(),all[0].size());
			for(unsigned int j=0;j<all[0].size();j++)
				if(all[times][i].first==all[0][j].first)
				{
					local=j;
					break;
				}
			if(local==max(all[times].size(),all[0].size()))
				continue;
			if(all[0][local].second.first!=0)
			{
				if(all[times][i].second.first==0)
					succeed_size++;
				all[times][i].second.first=all[0][local].second.first;
			}
			if(all[0][local].second.second.first!="error")
			{
				if(all[times][i].second.second.first=="error")
					succeed_md5++;
				all[times][i].second.second.first=all[0][local].second.second.first;
			}
			if(all[0][local].second.second.second!="error")
			{
				if(all[times][i].second.second.second=="error")
					succeed_sha1++;
				all[times][i].second.second.second=all[0][local].second.second.second;
			}
			Sleep(0);
		}
		for(unsigned int i=0;i<all[0].size();i++)
		{
			bool exist=0;
			for(unsigned int j=0;j<all[times].size();j++)
				if(all[0][i].first==all[times][j].first)
					exist=1;
			if(exist)
				continue;
			all[times].push_back(all[0][i]);
			Sleep(0);
		}
		if(SORT_LIST)
		{
			print("INFO","[stone_all]Sort file list.");
			sort(all[times].begin(),all[times].end(),compare_all);
		}
		print("INFO","[stone_all]Store file list in \"lists\\latest_list_"+turn_string(times)+".txt\".");
		ofstream fout(("lists\\latest_list_"+turn_string(times)+".txt").c_str());
		if(fout)
		{
			fout<<"all: "<<all[times].size()<<endl;
			for(unsigned int i=0;i<all[times].size();i++)
				fout<<"order: "<<i+1<<" MD5: "<<all[times][i].second.second.first<<" SHA1: "<<all[times][i].second.second.second<<" size: "<<all[times][i].second.first<<" name: \""<<all[times][i].first<<"\""<<endl;
			fout<<"all: "<<all[times].size()<<" getMD5 succeed: "<<succeed_md5<<" getSHA1 succeed: "<<succeed_sha1<<" getSIZE succeed: "<<succeed_size<<endl;
			fout.close();
		}
		else
		{
			print("ERROR","[stone_all]Open file error.");
			special_logger("error.txt","ERROR","[times]="+turn_string(times)+" [stone_all]Open file error.");
		}
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
	for(unsigned int j=input.size();j<getFiles_all[i].size();j++)
		files+=getFiles_all[i][j];
	string logtitle="[compare_file_"+turn_string(i)+"]";
	try
	{
		print("INFO",logtitle+"Search \""+output+files+"\".");
		if(!file_exists(output+files))
		{
			print("INFO",logtitle+"Saved file does not exist.");
			if(copied.find(make_pair(getMD5(input+files),getSHA1(output+files)))==copied.end())
			{
				print("INFO",logtitle+"Source file has not copied yet. Recording.");
				if(need_f[files]==0)
				{
					need.push_back(files);
					need_f[files]=1;
				}
			}
			else
				print("INFO",logtitle+"Source file has copied yet.");
			return NULL;
		}
		print("INFO",logtitle+"Saved file exists. Comparing files.");
		int result=check_file(input+files,output+files);
		if(result==0)
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
//				special_logger("error.txt","ERROR","[times]="+turn_string(times)+" "+logtitle+"[res]="+turn_string(res)+".");
//			}
		}
		if(4<=abs(COPY_MODE) && abs(COPY_MODE)<=6)
		{
			CopyFile((input+need[i]).c_str(),(output+need[i]).c_str(),FALSE);
			if(GetLastError())
			{
				print("ERROR",logtitle+"Copy failed.");
				print("DEBUG",logtitle+"GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
				special_logger("error.txt","ERROR","[times]="+turn_string(times)+" "+logtitle+"Copy failed: \""+input+need[i]+"\". GetLastError is "+turn_string_GetLastError()+". It means \""+GetLastError_string_meaning()+"\".");
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

