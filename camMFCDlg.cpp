// camMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "camMFC.h"
#include "camMFCDlg.h"
#include "afx.h"

#include "iostream"
#include "CvvImage.h"
#include "Threshold.h"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv; 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VideoCapture capture;
Mat frame;
CPoint mouse_point;
char Flag_imT=0;
IplImage* imT;
IplImage* imTGray;  //模板图像的灰度图
IplImage* imSGray;  //实时图像的灰度图
IplImage* Result;   //模板匹配结果图
int T_W=128;        //模板图尺寸
int T_H=128;

CRect rect,rect_w;
CDC* pDC;
HDC hDC;
CPoint img_point;   //鼠标在图像中的位置。
bool Flag_init=false;

double min_val;   //模板匹配
double max_val;
CvPoint min_loc;
CvPoint max_loc;
char light_on_num=0;   //指示灯亮的编号

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CcamMFCDlg 对话框




CcamMFCDlg::CcamMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CcamMFCDlg::IDD, pParent)
	, m_static(_T(""))
	, m_x(0)
	, m_y(0)
	, m_camnum(_T("0"))
	, m_xc(0)
	, m_yc(0)
	, m_statelist(_T(""))
	, m_value(0.0)
	, m_thread(0.8)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcamMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATE, m_static);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_CBString(pDX, IDC_CAMLIST, m_camnum);
	DDX_Text(pDX, IDC_XC, m_xc);
	DDX_Text(pDX, IDC_YC, m_yc);
	DDX_Control(pDX, IDC_UP, m_up);
	DDX_Text(pDX, IDC_STATELIST, m_statelist);
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDX_Text(pDX, IDC_THREAD, m_thread);
	DDX_Control(pDX, IDC_VALUE, m_valueC);
}

BEGIN_MESSAGE_MAP(CcamMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CcamMFCDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CcamMFCDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GetModel, &CcamMFCDlg::OnBnClickedGetmodel)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONUP()
ON_WM_CLOSE()
//ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_GetModelHX, &CcamMFCDlg::OnBnClickedGetmodelhx)
ON_BN_CLICKED(IDC_BUTTON3, &CcamMFCDlg::OnBnClickedButton3)
//ON_EN_CHANGE(IDC_THREAD, &CcamMFCDlg::OnEnChangeThread)
//ON_EN_SETFOCUS(IDC_THREAD, &CcamMFCDlg::OnEnSetfocusThread)
ON_BN_CLICKED(IDC_BUTTON4, &CcamMFCDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CcamMFCDlg 消息处理程序

BOOL CcamMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_GetModel)->EnableWindow(false);
	GetDlgItem(IDC_GetModelHX)->EnableWindow(false);
	m_statelist="第一步：选择摄像头并打开。\r\n";
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CcamMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CcamMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CcamMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CcamMFCDlg::OnBnClickedButton1()   //打开摄像头
{
	// TODO: 在此添加控件通知处理程序代码
	//VideoCapture capture(0);//创建VideoCapture对象
	UpdateData(true);
	capture.open(_ttoi(m_camnum));
	if (!capture.isOpened())//判断是否打开摄像头
	{
		m_statelist=m_statelist+CString("没有发现摄像头！\r\n");
		UpdateData(false);
	}
	else
    {
		Flag_init=true;
		m_statelist=m_statelist+CString("摄像头打开正常！\r\n");
		m_statelist=m_statelist+CString("第二步：选择模板。\r\n");
		GetDlgItem(IDC_GetModel)->EnableWindow(true);
		GetDlgItem(IDC_GetModelHX)->EnableWindow(true);
		UpdateData(false);

		//SetTimer(0,40,NULL);
	} 
	SetTimer(0,40,NULL);
}

void CcamMFCDlg::OnBnClickedButton2()    //关闭摄像头
{
	// TODO: 在此添加控件通知处理程序代码
	capture.release();
	if(pDC!=NULL)
		ReleaseDC(pDC);
	KillTimer(0);
	if(imTGray!=NULL)
		cvReleaseImage(&imTGray);
	if(imT!=NULL)
		cvReleaseImage(&imT);
	if(Result!=NULL)
		cvReleaseImage(&Result);
}

void CcamMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!capture.read(frame))//读取ing
    {
		m_static="无图像";
		UpdateData(FALSE);
    }
	else
	{
		IplImage image(frame);
		IplImage* m_Frame;
		m_Frame=&image;
		imSGray = cvCreateImage(cvGetSize(m_Frame),IPL_DEPTH_8U, 1);    //  323
		cvCvtColor(m_Frame,imSGray,CV_BGR2GRAY);  //将实时图像转为灰度图   323
		if(Flag_init)
		{
			pDC=GetDlgItem(IDC_IMAGE)->GetDC();// 获得显示控件的 DC  //PDC DC HDC ? ?
			hDC=pDC->GetSafeHdc();// 获取 HDC(设备句柄) 来进行绘图操作   
			GetDlgItem(IDC_IMAGE)->GetClientRect(&rect);    //GetClientRect, GetWindowRect
			GetDlgItem(IDC_IMAGE)->GetWindowRect(&rect_w);  //图片控件在窗体中的位置
			ScreenToClient(&rect_w);   //以用户端的左上角为原点
			int rw=rect.right-rect.left;    // 求出picture control的宽和高  
			int rh=rect.bottom-rect.top; 
			int iw=frame.cols;  
			int ih=frame.rows;  
			int tx = (int)(rw - iw)/2;          // 使视频帧的显示位置正好在控件的正中
			int ty = (int)(rh - ih)/2;  
			SetRect( rect, tx, ty, tx+iw, ty+ih ); //设置ROI区域 
			rect_w.left=rect_w.left+tx;  //
			rect_w.top=rect_w.top+ty;
			Flag_init=false;
		}
		if(Flag_imT==1)   //选择模板，画框框
		{
			img_point.x=mouse_point.x-rect_w.left;//鼠标在rect_window 图像中的位置
			img_point.y=mouse_point.y-rect_w.top;
			if (img_point.x-64>=0 && img_point.y-64>=0 && img_point.x+64<640 && img_point.y+64<480) 
			{
				cvRectangle(m_Frame,cvPoint(img_point.x-64,img_point.y-64),cvPoint(img_point.x+64,img_point.y+64),cvScalar(0,0,255),1,8,0);   //323
			}
		}
		else if(Flag_imT==2)    //显示模板
		{
			imT = cvCreateImage(cvSize(T_H,T_W), IPL_DEPTH_8U, 3); // IPL_DEPTH_8U 深度图 8U
			cvSetImageROI(m_Frame,cvRect( img_point.x-64,img_point.y-64 ,T_W,T_H));
			imT = cvCloneImage(m_Frame);
			cvResetImageROI(m_Frame);
			imTGray = cvCreateImage(cvGetSize(imT),IPL_DEPTH_8U, 1);  //  323
			cvCvtColor(imT,imTGray,CV_BGR2GRAY);                      //  323
			Flag_imT=3;
		}
		else if(Flag_imT==3)   //
		{
			CDC* pDC_imT=GetDlgItem(IDC_IMT)->GetDC();// 获得显示控件的 DC  
			HDC hDC_imT=pDC_imT->GetSafeHdc();// 获取 HDC(设备句柄) 来进行绘图操作  
			CRect rect_imT;
			GetDlgItem(IDC_IMT)->GetClientRect(&rect_imT); 
			CvvImage cimg_imT;
			cimg_imT.CopyOf(imT);    // 复制图片  
			cimg_imT.DrawToHDC(hDC_imT,&rect_imT);  
			ReleaseDC(pDC_imT);
			Result = cvCreateImage(cvSize(imSGray->width-imTGray->width+1,imSGray->height-imTGray->height+1),IPL_DEPTH_32F,1);
			m_statelist=m_statelist+CString("开始跟踪模板目标！\r\n");
			UpdateData(FALSE);
			Flag_imT=4;
		}
		else if(Flag_imT==4)
		{
			cvMatchTemplate(imSGray,imTGray,Result,CV_TM_CCORR_NORMED);   //image：輸入圖，必須為 8位元或浮點數圖。
			//templ：輸入的template，尺寸必須小於輸入圖，形態需相同。
				//result：比較結果圖，必須為單通道32位元浮點數圖，如果image的尺寸為W x H，templ的尺寸為w x h，則result的尺寸為(W - w + 1)x(H - h + 1)。
				//method：比較方法，有以下六種方法可選擇：目标跟踪算法  CV_TM_SQDIFF_NORMED 归一化平方差匹配法    CV_TM_CCORR_NORMED  归一化相关匹配法
			cvMinMaxLoc(Result,&min_val,&max_val,&min_loc,&max_loc,NULL);   // src：輸入圖。
			    //minVal：極小值，可輸入NULL表示不需要。
				//maxVal ：極大值，可輸入NULL表示不需要。
				//minLoc：極小值的位置，可輸入NULL表示不需要。
				//maxLoc：極大值的位置，可輸入NULL表示不需要。
				//mask：可有可無的遮罩。
			if (max_val>m_thread)
			{
				cvRectangle(m_Frame,cvPoint(max_loc.x,max_loc.y),cvPoint(max_loc.x+T_W-1,max_loc.y+T_H-1),cvScalar(0,0,255),1,8,0);
				m_static="目标跟踪正常！";
				m_value=max_val;   //匹配值

				m_x=max_loc.x+64;      //目标在图像中的坐标// 应该是min_loc.x min_loc.y  在用户坐标里的 +32 +32
				m_y=max_loc.y+64;
				m_xc=m_x-320;          //目标相对于图像中心的坐标
				m_yc=m_y-240;
				light_on_num=0;
				if(m_yc<0)
					light_on_num=light_on_num+1;
				else
					light_on_num=light_on_num+2;
				if(m_xc<0)
					light_on_num=light_on_num+4;
				else
					light_on_num=light_on_num+8;
				UpdateData(false);
			}
			else
			{
				m_static="目标跟踪异常！";
				m_value=max_val;   //匹配值
				UpdateData(false);
			}
		}
		else if(Flag_imT==5)    //霍夫跟踪圆环
		{
			Mat src_gray;
			double centerx=0,centery=0,meanradius=0;
			src_gray=cvarrToMat(imSGray);
			GaussianBlur(src_gray,src_gray,Size(9,9),2,2);
			vector<Vec3f> circles;
			HoughCircles(src_gray,circles,CV_HOUGH_GRADIENT,1.5,10,150,100,0,0);
			int cir_num=circles.size();
			if(cir_num>0)
			{
				for(size_t i=0;i<cir_num;i++)
				{
					Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
					int radius=cvRound(circles[i][2]);
				//	cvCircle(m_Frame,center,radius,Scalar(0,0,255),3,8,0);
					centerx=centerx+circles[i][0];
					centery=centery+circles[i][1];
					meanradius=meanradius+circles[i][2];
				}
				Point cc(cvRound(centerx/cir_num),cvRound(centery/cir_num));
				int rr=cvRound(meanradius/cir_num);
				cvCircle(m_Frame,cc,rr,Scalar(0,255,0),3,8,0);

				light_on_num=0;
				if(cc.y-240<0)
					light_on_num=light_on_num+1;
				else
					light_on_num=light_on_num+2;
				if(cc.x-320<0)
					light_on_num=light_on_num+4;
				else
					light_on_num=light_on_num+8;
				UpdateData(false);
			}
			m_static="圆环跟踪！";
			m_value=cir_num;   //匹配值
			UpdateData(false);
		}

		CvvImage cimg;
		cimg.CopyOf(m_Frame);    // 复制图片  
		cimg.DrawToHDC(hDC,&rect);  

		cvReleaseImage(&imSGray);
	}
	Light_on(light_on_num);              //指示灯显示

	CDialog::OnTimer(nIDEvent);
}
void CcamMFCDlg::OnBnClickedGetmodel()
{
	// TODO: 在此添加控件通知处理程序代码
	Flag_imT=1;
	if(imTGray!=NULL)
		cvReleaseImage(&imTGray);
	if(imT!=NULL)
		cvReleaseImage(&imT);
}

void CcamMFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mouse_point=point;
	if(Flag_imT==1)
		Flag_imT=2;
	CDialog::OnLButtonDown(nFlags, point);
}

void CcamMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mouse_point=point;
	CDialog::OnMouseMove(nFlags, point);
}
void CcamMFCDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	capture.release();
	if(pDC!=NULL)
		ReleaseDC(pDC);  
	KillTimer(0);
	if(imTGray!=NULL)
		cvReleaseImage(&imTGray);
	if(imT!=NULL)
		cvReleaseImage(&imT);
	if(Result!=NULL)
		cvReleaseImage(&Result);

	CDialog::OnClose();
}
void CcamMFCDlg::Light_on(char lightnum)
{
	CDC dc;
	CDC *upDc;
	CRect rect;
	if(lightnum&1)
	{
		upDc=GetDlgItem(IDC_UP)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_UP)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(255,0,0));
	}
	else
	{
		upDc=GetDlgItem(IDC_UP)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_UP)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(128,128,128));
	}
	if(lightnum&2)
	{
		upDc=GetDlgItem(IDC_DOWN)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_DOWN)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(255,0,0));
	}
	else
	{
		upDc=GetDlgItem(IDC_DOWN)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_DOWN)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(128,128,128));
	}
	if(lightnum&4)
	{
		upDc=GetDlgItem(IDC_LEFT)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_LEFT)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(255,0,0));
	}
	else
	{
		upDc=GetDlgItem(IDC_LEFT)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_LEFT)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(128,128,128));
	}
	if(lightnum&8)
	{
		upDc=GetDlgItem(IDC_RIGHT)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_RIGHT)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(255,0,0));
	}
	else
	{
		upDc=GetDlgItem(IDC_RIGHT)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_RIGHT)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(128,128,128));
	}
	if(lightnum&16)
	{
		upDc=GetDlgItem(IDC_FORWARD)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_FORWARD)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(255,0,0));
	}
	else
	{
		upDc=GetDlgItem(IDC_FORWARD)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_FORWARD)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(128,128,128));
	}
	if(lightnum&32)
	{
		upDc=GetDlgItem(IDC_BACKWARD)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_BACKWARD)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(255,0,0));
	}
	else
	{
		upDc=GetDlgItem(IDC_BACKWARD)->GetDC();
		dc.Attach(upDc->GetSafeHdc());
		GetDlgItem(IDC_BACKWARD)->GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(128,128,128));
	}
}


