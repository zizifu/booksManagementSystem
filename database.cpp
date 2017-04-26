#include "database.h"
#include <QWidget>
#include <QLineEdit>
#include <QLayout>
#include <QComboBox>
#include <QLabel>
#include <QFrame>
#include <QGroupBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QPalette>
#include <QBrush>
#include <QPixmap>
#include <QDateTime>
#include <QtSql>
#include <QStringList>
#include <QPluginLoader>
#include <QMessageBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QTimer>
#include <QDir>
#include <QFileDialog>


/* 构造函数 */
database::database(QObject *parent)
    : QObject(parent)
{
    //初始化变量
    user_num = 0;
    user_type = VISITOR;
    current_user = "游客";

    //新建计时器
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start(1000);

    //初始化指针
    tablewidget = NULL;
    tablewidget2 = NULL;
    nextbutton = NULL;
    lastbutton = NULL;
    hboxLayout6 = NULL;
    lastbutton = NULL;
    nextbutton = NULL;

    deleteUserButton = NULL;
    grantLentButton = NULL;
    lendBookButton = NULL;
    returnBookButton = NULL;

    window4 = NULL;

    for(int i=0;i<10;i++){
        item2[i] = NULL;
        item3[i] = NULL;
    }

    //窗口布局
    setLayout();

    //连接数据库
    createConnection();
}

/* 析构函数 */
database::~database()
{
    //取消与数据库的连接
    db.removeDatabase("book_manage");
}

/*  _______________________
   |                       |
   |   第一部分：页面布局     |
   |                       |
    ———————————————————————
*/

/* 初始化布局 */
void database::setLayout()
{
    //显示标题栏
    setWindowTitle();

    //以游客身份新建搜索窗口
    setWindowSearchBook(VISITOR);
}

/* 标题窗口布局*/
void database::setWindowTitle()
{
    //申请内存
    QPalette palette;
    loginbutton = new QPushButton(tr("登录"));
    registerbutton = new QPushButton(tr("注册"));
    spacelabel2 = new QLabel();
    spacelabel3 = new QLabel("\n\n");
    vboxLayout0 = new QVBoxLayout();
    hboxLayout2 = new QHBoxLayout();
    hboxLayout3 = new QHBoxLayout();
    titleImage = new QWidget();
    window = new QWidget();
    vboxLayout3 = new QVBoxLayout();
    mainTabWidget = new QTabWidget();

    //登录注册按钮状态设置
    registerbutton->setFlat(true);
    registerbutton->setFixedSize(QSize(60,30));
    loginbutton->setFlat(true);
    loginbutton->setFixedSize(QSize(60,30));

    //建立信号与槽
    connect(registerbutton,SIGNAL(clicked()),this,SLOT(setWindowRegister()));
    //点击注册按钮，显示注册窗口
    connect(loginbutton,SIGNAL(clicked()),this,SLOT(setWindowLogin()));
    //点击登录按钮，显示登录窗口

    //设置标题图片
    QDir d;
    titleImage->setAutoFillBackground(true);
    palette.setBrush(QPalette::Background,QBrush(QPixmap(d.currentPath()+"/title.jpg")));
    titleImage->setPalette(palette);
    titleImage->setLayout(hboxLayout2);

    //标题栏布局
    hboxLayout3->addWidget(spacelabel2);
    hboxLayout3->addWidget(loginbutton);
    hboxLayout3->addWidget(registerbutton);

    vboxLayout3->addWidget(spacelabel3);
    vboxLayout3->addLayout(hboxLayout3);

    hboxLayout2->addStretch();
    hboxLayout2->addLayout(vboxLayout3);

    //将标题图片和标签窗口加入总布局
    vboxLayout0->addWidget(titleImage);
    vboxLayout0->addWidget(mainTabWidget);

    //总窗口显示
    window->setAutoFillBackground(true);
    window->setLayout(vboxLayout0);
    window->setWindowTitle(tr("图书管理系统"));
    window->setFixedSize(1024*1.2,768*1.2);
    window->show();
}

/* 登录窗口布局 */
void database::setWindowLogin()
{
    //登录时禁用登录和注册按钮
    loginbutton->setDisabled(true);
    registerbutton->setDisabled(true);

    //新建用户和管理员登录窗口
    user_loginWidget = createLoginWindow(1);
    manager_loginWidget = createLoginWindow(2);

    //加入两个登录窗口加入分栏布局，并显示
    loginTabWidget = new QTabWidget();
    loginTabWidget->addTab(user_loginWidget,"用户登录");
    loginTabWidget->addTab(manager_loginWidget,"管理员登录");

    //禁用关闭按钮
    loginTabWidget->setWindowFlags(loginTabWidget->windowFlags()&~Qt::WindowCloseButtonHint);

    loginTabWidget->show();

}

/* 搜索书籍界面布局 */
void database::setWindowSearchBook(int type)
{
    //如果身份为用户，需要销毁之前的窗口新建
    if(type==USER_LOGIN||type==USER_REGISTER){
        delete window1;
    }

    //常量
    const int num = 8;
    const int columnNum = 3;
    const int typenum =23;

    //申请内存
    window1 = new QWidget();
    gridLayout = new QGridLayout();
    vboxLayout1 = new QVBoxLayout();
    vboxLayout = new QVBoxLayout();

    hboxLayout1 = new QHBoxLayout();
    hboxLayout = new QHBoxLayout();

    groupbox = new QGroupBox();
    groupbox1 = new QGroupBox();
    clearbutton = new QPushButton(tr("清空"));
    searchbutton = new QPushButton(tr("搜索"));
    tablewidget = new QTableWidget();

    tree = new QTreeWidget();
    root = new QTreeWidgetItem(QStringList()<<"所有类型");
    QTreeWidgetItem *leaf[typenum];

    //建立信号与槽
    connect(searchbutton,SIGNAL(clicked()),this,SLOT(searchBook()));
    //点击搜索按钮，进行搜索操作
    connect(clearbutton,SIGNAL(clicked()),this,SLOT(clear()));
    //点击清除按钮，进行清除操作
    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(searchBookByType(QTreeWidgetItem*,int)));
    //点击树状列表，进行查找书籍操作

    //更新窗口信息
    updateTitle(type);

    //设置左边的树状图书分类栏
    tree->setHeaderLabels(QStringList()<<"图书类型");
    tree->addTopLevelItem(root);
    tree->setFixedWidth(280);

    tree->setColumnWidth(1,10);

    QString str2[] = {
        "马列主义毛邓思想","哲学","社会科学总论","政治法律","军事","经济","文化科学体育教育","语言文字",
        "文学","艺术","历史地理","自然科学总论","数理科学与化学","天文学与地理科学","生物科学","医药卫生",
        "工业技术","交通运输","航空航天","环境科学"
    };
    for(int i=0;i<20;i++){
        leaf[i] = new QTreeWidgetItem(QStringList()<<str2[i]);
        root->addChild(leaf[i]);
    }

    tree->expandAll();

    //设置高级搜索栏
    window = new QWidget();
    nameLine = new QLineEdit();
    publishLine = new QLineEdit();
    authorLine = new QLineEdit();
    dateBox = new QComboBox();
    dateBox1 = new QComboBox();
    priceLine = new QLineEdit();
    priceLine1 = new QLineEdit();
    isLent = new QComboBox();

    label = new QLabel[num];
    QString str[] = {"书名","出版社","作者","年份"," --","状态","价位"," --"};

    for(int i=0;i<num;i++){
        label[i].setText(str[i]);
        gridLayout->addWidget(label+i,i/columnNum+1,2*(i%columnNum)+1);
    }

    dateBox->addItem("    ");
    dateBox1->addItem("    ");
    for(int i=0;i<60;i++){
        dateBox->addItem(QString::number(2016-i));
        dateBox1->addItem(QString::number(2016-i));
    }

    isLent->addItem("    ");
    isLent->addItem("已借出");
    isLent->addItem("未借出");

    //表格布局，放置搜索选项
    gridLayout->addWidget(nameLine,1,2);       //书名
    gridLayout->addWidget(publishLine,1,4);       //类型
    gridLayout->addWidget(authorLine,1,6);     //作者名称
    gridLayout->addWidget(dateBox,2,2);        //年份（起）
    gridLayout->addWidget(dateBox1,2,4);       //年份（终）
    gridLayout->addWidget(isLent,2,6);        //借阅状态
    gridLayout->addWidget(priceLine,3,2);     //价格（从）
    gridLayout->addWidget(priceLine1,3,4);     //价格（到）

    //搜索选项加入群组中
    groupbox->setLayout(gridLayout);
    groupbox->setFixedSize(600,140);

    //垂直布局，放置两个按钮
    vboxLayout1->addStretch();
    vboxLayout1->addWidget(searchbutton);
    vboxLayout1->addWidget(clearbutton);
    vboxLayout1->addStretch();

    groupbox1->setLayout(vboxLayout1);
    groupbox1->setFixedSize(200,140);

    //水平布局，依次加入搜索选项和按钮的垂直布局
    hboxLayout1->addWidget(groupbox);
    hboxLayout1->setSpacing(30);
    hboxLayout1->addWidget(groupbox1);

    //垂直布局，依次加入搜索框和显示框
    vboxLayout->addLayout(hboxLayout1);
    vboxLayout->addWidget(tablewidget);
    setWindowShowBook(type);
    vboxLayout->addLayout(hboxLayout6);

    //水平布局，加入左边栏和右窗口
    hboxLayout->addWidget(tree);
    hboxLayout->addLayout(vboxLayout);

    //窗口1加入水平布局
    window1->setLayout(hboxLayout);
    mainTabWidget->addTab(window1,"图书搜索");

    //如果对象为用户，那么新建借书窗口
    if(type==USER_LOGIN||type==USER_REGISTER){
        setWindowLoanBook();
    }
}

