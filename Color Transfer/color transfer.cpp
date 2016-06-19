#include<cv.h>
#include<highgui.h>
#include<math.h>
#include<iostream>

int colorTransfer(IplImage *img1,IplImage *img2,IplImage *&dst);

int main()
{
    cvNamedWindow("原始图片");
    cvNamedWindow("目标色彩");
    cvNamedWindow("颜色转移结果");
    IplImage * img1,*img2,*dst;

    img1=cvLoadImage("a.jpg");
    img2=cvLoadImage("b.jpg");
    cvShowImage("原始图片",img1);
    cvShowImage("目标色彩",img2);
    dst=cvCloneImage(img1);
    colorTransfer(img1,img2,dst);
    cvShowImage("颜色转移结果",dst);
    cvWaitKey(0);
    cvDestroyAllWindows();
}


int colorTransfer(IplImage *img1,IplImage *img2,IplImage *&dst)
{
    double sum=0;
    double m1[3],m2[3],d1[3],d2[3];
    int i,x,y;
    //IplImage *src1,*src2;
    cvCvtColor(img1,img1,CV_BGR2Lab);
    cvCvtColor(img2,img2,CV_BGR2Lab);
    IplImage *src1=cvCloneImage(img1),*src2=cvCloneImage(img2);
    int npixs1=src1->width*src1->height;
    int npixs2=src2->width*src2->height;

    //计算各个通道的均值和方差
    for(i=0;i<3;i++)
    {
        m1[i]=0;
        for(x=0;x<src1->height;x++)
        {
            uchar *ptr=(uchar*)(src1->imageData+x*src1->widthStep);
            for(y=0;y<src1->width;y++)
            {
                m1[i]+=ptr[3*y+i];
            }
        }
        m1[i]/=npixs1;
    }
    for(i=0;i<3;i++)
    {
        d1[i]=0;
        for(x=0;x<src1->height;x++)
        {
            uchar *ptr=(uchar*)(src1->imageData+x*src1->widthStep);
            for(y=0;y<src1->width;y++)
            {
                d1[i]+=(ptr[3*y+i]-m1[i])*(ptr[3*y+i]-m1[i]);
            }
        }
        d1[i]/=npixs1;
        d1[i]=sqrt(d1[i]);
    }
    for(i=0;i<3;i++)
    {
        m2[i]=0;
        for(x=0;x<src2->height;x++)
        {
            uchar *ptr=(uchar*)(src2->imageData+x*src2->widthStep);
            for(y=0;y<src2->width;y++)
            {
                m2[i]+=ptr[3*y+i];
            }
        }
        m2[i]/=npixs2;
    }
    for(i=0;i<3;i++)
    {
        d2[i]=0;
        for(x=0;x<src2->height;x++)
        {
            uchar *ptr=(uchar*)(src2->imageData+x*src2->widthStep);
            for(y=0;y<src2->width;y++)
            {
                d2[i]+=(ptr[3*y+i]-m2[i])*(ptr[3*y+i]-m2[i]);
            }
        }
        d2[i]/=npixs2;
        d2[i]=sqrt(d2[i]);
    }
    
    //对原图像做变换
    double rate[3];
    for(i=0;i<3;i++)
    {
        rate[i]=d2[i]/d1[i];
        for(x=0;x<src1->height;x++)
        {
            uchar *ptr=(uchar*)(src1->imageData+x*src1->widthStep);
            for(y=0;y<src1->width;y++)
            {
                double tmp=ptr[3*y+i];
                int t=(int)((tmp-m1[i])*rate[i]+m2[i]);
                if(t<0)                //对越界像素值进行调整
                {
                    t=0;
                }
                if(t>255)
               {
                   t=255;
                }
               ptr[3*y+i]=t;
                
            }
        }
    }
     cvCvtColor(src1,dst,CV_Lab2BGR);
    //dst=cvCloneImage(src1);
    return 0;
    
}