void CcamMFCDlg::OnBnClickedGetmodelhx()     //获取已知环形目标模板
{
	// TODO: 在此添加控件通知处理程序代码
	Flag_imT=5;
/*	IplImage *HXimT;
	HXimT=cvLoadImage("HXimT.jpg",1);
	if(HXimT==NULL)
	{
		m_statelist=m_statelist+CString("环形模板不存在！\r\n");
		UpdateData(FALSE);
	}
	else
	{
		CDC* pDC_imT=GetDlgItem(IDC_IMT)->GetDC();// 获得显示控件的 DC  
		HDC hDC_imT=pDC_imT->GetSafeHdc();// 获取 HDC(设备句柄) 来进行绘图操作  
		CRect rect_imT;
		GetDlgItem(IDC_IMT)->GetClientRect(&rect_imT);  
		CvvImage m_CvvImage;
		m_CvvImage.CopyOf(HXimT,1);
		m_CvvImage.DrawToHDC(hDC_imT,&rect_imT);
		m_statelist=m_statelist+CString("开始跟踪模板目标！\r\n");
		UpdateData(FALSE);
	}
*/
}
void CcamMFCDlg::OnBnClickedButton3()     //清空状态显示区
{
	// TODO: 在此添加控件通知处理程序代码
	m_statelist="";
	UpdateData(FALSE);
}
void CcamMFCDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(0);
	CThreshold th;
	th.m_threshold=m_thread;
	if(th.DoModal()==IDOK)
	{
		m_thread=th.m_threshold;
		UpdateData(false);
	}
	SetTimer(0,40,NULL);
}
/*			for(i=img_point.y-64;i<=img_point.y+64;i++)
			{
				j=img_point.x-64;
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+0] = 0;// B  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+1] = 0;// G  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+2] = 255;// R  
			}
			for(i=img_point.y-64;i<=img_point.y+64;i++)
			{
				j=img_point.x+64;
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+0] = 0;// B  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+1] = 0;// G  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+2] = 255;// R  
			}
			for(j=(img_point.x-64);j<=(img_point.x+64);j++)
			{
				i=img_point.y-64;
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+0] = 0;// B  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+1] = 0;// G  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+2] = 255;// R  
			}
			for(j=(img_point.x-64);j<=(img_point.x+64);j++)
			{
				i=img_point.y+64;
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+0] = 0;// B  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+1] = 0;// G  
				((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+2] = 255;// R  
			}
*/
	/*	for(i=img_point.y+64,ii=127;i>img_point.y-64;i--,ii--)
			for(j=(img_point.x-64),jj=0;j<(img_point.x+64);j++,jj++)
			{
				((uchar *)(imT->imageData+ii*imT->widthStep))[jj*imT->nChannels+0] =  ((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+0]; 
				((uchar *)(imT->imageData+ii*imT->widthStep))[jj*imT->nChannels+1] =  ((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+1]; 
				((uchar *)(imT->imageData+ii*imT->widthStep))[jj*imT->nChannels+2] =  ((uchar *)(m_Frame->imageData+i*m_Frame->widthStep))[j*m_Frame->nChannels+2];
			}
			*/

/*	char ss[256];
	sprintf(ss,"%d %d",img_point.x,img_point.y);
	m_static=ss;
	UpdateData(FALSE);
	*/