/* 单册添加书籍界面布局 */
void database::setWindowAddBook()
{
    //申请内存
    QLabel *label3 = new QLabel;
    label3->setText("(如果有多个作者，请用空格分开)");

    addBookWidget = new QWidget();
    deletebookWidget = new QWidget();
    window3 = new QWidget();
    vboxLayout4 = new QVBoxLayout();
    vboxLayout5 = new QVBoxLayout();
    vboxLayout6 = new QVBoxLayout();
    vboxLayout9 = new QVBoxLayout();
    hboxLayout4 = new QHBoxLayout();
    hboxLayout5 = new QHBoxLayout();
    hboxLayout9 = new QHBoxLayout();
    deleteOkButton = new QPushButton("确定");

    manageTabWidget = new QTabWidget();
    gridLayout2 = new QGridLayout();
    okbutton = new QPushButton(tr("确定"));

    nameLine2 = new QLineEdit();
    typeBox2 = new QComboBox();
    priceLine2 = new QLineEdit();
    authorLine2 = new QLineEdit();
    numLine2 = new QLineEdit();
    publishLine2 = new QLineEdit();
    dateBox2 = new QComboBox();

    idLine = new QLineEdit();
    label2 = new QLabel[7];
    label5 = new QLabel;
    label5->setText("请输入待删除书目的编号");

    //建立信号与槽
    connect(deleteOkButton,SIGNAL(clicked()),this,SLOT(deleteBook()));
    //点击确认删除按钮，进行删除书籍操作
    connect(okbutton,SIGNAL(clicked()),this,SLOT(oneAddBook()));
    //点击确认按钮，进行添加书本操作

    //设置选项框信息
    QString str[7] = {"书名","价格","数量","出版社","出版日期",
                      "分类","作者"};
    QString str2[20] = {
        "马列主义毛邓思想","哲学","社会科学总论","政治法律","军事","经济","文化科学体育教育","语言文字",
        "文学","艺术","历史地理","自然科学总论","数理科学与化学","天文学与地理科学","生物科学","医药卫生",
        "工业技术","交通运输","航空航天","环境科学"
    };

    for(int i = 0;i < 7;i++){
        label2[i].setText(str[i]);
    }

    for(int i=0;i<20;i++){
        typeBox2->addItem(str2[i]);
    }

    for(int i=0;i<60;i++){
        dateBox2->addItem(QString::number(2016-i));
    }

    //窗口布局

    //设置选项框表格
    gridLayout2->addWidget(label2,1,1);
    gridLayout2->addWidget(label2+1,1,3);
    gridLayout2->addWidget(label2+2,2,1);
    gridLayout2->addWidget(label2+3,2,3);
    gridLayout2->addWidget(label2+4,3,1);
    gridLayout2->addWidget(label2+5,3,3);
    gridLayout2->addWidget(label2+6,4,1);

    gridLayout2->addWidget(nameLine2,1,2);
    gridLayout2->addWidget(priceLine2,1,4);
    gridLayout2->addWidget(numLine2,2,2);
    gridLayout2->addWidget(publishLine2,2,4);
    gridLayout2->addWidget(dateBox2,3,2);
    gridLayout2->addWidget(typeBox2,3,4);
    gridLayout2->addWidget(authorLine2,4,2);
    gridLayout2->addWidget(label3,4,3,1,2);

    //加入确认按钮
    hboxLayout5->addStretch();
    hboxLayout5->addWidget(okbutton);
    hboxLayout5->addStretch();

    vboxLayout6->addLayout(gridLayout2);
    vboxLayout6->addLayout(hboxLayout5);

    addBookWidget->setLayout(vboxLayout6);

    hboxLayout9->addStretch();
    hboxLayout9->addWidget(deleteOkButton);
    hboxLayout9->addStretch();

    vboxLayout9->addStretch();
    vboxLayout9->addWidget(label5);
    vboxLayout9->addWidget(idLine);
    vboxLayout9->addLayout(hboxLayout9);
    vboxLayout9->addStretch();

    //添加分页窗口
    deletebookWidget->setLayout(vboxLayout9);
    manageTabWidget->addTab(addBookWidget,"添加图书");
    manageTabWidget->addTab(deletebookWidget,"删除书籍");
    vboxLayout5->addSpacing(100);
    vboxLayout5->addWidget(manageTabWidget);
    vboxLayout5->addSpacing(300);

    hboxLayout4->addStretch();
    hboxLayout4->addLayout(vboxLayout5);
    hboxLayout4->addStretch();

    window3->setLayout(hboxLayout4);
}

/* 多册添加书籍界面布局 */
void database::setWindowMutiAddBook()
{
    //申请内存
    tipLabel = new QLabel();
    window5 = new QWidget();
    tablewidget3 = new QTableWidget();
    vboxLayout12 = new QVBoxLayout();
    hboxLayout12 = new QHBoxLayout();
    hboxLayout13 = new QHBoxLayout();
    addBookOkButton = new QPushButton("提交表单");
    addBookClearButton = new QPushButton("清空表单");
    loadFileButton = new QPushButton("加载文件");

    //建立信号与槽
    connect(addBookOkButton,SIGNAL(clicked()),this,SLOT(mutiAddBook()));
    connect(addBookClearButton,SIGNAL(clicked()),this,SLOT(clearAddBookMessage()));
    connect(loadFileButton,SIGNAL(clicked()),this,SLOT(openFile()));

    //设置选项信息
    tipLabel->setText("请双击表格进行内容填写,或者通过文本加载。"
                      "如果有多个作者，请用空格分开\n"
                      "文本格式：按表格属性填写，同样属性按空格分开。"
                      "输入下本书籍信息要换行区分");
    tipLabel->setFrameShape (QFrame::Box);
    QStringList header;
    tablewidget3->setRowCount(13);
    tablewidget3->setColumnCount(7);
    header<<"书名"<<"价格"<<"数量"<<"出版社"<<"出版日期"<<"分类"<<"作者";

    //初始化表格信息
    for(int i=0;i<13;i++){
        for(int j=0;j<7;j++){
            if(j==4){
                publishDate[i] = new QComboBox();
                for(int k=0;k<60;k++){
                    publishDate[i]->addItem(QString::number(2016-k));
                }
                tablewidget3->setCellWidget(i,j,publishDate[i]);
            }
            else if(j==5){
                QString str[] = {
                    "马列主义毛邓思想","哲学","社会科学总论","政治法律","军事","经济","文化科学体育教育","语言文字",
                    "文学","艺术","历史地理","自然科学总论","数理科学与化学","天文学与地理科学","生物科学","医药卫生",
                    "工业技术","交通运输","航空航天","环境科学"
                };
                bookType[i] = new QComboBox();
                for(int k=0;k<20;k++){
                    bookType[i]->addItem(str[k]);
                }
                tablewidget3->setCellWidget(i,j,bookType[i]);
            }
            else{
                QTableWidgetItem *item = new QTableWidgetItem("");
                tablewidget3->setItem(i,j,item);
            }
        }
    }

    tablewidget3->setHorizontalHeaderLabels(header);

    //加入按钮和表格
    hboxLayout12->addStretch();
    hboxLayout12->addWidget(addBookOkButton);
    hboxLayout12->addWidget(addBookClearButton);

    hboxLayout13->addWidget(tipLabel);
    hboxLayout13->addWidget(loadFileButton);

    vboxLayout12->addLayout(hboxLayout13);
    vboxLayout12->addWidget(tablewidget3);
    vboxLayout12->addLayout(hboxLayout12);

    window5->setLayout(vboxLayout12);
}

