// camMFCDlg.cpp : ʵ���ļ�
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
IplImage* imTGray;  //ģ��ͼ��ĻҶ�ͼ
IplImage* imSGray;  //ʵʱͼ��ĻҶ�ͼ
IplImage* Result;   //ģ��ƥ����ͼ
int T_W=128;        //ģ��ͼ�ߴ�
int T_H=128;

CRect rect,rect_w;
CDC* pDC;
HDC hDC;
CPoint img_point;   //�����ͼ���е�λ�á�
bool Flag_init=false;

double min_val;   //ģ��ƥ��
double max_val;
CvPoint min_loc;
CvPoint max_loc;
char light_on_num=0;   //ָʾ�����ı��

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CcamMFCDlg �Ի���




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


// CcamMFCDlg ��Ϣ�������

BOOL CcamMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_GetModel)->EnableWindow(false);
	GetDlgItem(IDC_GetModelHX)->EnableWindow(false);
	m_statelist="��һ����ѡ������ͷ���򿪡�\r\n";
	UpdateData(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CcamMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CcamMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CcamMFCDlg::OnBnClickedButton1()   //������ͷ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//VideoCapture capture(0);//����VideoCapture����
	UpdateData(true);
	capture.open(_ttoi(m_camnum));
	if (!capture.isOpened())//�ж��Ƿ������ͷ
	{
		m_statelist=m_statelist+CString("û�з�������ͷ��\r\n");
		UpdateData(false);
	}
	else
    {
		Flag_init=true;
		m_statelist=m_statelist+CString("����ͷ��������\r\n");
		m_statelist=m_statelist+CString("�ڶ�����ѡ��ģ�塣\r\n");
		GetDlgItem(IDC_GetModel)->EnableWindow(true);
		GetDlgItem(IDC_GetModelHX)->EnableWindow(true);
		UpdateData(false);

		//SetTimer(0,40,NULL);
	} 
	SetTimer(0,40,NULL);
}

