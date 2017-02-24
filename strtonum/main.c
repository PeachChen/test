#include <stdio.h>
#include <string.h>

//我自己的
bool str_to_num_back(const char* str,int& outNum)
{
	if(str==NULL)
		return false;
	int length=strlen(str);
	int total=0;
	for(int i=length-1;i>=0;--i)
	{
		if(str[i]>='0' and str[i]<='9')
		{
			int n=str[i]-'0';
			int temp=i;
			while(temp<length-1)
			{
			  	n*=10;
				temp++;
			}
			total+=n;
		}
		else
		  return false;
	}
	outNum=total;
	return true;
}

bool str_to_num(const char* str,int& outNum)
{
	if(str==NULL)
		return false;
	int length=strlen(str);
	int total=0;
	for(int i=0;i<length;++i)
	{
		if(str[i]>='0' and str[i]<='9')
		{
			int n=str[i]-'0';
			int temp=i;
			while(temp<length-1)
			{
			  	n*=10;
				temp++;
			}
			total+=n;
		}
		else
		  return false;
	}
	outNum=total;
	return true;
}

//网上的(没有加非字符判断)
int str2int(const char* str)
{
	int temp=0;
	const char* pstr=str;
	if(*str=='-' || *str=='+')
	{
		str++;
	}
	while(*str!='\0')
	{
		if((*str<'0') || (*str>'9'))
		{
			break;
		}
		temp=temp*10+(*str-'0');
		str++;
	}
	if(*pstr=='-')
	{
		temp=-temp;
	}
	return temp;
}
//我认为最好的(数字字符asc||码后4位就是 对应的数字) 没有考虑各种细节
int str2intbest(const char* str)
{
	int temp=0;
	const char* pstr=str;
	if(*str=='-' || *str=='+')
	{
		str++;
	}
	while(*str!='\0')
	{
		if((*str<'0') || (*str>'9'))
		{
			break;
		}
		temp=temp*10+(int)(*str&0x0f);
		str++;
	}
	if(*pstr=='-')
	{
		temp=-temp;
	}
	return temp;
}

int main()
{
	int temp;
	if(str_to_num_back("4d00",temp))
	{
		printf("4d00 %d\n",temp);
	}
	if(str_to_num_back("0400",temp))
	{
		printf("0400 %d\n",temp);
	}
	if(str_to_num_back("3521485",temp))
	{
		printf("3521485 %d\n",temp);
	}
	if(str_to_num("400s",temp))
	{
		printf("400s %d\n",temp);
	}
	
	printf("str2int %d\n",str2int("-2514"));
	printf("str2int %d\n",str2int("-25014"));
	printf("str2int %d\n",str2int("2514S"));
	printf("str2int %d\n",str2int("-251400"));
	printf("str2int %d\n",str2int("00251400"));
	printf("str2int %d\n",str2int("-02514"));
	printf("str2int %d\n",str2int("-02ss5d14"));
	printf("str2intb %d\n",str2intbest("-2514"));
	printf("str2intb %d\n",str2intbest("-25014"));
	printf("str2intb %d\n",str2intbest("2514S"));
	printf("str2intb %d\n",str2intbest("-251400"));
	printf("str2intb %d\n",str2intbest("00251400"));
	printf("str2intb %d\n",str2intbest("-02514"));
	printf("str2intb %d\n",str2intbest("-02ss5d14"));

	return 0;
}