/* 借阅书籍界面布局 */
void database::setWindowLoanBook()
{
    //申请内存
    window4 = new QWidget();
    mainTabWidget->addTab(window4,"借阅情况");

    tablewidget2 = new QTableWidget(3,7);
    hboxLayout11 = new QHBoxLayout();
    vboxLayout11 = new QVBoxLayout();
    returnBookButton = new QPushButton("归还书籍");

    //建立信号与槽
    connect(returnBookButton,SIGNAL(clicked()),this,SLOT(returnBook()));
    //点击还书按钮，进行还书操作

    //加入按钮和表格
    hboxLayout11->addStretch();
    hboxLayout11->addWidget(returnBookButton);
    hboxLayout11->addStretch();

    vboxLayout11->addWidget(tablewidget2);
    vboxLayout11->addLayout(hboxLayout11);
    vboxLayout11->addStretch();

    window4->setLayout(vboxLayout11);

    //更新借阅表格
    updateLoanBookWindow();
}

/* 用户管理界面布局 */
void database::setWindowUserManage()
{
    //申请内存
    lastPageButton = new QPushButton("上一页");
    nextPageButton = new QPushButton("下一页");
    deleteUserButton = new QPushButton("删除用户信息");
    grantLentButton = new QPushButton("授予/删除借书权限");
    tablewidget1 = new QTableWidget(13,11);
    window2 = new QWidget();
    hboxLayout10 = new QHBoxLayout();
    vboxLayout10 = new QVBoxLayout();

    //建立信号与槽
    connect(lastPageButton,SIGNAL(clicked()),this,SLOT(userLastPage()));
    //点击上一页按钮，翻到上一页
    connect(nextPageButton,SIGNAL(clicked()),this,SLOT(userNextPage()));
    //点击下一页按钮，翻到下一页
    connect(deleteUserButton,SIGNAL(clicked()),this,SLOT(deleteUser()));
    //点击删除用户按钮，进行删除用户操作
    connect(grantLentButton,SIGNAL(clicked()),this,SLOT(grantLent()));
    //点击借书授权按钮，进行借书授权操作

    //载入用户信息，并更新用户管理表格
    loadUserMessage();
    updateManageWindow();

    //加入按钮
    hboxLayout10->addStretch();
    hboxLayout10->addWidget(deleteUserButton);
    hboxLayout10->addWidget(grantLentButton);
    hboxLayout10->addWidget(lastPageButton);
    hboxLayout10->addWidget(nextPageButton);

    //布局
    vboxLayout10->addWidget(tablewidget1);
    vboxLayout10->addLayout(hboxLayout10);
    window2->setLayout(vboxLayout10);
}

/* 用户注册界面布局 */
void database::setWindowRegister()
{
    //申请内存
    registerWindow = new QWidget();
    gridLayout3 = new QGridLayout();

    label4 = new QLabel[3];
    label4[0].setText("用户名");
    label4[1].setText("密码");
    label4[2].setText("密码确认");
    label3 = new QLabel("请输入注册信息(借书权\n"
                        "限将由管理员添加）");
    nameLine3 = new QLineEdit();
    passwordLine1 = new QLineEdit();
    passwordLine2 = new QLineEdit();
    passwordLine1->setEchoMode(QLineEdit::Password);
    passwordLine2->setEchoMode(QLineEdit::Password);
    registerOkButton = new QPushButton("确定");
    hboxLayout7 = new QHBoxLayout();
    hboxLayout8 = new QHBoxLayout();
    vboxLayout7 = new QVBoxLayout();

    //页面布局
    hboxLayout7->addStretch();
    hboxLayout7->addWidget(registerOkButton);
    hboxLayout7->addStretch();

    gridLayout3->addWidget(&label4[0],1,1);
    gridLayout3->addWidget(&label4[1],2,1);
    gridLayout3->addWidget(&label4[2],3,1);

    gridLayout3->addWidget(nameLine3,1,2);
    gridLayout3->addWidget(passwordLine1,2,2);
    gridLayout3->addWidget(passwordLine2,3,2);

    vboxLayout7->addWidget(label3);
    vboxLayout7->addLayout(gridLayout3);
    vboxLayout7->addLayout(hboxLayout7);

    hboxLayout8->addStretch();
    hboxLayout8->addLayout(vboxLayout7);
    hboxLayout8->addStretch();

    registerWindow->setLayout(hboxLayout8);
    registerWindow->resize(400,300);
    registerWindow->show();

    //建立信号与槽
    connect(registerOkButton,SIGNAL(clicked()),this,SLOT(Register()));
    //点击确认登录按钮，进行登录操作
}

/* 显示书籍信息界面布局 */
void database::setWindowShowBook(int type)
{
    //申请内存
    hboxLayout6 = new QHBoxLayout();
    lastbutton = new QPushButton(tr("上一页"));
    nextbutton = new QPushButton(tr("下一页"));

    //建立信号与槽
    connect(lastbutton,SIGNAL(clicked()),this,SLOT(bookLastPage()));
    //点击上一页按钮，翻到上一页
    connect(nextbutton,SIGNAL(clicked()),this,SLOT(bookNextPage()));
    //点击下一页按钮，翻到下一页


    //页面布局
    hboxLayout6->addStretch();
    hboxLayout6->addWidget(lastbutton);
    hboxLayout6->addWidget(nextbutton);

    //如果当前使用者为用户，加入借阅书籍按钮
    if(type==USER_LOGIN||type==USER_REGISTER){
        lendBookButton = new QPushButton(tr("借阅书籍"));
        connect(lendBookButton,SIGNAL(clicked()),this,SLOT(lendBook()));
        hboxLayout6->addWidget(lendBookButton);
    }

    //初始化当前页和最大页
    current_page = 1;
    max_page = 1;

    //更新显示书目窗口
    updateShowBookWindow();
}

/* 更新书籍信息界面 */
void database::updateShowBookWindow()
{
    QStringList header;

    //清除表格信息
    tablewidget->clear();

    //设置表格为不可选择、不可修改
    tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablewidget->setSelectionMode(QAbstractItemView::NoSelection);

    //根据当前使用者的不同使用不同的标题栏
    if(user_type==USER){
        tablewidget->setRowCount(10);
        tablewidget->setColumnCount(11);
        header<<""<<"编号"<<"书名"<<"价格"<<"库存"<<"入库时间"
             <<"入库人员"<<"出版社"<<"出版年份"<<"类型"<<"作者";
    }
    else{
        tablewidget->setRowCount(10);
        tablewidget->setColumnCount(10);
        header<<"编号"<<"书名"<<"价格"<<"库存"<<"入库时间"
             <<"入库人员"<<"出版社"<<"出版年份"<<"类型"<<"作者";
    }

    //加入标题栏
    tablewidget->setHorizontalHeaderLabels(header);

    //设置宽度
    for(int i=0;i<10;i++){
        tablewidget->setColumnWidth(i,120);
    }

    //求出当前页的最多显示条目
    int max;

    if(result.length()==0)return;
    if(current_page==max_page){
        max = result.length()%100/10;
        if(max==0)max=10;
    }
    else max = 10;

    //根据当前使用者的不同在表格上显示不同信息
    if(user_type==USER){
        for(int i=0;i<max;i++){
            item2[i] = new QTableWidgetItem();
            item2[i]->setCheckState(Qt::Unchecked);
            item3[i] = new QTableWidgetItem(result.at(10*i+100*(current_page-1)));
            tablewidget->setItem(i,0,item2[i]);
            tablewidget->setItem(i,1,item3[i]);
            for(int j=1;j<10;j++){
                QTableWidgetItem *item = new QTableWidgetItem(result.at(10*i+j+100*(current_page-1)));
                tablewidget->setItem(i,j + 1,item);
            }
        }
    }
    else{
        for(int i=0;i<max;i++){
            for(int j=0;j<10;j++){
                QTableWidgetItem *item = new QTableWidgetItem(result.at(10*i+j+100*(current_page-1)));
                tablewidget->setItem(i,j,item);
            }
        }
    }
}

