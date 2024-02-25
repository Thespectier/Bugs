#include <windows.h>  
#include <stdio.h>  
#include <string.h>  
#include <malloc.h>
#include "test.h"


int findFile(char *filePath,long *Size,long *Num,long *Directory)  
{  
    char szFind[520];       //定义一个要找的文件路径的量  
    WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息  
    HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值  
    int maxpath=0;                 //存放最长路径长度
    int nowpath=0;                //存放当前路径长度
    char maxfilepath[520];      //存放最长路径文件名称
    int maxdepth=1;             //存放最深目录层数
    int nowdepth=1;             //存放当前目录层数
    LinkQueue dirqueue=InitQueue(); //得到初始目录队列
    long number=0;
    long filenum=0;
    long dirnum=0;

    strcpy(szFind,filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间     
    strcat(szFind,"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所有文件，包括目录  
    hFind=FindFirstFile(szFind,&FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件  
   
 //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去    
 
    if(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在  
    {  
       printf("目录不存在！");  
       return 0;  
    }  

   while(1)
   {
       while(1)  
      {                                                                //因为 FindFirstFile返回的 findData 中 dwFileAttributes项的值是一个组合的值  
        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //做位的与运算来判断所找到的项目是不是文件夹  
        {  
           if(FindFileData.cFileName[0]!='.')                        //每个目录下有个..目录可以返回上一层。如果不是返回目录  
            //.表示当前目录，因为每个目录下面都有两个默认目录就是..和.分别表示上一级目录和当前目录  
           {  
              (*Directory)++;               //文件夹个数加一  
              dirnum++;
              if(*Directory!=dirnum)
              {
                 printf("Directory为%ld\ndirnum为%ld\n",*Directory,dirnum);
                 system("pause");
              }
              number++;
              strcpy(szFind,filePath);  
              strcat(szFind,"\\");  
              strcat(szFind,FindFileData.cFileName);             
              EnQueue(&dirqueue,szFind,nowdepth+1); 
           }  
        }  
        else 
        {  
            (*Num)++;                                                   //文件个数加一  
            filenum++;
            if(*Num!=filenum)
            {
               printf("*Num为%ld\n,filenum为%ld\n",*Num,filenum);
               system("pause");
            }
            number++;         
            (*Size)+=(long)(FindFileData.nFileSizeLow/1024);            //对文件的大小进行统计  FindFileData.nFileSizeLow是字节大小  
            //printf("%s\\%s\n%d字节\n",filePath,FindFileData.cFileName,FindFileData.nFileSizeLow);//输出目录下的文件的路径和名称和大小 
            nowpath=strlen(filePath)+strlen(FindFileData.cFileName);
            if(nowpath>maxpath)
            {
               maxpath=nowpath;
               strcpy(maxfilepath,filePath);
               strcat(maxfilepath,"\\");
               strcat(maxfilepath,FindFileData.cFileName);
            }
        }  
        if(!FindNextFile(hFind,&FindFileData))                        //继续查找FindFirstFile函数搜索后的文件    非零表示成功，零表示失败  
           break;                                                     //因此为0时，执行break  
      }  
      
      if(DelQueue(&dirqueue,szFind,&nowdepth))
      {
         int flag=0;
         strcpy(filePath,szFind);
         strcat(szFind,"\\*.*");
         hFind=FindFirstFile(szFind,&FindFileData);
         if(nowdepth>maxdepth)
         {
            maxdepth=nowdepth;
         }
         while(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在  
         {  
            printf("%s该目录无法访问\n",filePath);  
            if(DelQueue(&dirqueue,szFind,&nowdepth))
            {
               strcpy(filePath,szFind);
               strcat(szFind,"\\*.*");
               hFind=FindFirstFile(szFind,&FindFileData);
               if(nowdepth>maxdepth)
               {
                  maxdepth=nowdepth;
               }
            }
            else
            {
               flag=1;
               break;
            }
         }
         if(flag)
         {
            printf("扫描完毕\n");
            break;           
         } 
      }
      else
      {
         printf("扫描完毕\n");
         break;
      }
   }
    FindClose(hFind);//关闭句柄  
    printf("directory为%ld,  Num为%ld\n",*Directory,*Num);
    *Directory = dirnum;
    *Num = filenum;
    printf("最长带全路径文件为%s\n长度为%d\n目录层数为%d\n",maxfilepath,maxpath,maxdepth);
    printf("文件夹和文件数量总和为：%ld\n文件夹数量为%ld\n文件数量为%ld\n",number,*Directory,*Num);
    return 0;  
}  
 
 
int main()  
{ 
    system("chcp 65001");
    long Size=0;    //定义Size为文件大小，Num为文件个数，Directory为文件夹个数  
    long Num=0;
    long Directory=0;
    char input[MAX_PATH];             //定义输入文件路径的字符数组  
    printf("输入想要遍历的目录。如：C:\\\\windows 或 C:\\\\Documents and Settings\n");  
    scanf("%s",input);  
    findFile(input,&Size,&Num,&Directory);//执行子函数findFile  
    printf("\n总共大小：%ldKB\n文件个数：%ld\n文件夹个数：%ld\n",Size,Num,Directory);  
    system("pause");//"pause"这个是一个系统指令，就是在命令行上输出一行类似于“请按任意键继续...”的字，等待用户按一个键，然后返回。可以注释掉  
    return 0;  
}  
 