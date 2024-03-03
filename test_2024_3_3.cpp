#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "test.h"
#include<stdio.h>
#include<string.h>
#include<windows.h>
#include<qfiledialog.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->document()->setMaximumBlockCount(0);
    ui->verticalScrollBar->setRange(0, ui->textBrowser->document()->size().height());
    connect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &MainWindow::on_verticalScrollBar_valueChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_scan_directory_triggered()
{
    QWidget window;
    window.setWindowTitle("Scan Disk");

    QFileDialog dialog(&window);
    dialog.setFileMode(QFileDialog::Directory);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 获取所选目录的路径
            QString directory = files.first();

            // 扫描目录
            QDir dir(directory);
            dir.setFilter(QDir::Dirs | QDir::Files);
            QStringList entries = dir.entryList();

            // 显示信息到屏幕上
            QTextStream out(stdout);
            foreach (QString entry, entries) {
                out << entry << Qt::endl;
            }
        }
    }

}


void MainWindow::on_test_triggered()
{
    QWidget window;
    window.setWindowTitle("Select Directory");
    PVOID oldvalue = NULL;
    QFileDialog dialog(&window);
    dialog.setFileMode(QFileDialog::Directory);
    Wow64DisableWow64FsRedirection(&oldvalue);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 获取所选目录的路径
            QString directory = files.first();
            directory.replace("/","\\");
            char filePath[520];
            std::string stdstr = directory.toStdString();
            strcpy_s(filePath,stdstr.c_str());
            //.....................................
            char szFind[520];       //定义一个要找的文件路径的量
            WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息
            HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值
            int maxpath=0;                 //存放最长路径长度
            int nowpath=0;                //存放当前路径长度
            char maxfilepath[520];      //存放最长路径文件名称
            int maxdepth=1;             //存放最深目录层数
            int nowdepth=1;             //存放当前目录层数
            LinkQueue dirqueue=InitQueue(); //得到初始目录队列
            long number=0;                  //存放目录和文件总数
            long filenum=0;                 //存放文件数量
            long dirnum=0;                  //存放目录数量
            long Size=0;

            strcpy_s(szFind,filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间
            strcat_s(szFind,"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所有文件，包括目录
            wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
            mbstowcs(wfilename, szFind, strlen(szFind) + 1);
            hFind=FindFirstFile(wfilename,&FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件

            //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去

            if(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
            {
                DWORD dwLastError = GetLastError();
                // 根据错误代码判断原因
                switch (dwLastError) {
                case ERROR_FILE_NOT_FOUND:
                    ui->textBrowser->append("文件或目录不存在\n");
                    break;
                case ERROR_PATH_NOT_FOUND:
                    ui->textBrowser->append("路径不存在\n");
                    break;
                case ERROR_ACCESS_DENIED:
                    ui->textBrowser->append("拒绝访问\n");
                    break;
                case ERROR_INVALID_PARAMETER:
                    ui->textBrowser->append("参数无效\n");
                    break;
                default:
                    ui->textBrowser->append("未知错误\n");
                }
                ui->textBrowser->append(szFind);
                ui->textBrowser->append("该目录不存在！\n");
                setcursor();
                return;
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
                            dirnum++;
                            number++;
                            strcpy(szFind,filePath);
                            strcat(szFind,"\\");
                            strcat(szFind,(const char*)FindFileData.cFileName);
                            EnQueue(&dirqueue,szFind,nowdepth+1);
                        }
                    }
                    else
                    {
                        filenum++;
                        number++;
                        Size += (long)(FindFileData.nFileSizeLow/1024);            //对文件的大小进行统计  FindFileData.nFileSizeLow是字节大小
                        //printf("%ld  %s\\%s\n%d字节\n",filenum,filePath,FindFileData.cFileName,FindFileData.nFileSizeLow);//输出目录下的文件的路径和名称和大小
                        nowpath=strlen(filePath)+strlen((const char*)FindFileData.cFileName);
                        if(nowpath>maxpath)
                        {
                            maxpath=nowpath;
                            strcpy(maxfilepath,filePath);
                            strcat(maxfilepath,"\\");
                            strcat(maxfilepath,(const char*)FindFileData.cFileName);
                        }
                    }
                    if(!FindNextFile(hFind,&FindFileData))                        //继续查找FindFirstFile函数搜索后的文件    非零表示成功，零表示失败
                    {
                        break;
                    }                                                  //因此为0时，执行break
                    else
                    {
                        //printf("访问%s\n",FindFileData.cFileName);
                    }
                }

                if(DelQueue(&dirqueue,szFind,&nowdepth))
                {
                    int flag=0;
                    strcpy(filePath,szFind);
                    strcat(szFind,"\\*.*");
                    hFind=FindFirstFile((LPCWSTR)szFind,&FindFileData);
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
                            strcat(szFind,"\\*");
                            hFind=FindFirstFile((LPCWSTR)szFind,&FindFileData);
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
                        ui->textBrowser->append("扫描完毕!\n");
                        setcursor();
                        break;
                    }
                }
                else
                {
                    ui->textBrowser->append("扫描完毕!\n");
                    setcursor();
                    break;
                }
            }
            FindClose(hFind);//关闭句柄
            Wow64RevertWow64FsRedirection(oldvalue);
            //printf("最长带全路径文件为%s\n长度为%d\n目录层数为%d\n",maxfilepath,maxpath,maxdepth);
            ui->textBrowser->append("最长带全路径文件为");
            ui->textBrowser->append(maxfilepath);
            ui->textBrowser->append("\n长度为");
            ui->textBrowser->append(QString::number(maxpath));
            ui->textBrowser->append("\n目录层数为");
            ui->textBrowser->append(QString::number(maxdepth));
            ui->textBrowser->append("\n");
            //printf("文件夹和文件数量总和为：%ld\n文件夹数量为%ld\n文件数量为%ld\n",number,dirnum,filenum);

            //......................................
            setcursor();
        }
    }
}



void MainWindow::on_verticalScrollBar_valueChanged(int value)
{
    ui->textBrowser->verticalScrollBar()->setValue(value);
}

void MainWindow::setcursor()
{
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);
}