/* 更新借阅书籍界面 */
void database::updateLoanBookWindow()
{
    //更新前加载借阅信息
    loadLoanMessage();

    QStringList header;

    //清空表格信息
    tablewidget2->clear();

    //设置标题栏
    header<<""<<"书本编号"<<"书本名称"<<"借阅日期"<<"单册编号"<<"最晚归还日期"<<"剩余归还天数";
    tablewidget2->setHorizontalHeaderLabels(header);

    //设置表格为不可修改，不可选择
    tablewidget2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablewidget2->setSelectionMode(QAbstractItemView::NoSelection);

    //将信息显示到表格上
    for(int i=0;i<loanMessage.length();i++){
        item4[i] = new QTableWidgetItem();
        item4[i]->setCheckState(Qt::Unchecked);
        tablewidget2->setItem(i,0,item4[i]);
        for(int j=0;j<6;j++){
            QTableWidgetItem *item = new QTableWidgetItem(loanMessage.at(i)[j]);
            tablewidget2->setItem(i,j + 1,item);
        }
    }
}

/* 更新用户管理界面 */
void database::updateManageWindow()
{
    //计算当前页面能显示的最多用户数
    int max;
    if(user_current_page==user_max_page){
        max = (user_num%13==0)?13:user_num%13;
    }
    else{
        max = 13;
    }

    //清除表格信息
    tablewidget1->clear();

    //设置表格为不可选择，不可修改
    tablewidget1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablewidget1->setSelectionMode(QAbstractItemView::NoSelection);

    //设置标题栏
    QStringList header;
    header<<""<<"用户名"<<"密码"<<"借书权限"<<"借阅数目"<<"借阅书籍1"
         <<"借阅日期"<<"借阅书籍2"<<"借阅日期"<<"借阅书籍3"<<"借阅日期";
    tablewidget1->setHorizontalHeaderLabels(header);

    //将信息显示到表格上
    for(int i=0;i<max;i++){
        int row = i + (user_current_page - 1)*13;
        item0[i] = new QTableWidgetItem();
        item0[i]->setCheckState(Qt::Unchecked);
        item1[i] = new QTableWidgetItem(userMessage.at(row)[0]);

        tablewidget1->setItem(i,0,item0[i]);
        tablewidget1->setItem(i,1,item1[i]);

        for(int j=1;j<10;j++){
            QTableWidgetItem *item = new QTableWidgetItem(userMessage.at(row)[j]);
            tablewidget1->setItem(i,j+1,item);
        }
    }
}

/* 更新时间显示 */
void database::updateTime()
{
    //获取系统时间
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString text = "当前用户: " + current_user + " | 当前时间:"+current_date+" ";
    spacelabel2->setText(text);
}

/* 更新标题栏信息 */
void database::updateTitle(int type)
{
    //获取系统时间
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString text;

    //如果当前使用者为游客，更新标题栏，直接返回
    if(type==VISITOR){
         text = "当前用户：游客 | 当前时间:"+current_date+" ";
         spacelabel2->setText(text);
         return;
    }

    //登录后删除登录、注册按钮
    delete loginbutton;
    delete registerbutton;

    //新建退出登录按钮，点击退出登录按钮，进行退出登录操作
    quitloginbutton = new QPushButton("退出登录");
    connect(quitloginbutton,SIGNAL(clicked()),this,SLOT(quitLogin()));
    hboxLayout3->addWidget(quitloginbutton);

    quitloginbutton->setFlat(true);

    //根据当前使用者显示不同的标题栏
    if(type==USER_LOGIN){
        text = "当前用户："+username1->text()+" | 当前时间:"+current_date+" ";
        current_user = username1->text();
        user_type = USER;
        //删除登录窗口
        delete loginTabWidget;
    }
    else if(type==USER_REGISTER){
        text = "当前用户："+nameLine3->text()+" | 当前时间:"+current_date+" ";
        current_user = nameLine3->text();
        user_type = USER;
        //删除注册窗口
        delete registerWindow;
    }
    else if(type==MANAGER){
        text = "当前用户："+username2->text()+" | 当前时间:"+current_date+" ";
        current_user = username2->text();
        user_type = MANAGER;
        //删除登录窗口
        delete loginTabWidget;
    }
    spacelabel2->setText(text);
}

/* 创建登录窗口（登录窗口布局的辅助函数） */
QWidget* database::createLoginWindow(int type)
{
    //申请内存
    QWidget *widget = new QWidget();
    QLabel *username_label = new QLabel;
    QLabel *password_label = new QLabel;
    QGridLayout *glayout = new QGridLayout;
    QPushButton *yesbutton = new QPushButton(tr("确定"));
    QPushButton *quitbutton = new QPushButton(tr("退出"));
    QHBoxLayout *hlayout = new QHBoxLayout();
    QVBoxLayout *vlayout = new QVBoxLayout();

    //根据不同类型建立不同的输入栏
    if(type==1){
        username1 = new QLineEdit();
        password1 = new QLineEdit();
        password1->setEchoMode(QLineEdit::Password);
        glayout->addWidget(username1,1,2);
        glayout->addWidget(password1,2,2);
    }
    else if(type==2){
        username2 = new QLineEdit();
        password2 = new QLineEdit();
        password2->setEchoMode(QLineEdit::Password);
        glayout->addWidget(username2,1,2);
        glayout->addWidget(password2,2,2);
    }

    username_label->setText(tr("账号"));
    password_label->setText(tr("密码"));

    //表格布局加入文字标签
    glayout->setContentsMargins(50,100,50,100);
    glayout->setSpacing(40);
    glayout->addWidget(username_label,1,1);

    glayout->addWidget(password_label,2,1);

    //加入按钮和输入栏
    hlayout->addStretch();
    hlayout->addWidget(yesbutton);
    hlayout->addWidget(quitbutton);
    hlayout->addStretch();
    vlayout->addLayout(glayout);
    vlayout->addLayout(hlayout);

    widget->setLayout(vlayout);

    //点击退出按钮，进行删除登录窗口操作
    connect(quitbutton,SIGNAL(clicked()),this,SLOT(deleteLogin()));

    //点击确认按钮，根据类型不同将信号发送到用户登录和管理员登录中
    if(type==1){
        connect(yesbutton,SIGNAL(clicked()),this,SLOT(userLogin()));
    }
    else if(type==2){
        connect(yesbutton,SIGNAL(clicked()),this,SLOT(managerLogin()));
    }

    return widget;
}

/* 清空搜索框 */
void database::clear()
{
    nameLine->clear();
    publishLine->clear();
    authorLine->clear();
    priceLine->clear();
    priceLine1->clear();
}

/* 清空多册添加的表单 */
void database::clearAddBookMessage()
{
    //对每一个表单，设置为空
    for(int i = 0;i<13;i++){
        for(int j = 0;j<7;j++){
            if(j!=5&&j!=4){
                tablewidget3->item(i,j)->setText("");
            }
        }
    }
    //如果有图书信息缓存，清空
    for(int i=0;i<multiBookMessage.length();i++){
        multiBookMessage.pop_front();
    }
}

/* 删除登录界面*/
void database::deleteLogin()
{
    //删除登陆界面，并将登录注册按钮设为可使用
    delete loginTabWidget;
    loginbutton->setEnabled(true);
    registerbutton->setEnabled(true);
}

/* 退出登录更新界面*/
void database::quitLogin()
{
    //删除退出登录按钮
    delete quitloginbutton;

    //根据当前使用者的不同，删除对应的窗口以及控件
    if(user_type==MANAGER){
        delete window3;
        delete window2;
        delete window5;
    }
    else if(user_type==USER){
        delete window4;
        delete lendBookButton;
    }

    //获取系统时间
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString text = "当前用户: 游客 | 当前时间:"+current_date+" ";

    //更新使用者类型为游客
    user_type = VISITOR;
    current_user = "游客";
    spacelabel2->setText(text);

    //重新建立登录注册按钮，并摆放
    loginbutton = new QPushButton("登录");
    registerbutton = new QPushButton("注册");
    loginbutton->setFlat(true);
    registerbutton->setFlat(true);
    registerbutton->setFixedSize(QSize(60,30));
    loginbutton->setFixedSize(QSize(60,30));
    hboxLayout3->addWidget(loginbutton);
    hboxLayout3->addWidget(registerbutton);

    //重新连接登录注册与对应的槽函数
    connect(loginbutton,SIGNAL(clicked()),this,SLOT(setWindowLogin()));
    connect(registerbutton,SIGNAL(clicked()),this,SLOT(setWindowRegister()));
}