void CcamMFCDlg::OnBnClickedButton2()    //�ر�����ͷ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!capture.read(frame))//��ȡing
    {
		m_static="��ͼ��";
		UpdateData(FALSE);
    }
	else
	{
		IplImage image(frame);
		IplImage* m_Frame;
		m_Frame=&image;
		imSGray = cvCreateImage(cvGetSize(m_Frame),IPL_DEPTH_8U, 1);    //  323
		cvCvtColor(m_Frame,imSGray,CV_BGR2GRAY);  //��ʵʱͼ��תΪ�Ҷ�ͼ   323
		if(Flag_init)
		{
			pDC=GetDlgItem(IDC_IMAGE)->GetDC();// �����ʾ�ؼ��� DC  //PDC DC HDC ? ?
			hDC=pDC->GetSafeHdc();// ��ȡ HDC(�豸���) �����л�ͼ����   
			GetDlgItem(IDC_IMAGE)->GetClientRect(&rect);    //GetClientRect, GetWindowRect
			GetDlgItem(IDC_IMAGE)->GetWindowRect(&rect_w);  //ͼƬ�ؼ��ڴ����е�λ��
			ScreenToClient(&rect_w);   //���û��˵����Ͻ�Ϊԭ��
			int rw=rect.right-rect.left;    // ���picture control�Ŀ�͸�  
			int rh=rect.bottom-rect.top; 
			int iw=frame.cols;  
			int ih=frame.rows;  
			int tx = (int)(rw - iw)/2;          // ʹ��Ƶ֡����ʾλ�������ڿؼ�������
			int ty = (int)(rh - ih)/2;  
			SetRect( rect, tx, ty, tx+iw, ty+ih ); //����ROI���� 
			rect_w.left=rect_w.left+tx;  //
			rect_w.top=rect_w.top+ty;
			Flag_init=false;
		}
		if(Flag_imT==1)   //ѡ��ģ�壬�����
		{
			img_point.x=mouse_point.x-rect_w.left;//�����rect_window ͼ���е�λ��
			img_point.y=mouse_point.y-rect_w.top;
			if (img_point.x-64>=0 && img_point.y-64>=0 && img_point.x+64<640 && img_point.y+64<480) 
			{
				cvRectangle(m_Frame,cvPoint(img_point.x-64,img_point.y-64),cvPoint(img_point.x+64,img_point.y+64),cvScalar(0,0,255),1,8,0);   //323
			}
		}
		else if(Flag_imT==2)    //��ʾģ��
		{
			imT = cvCreateImage(cvSize(T_H,T_W), IPL_DEPTH_8U, 3); // IPL_DEPTH_8U ���ͼ 8U
			cvSetImageROI(m_Frame,cvRect( img_point.x-64,img_point.y-64 ,T_W,T_H));
			imT = cvCloneImage(m_Frame);
			cvResetImageROI(m_Frame);
			imTGray = cvCreateImage(cvGetSize(imT),IPL_DEPTH_8U, 1);  //  323
			cvCvtColor(imT,imTGray,CV_BGR2GRAY);                      //  323
			Flag_imT=3;
		}
		else if(Flag_imT==3)   //
		{
			CDC* pDC_imT=GetDlgItem(IDC_IMT)->GetDC();// �����ʾ�ؼ��� DC  
			HDC hDC_imT=pDC_imT->GetSafeHdc();// ��ȡ HDC(�豸���) �����л�ͼ����  
			CRect rect_imT;
			GetDlgItem(IDC_IMT)->GetClientRect(&rect_imT); 
			CvvImage cimg_imT;
			cimg_imT.CopyOf(imT);    // ����ͼƬ  
			cimg_imT.DrawToHDC(hDC_imT,&rect_imT);  
			ReleaseDC(pDC_imT);
			Result = cvCreateImage(cvSize(imSGray->width-imTGray->width+1,imSGray->height-imTGray->height+1),IPL_DEPTH_32F,1);
			m_statelist=m_statelist+CString("��ʼ����ģ��Ŀ�꣡\r\n");
			UpdateData(FALSE);
			Flag_imT=4;
		}
		else if(Flag_imT==4)
		{
			cvMatchTemplate(imSGray,imTGray,Result,CV_TM_CCORR_NORMED);   //image��ݔ��D����횞� 8λԪ���c���D��
			//templ��ݔ���template���ߴ���С�ݔ��D���ΑB����ͬ��
				//result�����^�Y���D����횞��ͨ��32λԪ���c���D�����image�ĳߴ��W x H��templ�ĳߴ��w x h���tresult�ĳߴ��(W - w + 1)x(H - h + 1)��
				//method�����^���������������N�������x��Ŀ������㷨  CV_TM_SQDIFF_NORMED ��һ��ƽ����ƥ�䷨    CV_TM_CCORR_NORMED  ��һ�����ƥ�䷨
			cvMinMaxLoc(Result,&min_val,&max_val,&min_loc,&max_loc,NULL);   // src��ݔ��D��
			    //minVal���OСֵ����ݔ��NULL��ʾ����Ҫ��
				//maxVal ���O��ֵ����ݔ��NULL��ʾ����Ҫ��
				//minLoc���OСֵ��λ�ã���ݔ��NULL��ʾ����Ҫ��
				//maxLoc���O��ֵ��λ�ã���ݔ��NULL��ʾ����Ҫ��
				//mask�����пɟo�����֡�
			if (max_val>m_thread)
			{
				cvRectangle(m_Frame,cvPoint(max_loc.x,max_loc.y),cvPoint(max_loc.x+T_W-1,max_loc.y+T_H-1),cvScalar(0,0,255),1,8,0);
				m_static="Ŀ�����������";
				m_value=max_val;   //ƥ��ֵ

				m_x=max_loc.x+64;      //Ŀ����ͼ���е�����// Ӧ����min_loc.x min_loc.y  ���û�������� +32 +32
				m_y=max_loc.y+64;
				m_xc=m_x-320;          //Ŀ�������ͼ�����ĵ�����
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
				m_static="Ŀ������쳣��";
				m_value=max_val;   //ƥ��ֵ
				UpdateData(false);
			}
		}
		else if(Flag_imT==5)    //�������Բ��
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
			m_static="Բ�����٣�";
			m_value=cir_num;   //ƥ��ֵ
			UpdateData(false);
		}

		CvvImage cimg;
		cimg.CopyOf(m_Frame);    // ����ͼƬ  
		cimg.DrawToHDC(hDC,&rect);  

		cvReleaseImage(&imSGray);
	}
	Light_on(light_on_num);              //ָʾ����ʾ

	CDialog::OnTimer(nIDEvent);
}
void CcamMFCDlg::OnBnClickedGetmodel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Flag_imT=1;
	if(imTGray!=NULL)
		cvReleaseImage(&imTGray);
	if(imT!=NULL)
		cvReleaseImage(&imT);
}

void CcamMFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	mouse_point=point;
	if(Flag_imT==1)
		Flag_imT=2;
	CDialog::OnLButtonDown(nFlags, point);
}

void CcamMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	mouse_point=point;
	CDialog::OnMouseMove(nFlags, point);
}
void CcamMFCDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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


void CcamMFCDlg::OnBnClickedGetmodelhx()     //��ȡ��֪����Ŀ��ģ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Flag_imT=5;
/*	IplImage *HXimT;
	HXimT=cvLoadImage("HXimT.jpg",1);
	if(HXimT==NULL)
	{
		m_statelist=m_statelist+CString("����ģ�岻���ڣ�\r\n");
		UpdateData(FALSE);
	}
	else
	{
		CDC* pDC_imT=GetDlgItem(IDC_IMT)->GetDC();// �����ʾ�ؼ��� DC  
		HDC hDC_imT=pDC_imT->GetSafeHdc();// ��ȡ HDC(�豸���) �����л�ͼ����  
		CRect rect_imT;
		GetDlgItem(IDC_IMT)->GetClientRect(&rect_imT);  
		CvvImage m_CvvImage;
		m_CvvImage.CopyOf(HXimT,1);
		m_CvvImage.DrawToHDC(hDC_imT,&rect_imT);
		m_statelist=m_statelist+CString("��ʼ����ģ��Ŀ�꣡\r\n");
		UpdateData(FALSE);
	}
*/
}
void CcamMFCDlg::OnBnClickedButton3()     //���״̬��ʾ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_statelist="";
	UpdateData(FALSE);
}
void CcamMFCDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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


