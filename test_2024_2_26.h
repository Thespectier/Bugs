#include<windows.h>
#include<stdio.h>

typedef struct Tnode{
	char maxname[300];      //带全路径名称
    char name[300];         //单文件or目录名称
	int flag;               //flag==1为目录，flag==2为文件
    char time[128];         //修改时间
	long size;               //大小，单位为字节
	struct Tnode *kid;        //孩子结点指针
	struct Tnode *brother;    //兄弟结点指针
}Tnode,*Treeptr;               //目录树结点

typedef struct Qnode{
	char dirmaxname[520];
	int depth;
	struct Qnode *next;
	Treeptr tnode;
} Qnode, *QueuePtr;  // �ڵ�����

typedef struct {
	Qnode *front;
	Qnode *rear;
} LinkQueue;       // ��ʽ���нڵ�

// ���ɶ���
#define  LENG sizeof(Qnode)
LinkQueue InitQueue(){
	LinkQueue Q;
	Q.front = Q.rear = (QueuePtr)malloc(LENG);
	Q.front->next = NULL;
	return Q;
} 

// ����Ԫ��
int EnQueue(LinkQueue *Q, char* szFind,int nowdepth){
	Qnode *p;
	p = (Qnode *)malloc(LENG);
	if (!p) {
		printf("ERROR");
		return 0;
	}
    strcpy(p->dirmaxname,szFind);
    p->depth = nowdepth;
	p->next = NULL;
	(*Q).rear->next =  p;
	(*Q).rear = p;
	return 1;
};

// ɾ��Ԫ��
int DelQueue(LinkQueue *Q, char* szFind,int* nowdepth){
	Qnode *p;
	if ((*Q).front==(*Q).rear){
		printf ("Empty queqe");
		return 0;
	}
	p = (*Q).front->next;
    strcpy(szFind,p->dirmaxname);
    *nowdepth = p->depth;
	(*Q).front->next  = (*Q).front->next->next ;
	if ((*Q).rear==p){
		(*Q).rear = (*Q).front;
	}
	free(p);
	return 1;
} 

int T_EnQueue(LinkQueue *Q,Treeptr tnode ,int nowdepth){
	Qnode *p;
	p = (Qnode *)malloc(LENG);
	if (!p) {
		printf("ERROR");
		return 0;
	}
    p->tnode = tnode;
    p->depth = nowdepth;
	p->next = NULL;
	(*Q).rear->next =  p;
	(*Q).rear = p;
	return 1;
};

int T_DelQueue(LinkQueue *Q,Treeptr* tnode,int* nowdepth){
	Qnode *p;
	if ((*Q).front==(*Q).rear){
		printf ("Empty queqe");
		return 0;
	}
	p = (*Q).front->next;
    *tnode = p->tnode;
    *nowdepth = p->depth;
	(*Q).front->next  = (*Q).front->next->next ;
	if ((*Q).rear==p){
		(*Q).rear = (*Q).front;
	}
	free(p);
	return 1;
} 

//给定带全路径名在目录树中找到结点
Treeptr findnode(char* maxname,Treeptr head)
{
    char Tname[300];
    char Pname[300];
    char* str1;
    char* str2;
    int num1=0;
    int num2=0;
    const char delimiter[2]="\\";
    Treeptr Pnode;
    strcpy(Tname,maxname);
    strcpy(Pname,head->maxname);
    str2 = strtok(Pname,delimiter);
    num2++;
    while(str2!=NULL)
    {
        num2++;
        str2 = strtok(NULL,delimiter);
    }
    str1 = strtok(Tname,delimiter);
    num1++;
    while(num1!=num2)
    {
        num1++;
        str1 = strtok(NULL,delimiter);
    }
    if(str1==NULL)
    {
        return head;
    }

    Pnode = head;
    while(str1!=NULL)
    {
        Pnode = Pnode->kid;
        while(strcmp(Pnode->name,str1)!=0)
        {
            Pnode = Pnode->brother;
            if(Pnode==NULL)
            {
                printf("该目录or文件不存在！\n");
                break;
            }
        }
        str1 = strtok(NULL,delimiter);
    }
    return Pnode;

}
//得到格式化修改时间
int getrighttime(FILETIME ftLastWriteTime,char* timestring)
{
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&ftLastWriteTime, &systemTime);
    wsprintf(timestring, TEXT("%04d-%02d-%02d %02d:%02d:%02d"), 
        systemTime.wYear, systemTime.wMonth, systemTime.wDay,
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    return 1;
}
//生成目录树目录结点
Treeptr createDIRnode(char* Dmaxname,char* Dname,FILETIME ftlastWriteTime,Treeptr parent)
{
    Treeptr Dnode;
    Treeptr Pnode;
    Dnode = (Treeptr)malloc(sizeof(Tnode));
    strcpy(Dnode->maxname,Dmaxname);
    strcpy(Dnode->name,Dname);
    getrighttime(ftlastWriteTime,Dnode->time);
    Dnode->flag = 1;
    Dnode->brother = NULL;
    Dnode->kid = NULL;
    if(parent->kid==NULL)
    {
        parent->kid = Dnode;
    }
    else
    {
        Pnode = parent->kid;
        while(Pnode->brother!=NULL)
        {
            Pnode = Pnode->brother;
        }
        Pnode->brother = Dnode;
    }
    return Dnode;
}
//生成目录树文件结点
Treeptr createFILEnode(char* Fmaxname,WIN32_FIND_DATA* FindFileDate,Treeptr parent)
{
    Treeptr Fnode;
    Treeptr Pnode;
    Fnode = (Treeptr)malloc(sizeof(Tnode));
    strcpy(Fnode->maxname,Fmaxname);
    strcpy(Fnode->name,FindFileDate->cFileName);
    getrighttime(FindFileDate->ftLastWriteTime,Fnode->time);
    Fnode->flag = 2;
    Fnode->brother = NULL;
    Fnode->kid = NULL;
    if(parent->kid==NULL)
    {
        parent->kid = Fnode;
    }
    else
    {
        Pnode = parent->kid;
        while(Pnode->brother!=NULL)
        {
            Pnode = Pnode->brother;
        }
        Pnode->brother = Fnode;
    }
    return Fnode;
}
//遍历计算目录树深度
int getTreeDepth(Treeptr head)
{
    LinkQueue TREEqueue= InitQueue();
    Treeptr Pnode;
    Treeptr Tnode;
    int nowdepth=1;
    int maxdepth=1;
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
                Pnode = Pnode->brother;
            }
            if(nowdepth>maxdepth)
            {
                maxdepth = nowdepth;
            }
        }
        if(T_DelQueue(&TREEqueue,&Pnode,&nowdepth)==0)
        {
            printf("目录树遍历完毕，目录树深度为%d\n",maxdepth);
            break;
        }
        else
        {
            Pnode = Pnode->kid;
        }
    }
    return maxdepth;
}