/* 用户管理翻页（上一页）*/
void database::userLastPage()
{
    //当前页为1,不存在上一页，返回
    if(user_current_page==1)return;
    //当前页减1
    user_current_page--;
    //更新用户管理窗口
    updateManageWindow();
}

/* 用户管理翻页（下一页）*/
void database::userNextPage()
{
    //如果当前页面为最大页面，不存在下一页面，返回
    if(user_current_page==user_max_page)return;
    //当前页面加一
    user_current_page++;
    //更新用户管理窗口
    updateManageWindow();
}

/* 显示书籍翻页（上一页）*/
void database::bookLastPage()
{
    //当前页为1,不存在上一页，返回
    if(current_page==1)return;
    //当前页减1
    current_page--;
    //更新显示书本窗口
    updateShowBookWindow();
}

/* 显示书籍翻页（下一页）*/
void database::bookNextPage()
{
    //如果当前页面为最大页面，不存在下一页面，返回
    if(current_page==max_page)return;
    //当前页面加一
    current_page++;
    //更新显示书本窗口
    updateShowBookWindow();
}

/* 打开文件导入书籍数据 */
void database::openFile()
{
    //新建文件显示窗口
    fileWindow = new QWidget;
    QFileDialog dlg(fileWindow,"打开文本");
    dlg.resize(400,300);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);

    //只允许打开.txt后缀的文件
    dlg.setNameFilter("*txt");

    //打开文件对话框
    if(dlg.exec()==QDialog::Accepted ){
        QStringList files = dlg.selectedFiles();
        if(!files.isEmpty()){
            //以只读形式打开选中文件
            QFile file;
            file.setFileName(files.at(0));
            file.open(QIODevice::ReadOnly);
            if(!file.atEnd()){
                int count = 0;
                int rowNum = 0;
                //读取文本所有信息，并进行转码处理
                QByteArray buff;
                buff = file.readAll();
                QString fileContent = QString::fromLocal8Bit(buff);
                QString str = "";

                //对文本进行格式化处理
                for(int k=0;k<=fileContent.length();k++){
                    //读到换行符或者读完文本，把书籍信息存储
                    if(fileContent.at(k)=='\n'||k==fileContent.length()){
                        bookMessage[6] = str;
                        if(rowNum>=13){ //列数超过显示范围，暂存到容器中
                            QString *tmp = new QString[7];
                            for(int i=0;i<7;i++){
                                tmp[i] = bookMessage[i];
                            }
                            multiBookMessage.push_back(tmp);
                        }
                        else{ //列数未超过显示范围，直接显示在表格中
                            for(int i=0;i<7;i++){
                                if(i==4)publishDate[rowNum]->setCurrentText(bookMessage[i]);
                                else if(i==5)bookType[rowNum]->setCurrentText(bookMessage[i]);
                                else{
                                    tablewidget3->item(rowNum,i)->setText(bookMessage[i]);
                                }
                            }
                            rowNum++;
                            count = 0;
                            str = "";
                        }
                        //如果以换行符结束，不需要继续处理
                        if(fileContent.at(k)=='\n'&&k==fileContent.length()-1){
                            break;
                        }
                    }
                    //读到空格且不是作者信息中的空格，跳过空格，并把字符串存储到bookmessage中
                    else if(fileContent.at(k)==' '&&count!=6){
                        bookMessage[count++] = str;
                        str = "";
                    }
                    //读到空格且是作者信息中的空格，直接存储空格信息
                    else if(fileContent.at(k)==' '&&count==6){
                        str = str + fileContent.at(k);
                    }
                    //读到支持的文本内容，将其加入字符串中
                    else if(fileContent.at(k).isLetter()||fileContent.at(k).isNumber()
                            ||fileContent.at(k)=='.'||fileContent.at(k)==':'
                            ||fileContent.at(k)=='-'||fileContent.at(k)=='"'
                            ||fileContent.at(k)=='('||fileContent.at(k)==')'
                            ||fileContent.at(k)=='·'){
                        str = str + fileContent.at(k);
                    }
                }
            }
        }
    }
}

/*  _______________________
   |                       |
   |   第二部分：业务逻辑     |
   |                       |
    ———————————————————————
*/

/* 用户注册 */
void database::Register()
{
    //检测用户是否输入了全部的信息
    if(nameLine3->text().isEmpty()||passwordLine1->text().isEmpty()||passwordLine2->text().isEmpty()){
        QMessageBox::critical(NULL, "Error", "您的信息填写不完整",
                              QMessageBox::Yes);
        return;
    }

    //检测密码是否一致
    if(passwordLine1->text().compare(passwordLine2->text())!=0){
        QMessageBox::critical(NULL, "Error", "两次密码输入不一致！",
                              QMessageBox::Yes);
        return;
    }

    //查询用户名是否已存在于数据库
    QSqlQuery query;
    query.exec("select username from user where username = '" + nameLine3->text() + "'");
    if(query.next()){
        QMessageBox::critical(NULL, "Error", "该用户名已被注册",
                              QMessageBox::Yes);
        return;
    }

    //向user表中插入用户信息
    query.exec("insert into user values('" + nameLine3->text() + "','" + passwordLine1->text() + "'" +",false)");
    if(query.isActive()){
        QMessageBox::about(0,"database","注册成功！");
    }
    else{
        QMessageBox::critical(NULL, "Error", "注册失败",
                              QMessageBox::Yes);
        return;
    }

    //更新检索书籍窗口
    setWindowSearchBook(USER_REGISTER);
}

/* 用户登录 */
void database::userLogin()
{
    QSqlQuery query2;

    //根据用户输入的用户名，在user表中查询其密码
    query2.exec("select password from user where username = '"
                +username1->text()+"'");
    if(!query2.isActive()){
        return;
    }

    //判断密码是否与结果一致
    if(query2.next()){
        QString sel_password = query2.value(0).toString();
        if(QString::compare(sel_password,password1->text())!=0){
            QMessageBox::critical(NULL, "Error", "密码错误",
                                  QMessageBox::Yes);
            return;
        }
    }
    //如果数据库中没有此用户名，给出警告
    else{
        QMessageBox::critical(NULL, "Error", "该用户名不存在",
                              QMessageBox::Yes);
        return;
    }

    //更新检索书籍窗口
    setWindowSearchBook(USER_LOGIN);
}

/* 管理员登录 */
void database::managerLogin()
{
    QSqlQuery query2;

    //根据用户输入的用户名，在manager表中查询其密码
    query2.exec("select password from manager where username = '"
                +username2->text()+"'");
    if(!query2.isActive()){
        return;
    }

    //判断密码是否与结果一致
    if(query2.next()){
        QString sel_password = query2.value(0).toString();
        if(QString::compare(sel_password,password2->text())!=0){
            QMessageBox::critical(NULL, "Error", "密码错误",
                                  QMessageBox::Yes);
            return;
        }
    }

    //如果数据库中没有此用户名，给出警告
    else{
        QMessageBox::critical(NULL, "Error", "该用户名不存在",
                              QMessageBox::Yes);
        return;
    }

    //更新标题栏信息
    updateTitle(MANAGER);

    //添加图书单册入库窗口
    setWindowAddBook();
    //添加用户管理窗口
    setWindowUserManage();
    //添加图书多册入库窗口
    setWindowMutiAddBook();

    //在分页栏中加入以上窗口
    mainTabWidget->addTab(window2,"用户管理");
    mainTabWidget->addTab(window3,"单册入库");
    mainTabWidget->addTab(window5,"多册入库");
}

