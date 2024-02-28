#include<windows.h>
#include<stdio.h>
#include"test_one.h"


//该函数用于查询目录结点信息是否改变并输出查询结果
int CheckDiff(DirInfoptr thisnode,char* maxname)
{
    //在该函数中查询的是目录信息结点，检查其是否发生变化以及发生了哪些变化；
    //目录结点信息包括大小，文件个数，最早时间文件以及最晚时间文件
    //其中首先要判断该目录在目录树中是否被删除，
    //再判断大小和文件个数是否改变，如果改变则输出改变
    //再判断单独判断最早时间文件和最晚时间文件是否发生变化
    //先判断其指针信息是否发生变化，
    //若没发生变化则再比较其文件信息是否发生了变化，but目前程无法实现该功能**********
    int point=0;      //存放该结点是否有信息改变，没有为0，有为1，最后作为函数返回值
    if(thisnode!=NULL)
    {
        if(thisnode->nownumber!=thisnode->originnumber)
        {
            point=1;
        }
        if(thisnode->nowsize!=thisnode->originsize)
        {
            point=1;
        }
        if(thisnode->nowFirstFile!=thisnode->originFirstFile)
        {
            point=1;
        }
        if(thisnode->nowLatestFile!=thisnode->originLatestFile)
        {
            point=1;
        }
        if(point==1)
        {
            printf("%s该目录中文件总数变化为%ld,总大小变化为%ld字节\n",maxname,(thisnode->nownumber)-(thisnode->originnumber),(thisnode->nowsize)-(thisnode->originsize));
            if(thisnode->nowFirstFile!=thisnode->originFirstFile)
            {
                printf("其中最早时间文件变为%s  ",thisnode->nowFirstFile->maxname);
                printf("大小为%ld字节  ",thisnode->nowFirstFile->size);
                printf(" 时间为%s\n",thisnode->nowFirstFile->time);
            }
            if(thisnode->nowLatestFile!=thisnode->originLatestFile)
            {
                printf("其中最晚时间文件变为%s  ",thisnode->nowLatestFile->maxname);
                printf("大小为%ld字节  ",thisnode->nowLatestFile->size);
                printf(" 时间为%s\n",thisnode->nowLatestFile->time);
            }
            printf("\n\n");
        }
    }
    else
    {
        printf("%s该目录已删除！\n\n\n",maxname);
    }
    return point;
}
//该函数用于遍历目录文件创建目录树并返回目录树树根指针
Treeptr createTREE(char *filePath,long *Size,long *Num,long *Directory,long *Treedepth)  
{  
    char szFind[520];       //定义一个要找的文件路径的量  
    WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息  
    HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值  
    char nowfilepath[520];      //存放当前访问文件路径
    int nowdepth=1;             //存放当前目录层数
    LinkQueue dirqueue=InitQueue(); //得到初始目录队列
    long filenum=0;                 //存放文件数量
    long dirnum=0;                  //存放目录数量
    Treeptr headnode;               //指向目录树树根结点
    Treeptr parentnode;             //指向当前搜索的目录结点
    Treeptr nownode;                //指向当前生成目录树结点

    strcpy(szFind,filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间     
    strcat(szFind,"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所有文件，包括目录  
    hFind=FindFirstFile(szFind,&FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件  
   
 //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去    
    if(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在  
    {  
       printf("目录不存在！\n");  
       return 0;  
    }  
   //得到初始目录树结点
   headnode = (Treeptr)malloc(sizeof(Tnode));
   strcpy(headnode->maxname,filePath);
   strcpy(headnode->name,filePath);
   headnode->flag = 1;
   headnode->brother = NULL;
   headnode->kid = NULL;
   headnode->lastptr = NULL;
   headnode->Information = NULL;
   nownode = headnode;
   parentnode = headnode;
   while(1)
   {  
       while(1)  
      {                                                                //因为 FindFirstFile返回的 findData 中 dwFileAttributes项的值是一个组合的值  
        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //做位的与运算来判断所找到的项目是不是文件夹  
        {  
           if(FindFileData.cFileName[0]!='.')                        //每个目录下有个..目录可以返回上一层。如果不是返回目录  
            //.表示当前目录，因为每个目录下面都有两个默认目录就是..和.分别表示上一级目录和当前目录  
           {   
              dirnum++;
              strcpy(szFind,filePath);  
              strcat(szFind,"\\");  
              strcat(szFind,FindFileData.cFileName);             
              EnQueue(&dirqueue,szFind,nowdepth+1); 
              createDIRnode(szFind,FindFileData.cFileName,FindFileData.ftLastWriteTime,parentnode);

           }  
        }  
        else 
        {  
            filenum++; 
            (*Size)+=(long)(FindFileData.nFileSizeLow/1024);            //对文件的大小进行统计  FindFileData.nFileSizeLow是字节大小 
            strcpy(nowfilepath,filePath);
            strcat(nowfilepath,"\\");
            strcat(nowfilepath,FindFileData.cFileName);
            createFILEnode(nowfilepath,&FindFileData,parentnode);
            
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
         while(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在  
         {  
            printf("%s该目录无法访问\n",filePath);  
            if(DelQueue(&dirqueue,szFind,&nowdepth))
            {
               strcpy(filePath,szFind);
               strcat(szFind,"\\*.*");
               hFind=FindFirstFile(szFind,&FindFileData);
            }
            else
            {
               flag=1;
               break;
            }
         }
         if(flag)
         {
            printf("目录扫描完毕\n");
            break;           
         } 
         parentnode = findnode(filePath,headnode);
      }
      else
      {
         printf("目录扫描完毕\n");
         break;
      }
   }
    printf("目录树生成成功！\n");
    FindClose(hFind);//关闭句柄  
    *Directory = dirnum;
    *Num = filenum;
    *Treedepth=getTreeDepth(headnode);
    return headnode;  
}  
//该函数用于从指定文件中读取目录信息在目录树中统计统计文件信息。最早时间、最晚时间，文件总数，总的文件大小（不包含子目录）
int GetFileInfo(Treeptr head)
{
    char maxname[300];
    Treeptr Pnode,Qnode;
    FILE* file = fopen("mystat.txt","r");
    if(file==NULL)
    {
        printf("mystat.txt文件无法读取！\n");
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"stat dirs\n")!=0)
    {
        printf("读取文件错误！\n");
        return 0;
    }
    while(fscanf(file,"%s",maxname)==1)
    {
        if(strcmp(maxname,"end")==0)
        {
            break;
        }
        Pnode = findnode(maxname,head);
        if(Pnode==NULL)
        {
            printf("%s不存在！\n\n",maxname);
            continue;
        }
        if(Pnode->flag==1)
        {
            Pnode->Information = (DirInfoptr)malloc(sizeof(Dnode));
            Qnode = Pnode->kid;
            Pnode->Information->originFirstFile = NULL;
            Pnode->Information->originLatestFile = NULL;
            Pnode->Information->originnumber = 0;
            Pnode->Information->originsize = 0;
            if(Qnode==NULL)
            {
                printf("%s该目录为空！\n\n",maxname);
                continue;
            }
            else
            {   
                while(Qnode!=NULL)
                {
                    if(Qnode->flag==2)
                    {
                        (Pnode->Information->originnumber)++;
                        (Pnode->Information->originsize)+=(Qnode->size);
                        if((Pnode->Information->originFirstFile==NULL)||(Pnode->Information->originLatestFile==NULL))
                        {
                            Pnode->Information->originFirstFile = Pnode->Information->originLatestFile = Qnode;
                        }
                        else
                        {
                            int a = CompareFileTime(&(Pnode->Information->originFirstFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a>0)
                            {
                                Pnode->Information->originFirstFile = Qnode;
                            }
                            a = CompareFileTime(&(Pnode->Information->originLatestFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a<0)
                            {
                                Pnode->Information->originLatestFile = Qnode;
                            }
                        }
                    }
                    Qnode = Qnode->brother;
                }
                if(Pnode->Information->originnumber==0)
                {
                    printf("%s该目录内无文件！\n\n",maxname);
                    continue;
                }
                else
                {
                    printf("%s该目录中文件总数为%ld,总大小为%ld字节\n",maxname,Pnode->Information->originnumber,Pnode->Information->originsize);
                    printf("其中最早时间文件为%s  ",Pnode->Information->originFirstFile->maxname);
                    printf("大小为%ld字节  ",Pnode->Information->originFirstFile->size);
                    printf(" 时间为%s\n",Pnode->Information->originFirstFile->time);
                    printf("其中最晚时间文件为%s  ",Pnode->Information->originLatestFile->maxname);
                    printf("大小为%ld字节  ",Pnode->Information->originLatestFile->size);
                    printf(" 时间为%s\n\n\n",Pnode->Information->originLatestFile->time);
                }
            }
        }
        else
        {
            printf("%s该文件大小为%ld字节, 时间为%s\n\n\n",maxname,Pnode->size,Pnode->time);
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
    return 1;
}
//该函数用于从指定文件中读取文件操作并在目录树上进行模拟文件操作
int SimFileOper(Treeptr head)
{

}
//该函数用于从指定文件中读取目录操作并在目录树上进行模拟目录操作
int SimDirOper(Treeptr head)
{
    char maxname[300];
    Treeptr Pnode,Qnode;
    const char delimiter[]=",";
    char* str1;
    FILE* file = fopen("mydir.txt","r");
    if(file==NULL)
    {
        printf("mydir.txt文件无法读取！\n");
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"selected dirs\n")!=0)
    {
        printf("读取文件错误！\n");
        return 0;
    }
    while(fscanf(file,"%s",maxname)==1)
    {
        if(strcmp(maxname,"end")==0)
        {
            break;
        }
        printf("%s\n\n",maxname);
        str1 = strtok(maxname,delimiter);
        printf("%s\n\n",str1);
      //  Pnode = findnode(str1,head);
    }
    return 1;
}
//该函数用于读取指定文件中的目录信息并检查发生了哪些变化
int CheckInfoChange(Treeptr head)
{
    char maxname[300];
    int number=0;
    Treeptr Pnode,Qnode;
    FILE* file = fopen("mystat.txt","r");
    if(file==NULL)
    {
        printf("mystat.txt文件无法读取！\n");
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"stat dirs\n")!=0)
    {
        printf("读取文件错误！\n");
        return 0;
    }
    while(fscanf(file,"%s",maxname)==1)
    {
        if(strcmp(maxname,"end")==0)
        {
            break;
        }
        Pnode = findnode(maxname,head);
        if(Pnode==NULL)
        {
            number += CheckDiff(NULL,maxname);
            continue;
        }
        if(Pnode->flag==1)
        {
            Qnode = Pnode->kid;
            Pnode->Information->nowFirstFile = NULL;
            Pnode->Information->nowLatestFile = NULL;
            Pnode->Information->nownumber = 0;
            Pnode->Information->nowsize = 0;
            if(Qnode==NULL)
            {
                number += CheckDiff(Pnode->Information,maxname);
                continue;
            }
            else
            {   
                while(Qnode!=NULL)
                {
                    if(Qnode->flag==2)
                    {
                        (Pnode->Information->nownumber)++;
                        (Pnode->Information->nowsize)+=(Qnode->size);
                        if((Pnode->Information->nowFirstFile==NULL)||(Pnode->Information->nowLatestFile==NULL))
                        {
                            Pnode->Information->nowFirstFile = Pnode->Information->nowLatestFile = Qnode;
                        }
                        else
                        {
                            int a = CompareFileTime(&(Pnode->Information->nowFirstFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a>0)
                            {
                                Pnode->Information->nowFirstFile = Qnode;
                            }
                            a = CompareFileTime(&(Pnode->Information->nowLatestFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a<0)
                            {
                                Pnode->Information->nowLatestFile = Qnode;
                            }
                        }
                    }
                    Qnode = Qnode->brother;
                }
                number += CheckDiff(Pnode->Information,maxname);
            }
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
    printf("\n\n   共有%d条目录信息发生变化\n\n",number);
    return 1;
}
//该函数用于在目录树中查询指定文件或者目录信息
int FindInfo(Treeptr head)
{
    char maxname[300];
    printf("请输入要查询的文件或目录路径：\n");
    scanf("%s",maxname);
    Treeptr Pnode,Qnode;
    Pnode = findnode(maxname,head);
    if(Pnode==NULL)
    {
        printf("该目录或文件不存在！\n\n\n");
        return 0;
    }
    if(Pnode->flag==1)
    {
        Pnode->Information = (DirInfoptr)malloc(sizeof(Dnode));
        Qnode = Pnode->kid;
        Pnode->Information->nowFirstFile = NULL;
        Pnode->Information->nowLatestFile = NULL;
        Pnode->Information->nownumber = 0;
        Pnode->Information->nowsize = 0;
        if(Qnode==NULL)
        {
            printf("该目录为空！\n");
            return 1;
        }
        else
        {   
            while(Qnode!=NULL)
            {
                if(Qnode->flag==2)
                {
                    (Pnode->Information->nownumber)++;
                    (Pnode->Information->nowsize)+=(Qnode->size);
                    if((Pnode->Information->nowFirstFile==NULL)||(Pnode->Information->nowLatestFile==NULL))
                    {
                        Pnode->Information->nowFirstFile = Pnode->Information->nowLatestFile = Qnode;
                    }
                    else
                    {
                        int a = CompareFileTime(&(Pnode->Information->nowFirstFile->fltwritetime),&(Qnode->fltwritetime));
                        if(a>0)
                        {
                            Pnode->Information->nowFirstFile = Qnode;
                        }
                        a = CompareFileTime(&(Pnode->Information->nowLatestFile->fltwritetime),&(Qnode->fltwritetime));
                        if(a<0)
                        {
                            Pnode->Information->nowLatestFile = Qnode;
                        }
                    }
                }
                Qnode = Qnode->brother;
            }
            if(Pnode->Information->nownumber==0)
            {
                printf("该目录内无文件！\n");
                return 1;
            }
            else
            {
                printf("该目录中文件总数为%ld,总大小为%ld字节\n",Pnode->Information->nownumber,Pnode->Information->nowsize);
                printf("其中最早时间文件为%s  ",Pnode->Information->nowFirstFile->maxname);
                printf("大小为%ld字节  ",Pnode->Information->nowFirstFile->size);
                printf(" 时间为%s\n",Pnode->Information->nowFirstFile->time);
                printf("其中最晚时间文件为%s  ",Pnode->Information->nowLatestFile->maxname);
                printf("大小为%ld字节  ",Pnode->Information->nowLatestFile->size);
                printf(" 时间为%s\n\n\n",Pnode->Information->nowLatestFile->time);
            }
        }
    }
    else
    {
        printf("该文件大小为%ld字节, 时间为%s\n\n\n",Pnode->size,Pnode->time);
    }
    return 1;
}
//该函数用于释放目录树所占的内存来防止内存泄漏
int ClearTree(Treeptr head)
{
    LinkQueue TREEqueue= InitQueue();
    Treeptr Pnode;
    Treeptr Tnode;
    int nowdepth=1;
    Pnode = head->kid;
    while(1)
    {
        while(Pnode!=NULL)
        {
            if(Pnode->flag==1)
            {
                nowdepth++;
                T_EnQueue(&TREEqueue,Pnode,nowdepth);
                Pnode = Pnode->brother;
            }
            else
            {
                nowdepth++;
                Tnode = Pnode;
                Pnode = Pnode->brother;
                free(Tnode);
            }
        }
        if(T_DelQueue(&TREEqueue,&Pnode,&nowdepth)==0)
        {
            printf("目录树内存空间释放完毕!\n");
            break;
        }
        else
        {
            Tnode = Pnode;
            Pnode = Pnode->kid;
            free(Tnode);
        }
    }
    free(head);
    return 1;
}