/* 多条件查询书籍 */
void database::searchBook()
{
    //判断用户是否填写了信息
    if(nameLine->text().isEmpty()&&publishLine->text().isEmpty()&&authorLine->text().isEmpty()
            &&(!dateBox->currentText().compare("    ")&&!dateBox1->currentText().compare("    "))
            &&!isLent->currentText().compare("    ")&&(priceLine->text().isEmpty()&&priceLine1->text().isEmpty())){
        QMessageBox::critical(0,QObject::tr("error"),"您尚未填写任何信息");
        return;
    }

    //检查日期和价格的填写是否正确
    else if(((dateBox->currentText().compare("    ")==0&&dateBox->currentText().compare("    ")!=0)||
            (dateBox->currentText().compare("    ")!=0&&dateBox->currentText().compare("    ")==0))&&
            (!priceLine->text().isEmpty()&&priceLine1->text().isEmpty()||
             priceLine->text().isEmpty()&&!priceLine1->text().isEmpty())){
        QMessageBox::critical(0,QObject::tr("error"),"您的日期和价格范围有误");
        return;
    }
    else if((dateBox->currentText().compare("    ")==0&&dateBox->currentText().compare("    ")!=0)||
            (dateBox->currentText().compare("    ")!=0&&dateBox->currentText().compare("    ")==0)){
        QMessageBox::critical(0,QObject::tr("error"),"您的日期范围有误");
        return;
    }
    else if(!priceLine->text().isEmpty()&&priceLine1->text().isEmpty()||
            priceLine->text().isEmpty()&&!priceLine1->text().isEmpty()){
        QMessageBox::critical(0,QObject::tr("error"),"您的价格范围有误");
        return;
    }
    int x1,x2,y1,y2;
    x1=x2=y1=y2=0;
    if(dateBox->currentText().compare("    ")){
        x1 = dateBox->currentText().toInt();
        x2 = dateBox1->currentText().toInt();
    }
    if(!priceLine->text().isEmpty()){
        y1 = priceLine->text().toInt();
        y2 = priceLine1->text().toInt();
    }

    if(x1>x2&&y1>y2){
        QMessageBox::critical(0,QObject::tr("error"),"您的日期和价格范围有误");
        return;
    }
    else if(x1>x2){
        QMessageBox::critical(0,QObject::tr("error"),"您的日期范围有误");
        return;
    }
    else if(y1>y2){
        QMessageBox::critical(0,QObject::tr("error"),"您的价格范围有误");
        return;
    }

    //将用户输入的信息暂存于容器中
    QVector<QString>list;
    if(!nameLine->text().isEmpty()){
        list.push_back("name");
        list.push_back(nameLine->text());
    }
    if(!publishLine->text().isEmpty()){
        list.push_back("publisher");
        list.push_back(publishLine->text());
    }
    if(!authorLine->text().isEmpty()){
        list.push_back("author");
        list.push_back(authorLine->text());
    }
    if(dateBox->currentText().compare("    ")!=0){
        list.push_back("publish_year");
        list.push_back(dateBox->currentText());
        list.push_back(dateBox1->currentText());
    }

    if(isLent->currentText().compare("    ")!=0){
        QString str;
        if(isLent->currentText().compare("未借出")==0){
            str = ">";
        }
        else if(isLent->currentText().compare("已借出")==0){
            str = "=";
        }
        list.push_back("number");
        list.push_back(str);
    }
    if(!priceLine->text().isEmpty()){
        list.push_back("price");
        list.push_back(priceLine->text());
        list.push_back(priceLine1->text());
    }

    //根据用户输入的信息在book表和book_author两个表中进行检索
    int len = list.length();
    int count = 0;
    QString ans = "select * from book where " ;
    QSqlQuery query,query2;

    while(count<len){
        if(count!=0)ans = ans + " and ";
        QString tmp = list.front();
        list.pop_front();
        count++;
        QString tmp1,tmp2;
        if(tmp.compare("publish_year")==0){
            tmp1 = list.front();
            list.pop_front();
            count++;
            tmp2 = list.front();
            list.pop_front();
            count++;
            ans = ans + tmp + " >= '" + tmp1 + "' and " +
                    tmp + " <= '" + tmp2 + "'";
        }
        else if(tmp.compare("price")==0){
            tmp1 = list.front();
            list.pop_front();
            count++;
            tmp2 = list.front();
            list.pop_front();
            count++;
            ans = ans + tmp + " > " + tmp1 + " and " +
                    tmp + " < " + tmp2;
        }

        else if(tmp.compare("number")==0){
            tmp1 = list.front();
            list.pop_front();
            count++;
            ans = ans + tmp + tmp1 + "0";
        }
        else if(tmp.compare("author")==0){
            bool flag = false;
            QVector<QString>str;
            tmp1 = list.front();
            list.pop_front();
            count++;
            query2.exec("select book_id from book_author where author_name like '%" + tmp1 + "%'");
            while(query2.next()){
                flag = true;
                str.push_back(query2.value(0).toString());
            }
            if(!flag){
                QMessageBox::critical(0,QObject::tr("error"),"未查到搜索的结果！");
                return;
            }
            if(str.length()==1){
                 ans = ans + "book_id = '" + str.front() + "'";
                 str.pop_front();
            }
            else{
                for(int i=0;i<str.length();i++){
                    if(i==0) ans = ans + "(book_id = '" + str[i] + "'";
                    else if(i==str.length()-1) ans = ans + " or book_id = '" + str[i] + "')";
                    else ans = ans + " or book_id = '" + str[i] + "'";
                }
            }
            str.clear();
        }
        else{
            tmp1 = list.front();
            list.pop_front();
            count++;
            ans = ans + tmp + " like '%" + tmp1 + "%'";
        }
    }

    query.exec(ans);
    if(!query.isActive()){
        QMessageBox::critical(0,QObject::tr("error"),query.lastError().text());
    }

    //将结果容器清空，把搜索的结果暂存于结果容器中，之后根据页码显示相应的结果
    result.clear();

    while(query.next()){

        QString ans="";
        QString str = query.value(0).toString();
        query2.exec("select author_name from book_author where book_id = '"+str+"'");
        while(query2.next()){
            ans = ans + query2.value(0).toString() + " ";
        }
        for(int i=0;i<9;i++){
            result.push_back(query.value(i).toString());
        }
        result.push_back(ans);
    }
    if(result.length()==0){
        QMessageBox::critical(0,QObject::tr("error"),"未查到搜索的结果！");
        return;
    }

    //初始化当前页和最大页
    current_page = 1;
    max_page = result.length()/100 + (result.length()%100!=0);

    //更新显示书籍窗口
    updateShowBookWindow();
}

/* 按照书的分类查询书籍 */
void database::searchBookByType(QTreeWidgetItem* tree,int num)
{
    QSqlQuery query,query2;
    QString text = tree->text(0);

    //根据分类，在book表和book_author两个表中进行检索
    query.exec("select * from book "
               "where type = '"+text+"'");
    if(!query.isActive()){
        QMessageBox::critical(0,QObject::tr("error"),"未查询到结果！");
    }

    //将结果容器清空，把搜索的结果暂存于结果容器中，之后根据页码显示相应的结果
    result.clear();
    while(query.next()){
        QString ans="";
        QString str = query.value(0).toString();
        query2.exec("select author_name from book_author where book_id = '"+str+"'");
        while(query2.next()){
            ans = ans + query2.value(0).toString() + " ";
        }
        for(int i=0;i<9;i++){
            result.push_back(query.value(i).toString());
        }
        result.push_back(ans);
    }

    //初始化当前页和最大页
    current_page = 1;
    max_page = result.length()/100 + (result.length()%100!=0);

    //更新显示书籍窗口
    updateShowBookWindow();
}

/* 加载借书信息 */
void database::loadLoanMessage()
{
    QSqlQuery query,query1;

    //由于容器中存的是指针对象，需要一个个释放内存
    //防止频繁更新表导致的内存泄漏
    for(int i=0;i<loanMessage.length();i++){
        for(int j=0;j<5;j++){
            loanMessage.at(i)[j].clear();
        }
    }
    //清空借阅信息
    loanMessage.clear();

    //从表loan中查询借书信息
    query.exec("select book_id,loan_date,loan_number from loan where username = '"
               + current_user + "'");

    //把搜索的结果暂存于借阅信息容器中，之后根据页码显示相应的结果
    while(query.next()){
        QString *ans = new QString[6];
        ans[0] = query.value(0).toString();
        query1.exec("select name from book where book_id = '" + ans[0] + "'");
        if(query1.next()){
            ans[1] = query1.value(0).toString();
        }
        else{
            qDebug()<<"error";
            return;
        }
        ans[2] = query.value(1).toString();//日期
        ans[3] = query.value(2).toString();//单册编号
        QDate currentDate = QDate::currentDate();
        QDate loanDate = query.value(1).toDate();
        QDate returnDate = loanDate.addDays(90);
        int remainingTime = currentDate.daysTo(returnDate);
        ans[4] = returnDate.toString("yyyy-MM-dd");
        if(remainingTime>=0){
            ans[5] = QString::number(remainingTime);
        }
        else{
            ans[5] = "已逾期";
        }
        loanMessage.push_back(ans);
    }
}

/* 加载用户信息 */
void database::loadUserMessage()
{
    user_num = 0;
    QSqlQuery query,query1,query2;

    //由于容器中存的是指针对象，需要一个个释放内存
    //防止频繁更新表导致的内存泄漏
    for(int i=0;i<userMessage.length();i++){
        for(int j=0;j<10;j++){
            userMessage.at(i)[j].clear();
        }
    }
    //清空用户信息
    userMessage.clear();

    //在表user中搜索所有信息
    query.exec("select * from user");

    int i = 0;
    QString name[3];
    QString id[3];
    QString date[3];

    //把搜索的结果暂存于借阅信息容器中，之后根据页码显示相应的结果
    while(query.next()){
        QString *ans = new QString[10];
        int num = 0;
        int count = 0;
        for(int j=0;j<=2;j++){
            ans[j] = query.value(j).toString();
        }

        query1.exec("select book_id,loan_date"
                    " from loan where username = '" + ans[0] + "'");
        while(query1.next()){
            id[count] = query1.value(0).toString();
            date[count] = query1.value(1).toString();
            num++;
            query2.exec("select name from book where book_id = '"
                        + id[count] + "'");
            if(query2.next()){
               name[count++] = query2.value(0).toString();
            }
        }
        ans[3] = QString::number(num);

        for(int j=0;j<count;j++){
            ans[4 + 2*j] = name[j] + "(" + id[j] + ")";
            ans[5 + 2*j] = date[j];
        }
        for(int j=count;j<3;j++){
            ans[4 + 2*j] = "";
            ans[5 + 2*j] = "";
        }
        userMessage.push_back(ans);
        i++;
        user_num++;
    }

    //初始化当前页码和最大页码
    user_current_page = 1;
    if(user_num%13==0)user_max_page = user_num/13;
    else user_max_page = user_num/13 + 1;
}

/* 连接数据库 */
bool database::createConnection()
{
    //连接MySQL数据库
    db = QSqlDatabase::addDatabase("QMYSQL");
    //设置主机名
    db.setHostName("localhost");
    //设置数据库名
    db.setDatabaseName("book_manage");
    //设置账号名
    db.setUserName("root");
    //设置密码名
    db.setPassword("root");
    //设置端口
    db.setPort(3306);
    if(!db.open()){
        QMessageBox::critical(0,QObject::tr("error"),db.lastError().text());
        return false;
    }
    return true;
}

/* 删除书籍 */
void database::deleteBook()
{
    QSqlQuery query,query1;

    //删除之前，检索书号是否存在，便于给出用户反馈信息
    query1.exec("select book_id from book where book_id = '" + idLine->text() + "'");
    if(!query1.next()){
        QMessageBox::critical(NULL, "Error", "该书号不存在",
                              QMessageBox::Yes);
        return;
    }

    //从book中删除该书本信息
    query.exec("delete from book where book_id = '" + idLine->text() + "'");
    if(!query.isActive()){
        QMessageBox::critical(NULL, "Error", "删除失败",
                              QMessageBox::Yes);
    }
    else{
        QMessageBox::about(NULL,"Ok","删除成功");
    }
}

/* 书籍入库 */
bool database::addBook()
{
    QSqlQuery query,query1,query2;

    QString author_str = bookMessage[6];
    QVector<QString>list;
    QString tmp="";

    //入库前，先查找是否存在相同（除作者）的书籍
    query.exec("select book_id from book where name = '"
               + bookMessage[0] + "' and price = '"
               + bookMessage[1] + "' and publisher = '"
               + bookMessage[3] + "' and publish_year = '"
               + bookMessage[4] + "' and type = '"
               + bookMessage[5] + "'");

    //如果存在的话，给出已存在书的信息
    //并向用户询问，选择将两本书合并，还是退出
    if(query.next()){
        query1.exec("select author_name from book_author where book_id = '" +
                    query.value(0).toString() + "'");
        QString author = "";
        QString str = "";
        while(query1.next()){
            author = author + query1.value(0).toString() + " ";
        }
        for(int i = 0;i<6;i++){
            if(i==2)continue;
            else str = str + bookMessage[i] + ",";
        }
        str = str + author;
        QMessageBox::StandardButton messagebox  = QMessageBox::question(NULL,"question","图书库已有类似图书：\n（" +
                              str + ")\n是否需要合并两本书？（不合并则取消入库）");
        if(messagebox==QMessageBox::Yes){
            query2.exec("update book set number = number + " + bookMessage[2]
                    + " where book_id = '" + query.value(0).toString() +"'");
            if(query2.isActive()){
                return true;
            }
            else return false;
        }
        else if(messagebox==QMessageBox::No){
            return false;
        }
    }

    //对用户一次输入的多个作者，进行格式化，将作者名分开，并暂存于容器中
    for(int i=0;i<author_str.length();i++){
        if(author_str.at(i)!=' ')tmp = tmp + author_str.at(i);
        else{
            list.push_back(tmp);
            tmp = "";
        }
    }
    if(!tmp.isEmpty())list.push_back(tmp);

    //获取当前时间
    QDate d = QDate::currentDate();
    QString str2 = d.toString("yyyy-MM-dd");

    //查询当前书的总数，为该书分配一个编号，计算方法为当前书总数 + 1
    //如果该编号已存在，继续查找连续的编号，直到找到一个未使用的编号为止
    query.exec("select count(*) from book");
    int num = 0;
    int tmp_num;
    if(query.next()){
        num = query.value(0).toInt();
    }
    if(num>=99999){
        QMessageBox::critical(NULL,"error","数据库存储已满，请申请数据库扩容");
        return false;
    }
    tmp_num = ++num;

    int count = 0;
    while(tmp_num){
        tmp_num/=10;
        count++;
    }
    count=5-count;

    QString str = QString::number(num);

    for(int i=0;i<count;i++){
        str = "0" + str;
    }

    query.exec("select book_id from book where book_id = '" + str +"'");

    while(query.next()){
        count = 0;
        tmp_num = ++num;
        while(tmp_num){
            tmp_num/=10;
            count++;
        }
        str = QString::number(num);
        count=5-count;
        for(int i=0;i<count;i++){
            str = "0" + str;
        }
        query.exec("select book_id from book where book_id = '" + str +"'");
    }

    //向表book中插入书籍信息
    query2.exec("insert into book values('"
               + str + "','" + bookMessage[0] + "'," +
               bookMessage[1] + "," + bookMessage[2] + ",'" +
               str2 + "','" + current_user + "','" +
               bookMessage[3] + "','" + bookMessage[4] +
               "','" + bookMessage[5]+"')");

    //向表book_author中插入作者信息
    for(int i=0;i<list.size();i++){
        query1.exec("insert into book_author values('"+
                    str + "','"+list.front()+"')");
        list.pop_front();
    }

    if(query1.isActive()&&query1.isActive()){
        return true;
    }
    else{
        return false;
    }
}

/* 单册书籍入库（加载书籍信息） */
void database::oneAddBook()
{
    //检查用户填写的信息是否完整
    if(nameLine2->text().isEmpty()||priceLine2->text().isEmpty()||
            authorLine2->text().isEmpty()||numLine2->text().isEmpty()||
            publishLine2->text().isEmpty()){
        QMessageBox::critical(NULL, "Error", "您的信息填写不完整",
                              QMessageBox::Yes);
        return;
    }

    //将用户输入的信息暂存于数组中
    bookMessage[0] = nameLine2->text();
    bookMessage[1] = priceLine2->text();
    bookMessage[2] = numLine2->text();
    bookMessage[3] = publishLine2->text();
    bookMessage[4] = dateBox2->currentText();
    bookMessage[5] = typeBox2->currentText();
    bookMessage[6] = authorLine2->text();

    if(addBook()){
        QMessageBox::about(0,"database","插入成功！");
    }
    else{
        QMessageBox::critical(0,QObject::tr("error"),"插入失败");
    }

    //清空输入框
    nameLine2->clear();
    priceLine2->clear();
    numLine2->clear();
    publishLine2->clear();
    authorLine2->clear();
}

/* 多册书籍入库（加载书籍信息） */
void database::multiAddBook()
{
    bool flag = true;
    int count = 0;//统计书本数目

    //检查用户输入是否完整
    for(int i=0;i<13;i++){
        bool hasValue = false;
        bool isEmpty = false;
        for(int j=0;j<7;j++){
            if(j==4||j==5)continue;
            else{
                if(tablewidget3->item(i,j)->text().compare("")==0){
                    isEmpty = true;
                }
                else hasValue = true;
            }
            if(hasValue&&isEmpty){
                QMessageBox::critical(NULL,"error","您的信息填写不完整");
                return;
            }
        }
        if(!isEmpty){
            count++;
        }
    }

    //将用户输入的信息暂存于数组中
    for(int i=0;i<count;i++){
        for(int j=0;j<7;j++){
            if(j==4){
                bookMessage[j] = publishDate[i]->currentText();
            }
            else if(j==5){
                bookMessage[j] = bookType[i]->currentText();
            }
            else{
                bookMessage[j] = tablewidget3->item(i,j)->text();
            }
        }
        if(!addBook()){
            flag = false;
            QMessageBox::critical(NULL,"error","第" + QString::number(i+1) + "本书添加失败");
        }
    }
    if(flag){
        QMessageBox::about(NULL,"ok","插入成功");
        //清空书本信息
        for(int i=0;i<7;i++){
            bookMessage[i].clear();
        }
    }
    //对每一个表单，设置为空
    for(int i = 0;i<13;i++){
        for(int j = 0;j<7;j++){
            if(j!=5&&j!=4){
                tablewidget3->item(i,j)->setText("");
            }
        }
    }
    //继续加载文本信息（如果还有的话）
    int max = multiBookMessage.length()<13?multiBookMessage.length():13;
    for(int i=0;i<max;i++){
        for(int j=0;j<7;j++){
            if(j==4)publishDate[i]->setCurrentText(multiBookMessage.at(i)[j]);
            else if(j==5)bookType[i]->setCurrentText(multiBookMessage.at(i)[j]);
            else{
                tablewidget3->item(i,j)->setText(multiBookMessage.at(i)[j]);
            }
        }
    }

    for(int i=0;i<max;i++){
        multiBookMessage.pop_front();
    }
}

/* 删除用户 */
void database::deleteUser()
{
    QSqlQuery query;
    //判断是否选中用户
    bool flag = false;

    //求出当前页面最多显示的用户
    int max;
    if(user_current_page==user_max_page){
        max = (user_num%13==0)?13:user_num%13;
    }
    else max = 13;

    //删除管理员选中的用户信息
    for(int i=0;i<max;i++){
        if(item0[i]->checkState() == Qt::Checked){
            //如果用户有未归还的书籍，禁止删除
            if(tablewidget1->item(i,4)->text().toInt()>0){
                QMessageBox::critical(0,"error","该用户有未归还的书籍，不能删除！");
                return;
            }
            flag = true;
            query.exec("delete from user where username = '" +
                      item1[i]->text() + "'");
        }
    }
    if(!flag){
        QMessageBox::critical(0,"error","您尚未选择任何用户！");
        return;
    }
    if(query.isActive()){
        QMessageBox::about(0,"ok","删除成功");
    }
    else{
        QMessageBox::critical(0,"error","删除失败");
    }

    //更新用户信息
    loadUserMessage();
    //更新用户管理窗口
    updateManageWindow();
}

/* 授予借书权限*/
void database::grantLent()
{
    //判断是否选中用户
    bool flag = false;

    //判断执行的操作是授权还是取消授权
    int isGrant;
    QSqlQuery query;

    //求出当前页面最多显示的用户
    int max;
    if(user_current_page==user_max_page){
        max = (user_num%13==0)?13:user_num%13;
    }
    else max = 13;

    //根据用户当前借阅权限，执行授予借阅权限/取消借阅权限操作
    for(int i=0;i<max;i++){
        if(item0[i]->checkState() == Qt::Checked){
            flag = true;
            isGrant = tablewidget1->item(i,3)->text().toInt();

            if(isGrant==0){
                isGrant = true;
                query.exec("update user set isLent = true where username = '" +
                          item1[i]->text() + "'");
            }
            else if(isGrant==1){
                isGrant = false;
                query.exec("update user set isLent = false where username = '" +
                          item1[i]->text() + "'");
            }
        }
    }
    if(!flag){
        QMessageBox::critical(0,"error","您尚未选择任何用户！");
        return;
    }
    if(query.isActive()){
        if(isGrant){
            QMessageBox::about(0,"ok","授权成功");
        }
        else{
            QMessageBox::about(0,"ok","取消权限成功");
        }
    }
    else{
        if(isGrant){
            QMessageBox::critical(0,"error","授权失败");
        }
        else{
            QMessageBox::critical(0,"error","取消权限失败");
        }
        return;
    }

    //更新用户信息
    loadUserMessage();
    //更新用户管理窗口
    updateManageWindow();
}

/* 归还书籍 */
void database::returnBook()
{
    //判断是否选中书籍
    bool flag = false;
    QSqlQuery query,query1;

    //执行归还操作
    for(int i=0;i<loanMessage.length();i++){
        if(item4[i]->checkState() == Qt::Checked){
            flag = true;
            query.exec("delete from loan where username = '" +
                       current_user + "' and book_id = '" +
                       loanMessage.at(i)[0] + "' and loan_number = " +
                       loanMessage.at(i)[3]);
            query1.exec("update book set number = number + 1 where book_id = '"+
                       loanMessage.at(i)[0] + "'");
        }
    }
    if(!flag){
        QMessageBox::critical(0,"error","您尚未选中任何书目！");
        return;
    }
    if(query.isActive()){
        QMessageBox::about(NULL,"ok","归还成功！");
    }
    else{
        QMessageBox::about(NULL,"error","归还失败");
        return;
    }
    if(query1.isActive()){
        QMessageBox::about(NULL,"ok","更新成功！");
    }
    else{
        QMessageBox::about(NULL,"error","更新失败");
        return;
    }

    //更新借书窗口
    updateLoanBookWindow();
}

/* 借阅书籍 */
void database::lendBook()
{
    //判断是否选中书籍
    bool flag = false;
    int max_num = 0;
    QSqlQuery query,query1,query2;

    //借书前判断用户是否具有借阅权限
    query.exec("select isLent from user where username = '" + current_user + "'");

    if(query.next()){
        bool isLent = query.value(0).toBool();
        if(!isLent){
            QMessageBox::critical(0,"error","您不具有借书权限,需等待管理员添加!");
            return;
        }
    }
    else{
        return;
    }

    //判断用户借阅的书籍是否达到最大书籍
    query.exec("select count(*) from loan where username = '" +
               current_user + "'");
    if(query.next()){
        int totalNum = query.value(0).toInt();
        if(totalNum>=3){
            QMessageBox::critical(0,"error","您的借书数目已达到限额（3本）！");
            return;
        }
    }
    else{
        return;
    }

    //得到当前页码所能显示的最多书籍
    int max;
    if(current_page==max_page){
        max = result.length()%100/10;
        if(result.length()%100==0)max = 10;
    }
    else max = 10;
    QDate d = QDate::currentDate();
    QString date = d.toString("yyyy-MM-dd");

    //根据用户选中的书籍插入相应借阅信息
    for(int i=0;i<max;i++){
        if(item2[i]->checkState() == Qt::Checked){
            bool isMutiNum = false;
            int num = 1;
            flag = true;

            //判断当前书本是否有库存
            query.exec("select number from book where book_id = '" + item3[i]->text() + "'");
            if(query.next()){
                int num = query.value(0).toInt();
                if(num==0){
                    QString str = item3[i]->text() + "暂无库存！";
                    QMessageBox::critical(NULL,"error",str);
                }
            }
            else{
                return;
            }

            //考虑到一本书可以被借多次，在这里进行特殊的处理
            query.exec("select loan_number from loan where username ='" +
                        current_user + "' and book_id = '" + item3[i]->text()
                        + "'" );
            while(query.next()){
                isMutiNum = true;
                num = query.value(0).toInt();
                if(num>max_num)max_num = num;
            }
            if(isMutiNum){
                max_num++;
                query1.exec("insert into loan values('" + current_user + "','" +
                                        item3[i]->text() + "','" + date + "'," +
                                        QString::number(max_num) + ")" );
            }
            else{
                query1.exec("insert into loan values('" + current_user + "','" +
                                        item3[i]->text() + "','" + date + "',1)" );
            }
            if(query1.isActive()){
                query2.exec("update book set number = number - 1 where book_id = '" +
                            item3[i]->text() + "'");
            }
        }
    }
    if(!flag){
        QMessageBox::critical(0,"error","您尚未选择任何书目！");
        return;
    }
    if(query1.isActive()){
        QMessageBox::about(0,"ok","借阅成功");

    }
    else{
        QMessageBox::critical(0,"error","借阅失败");
        return;
    }

    updateLoanBookWindow();
}

