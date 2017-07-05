//
//  main.cpp
//  CVtest
//
//  Created by 胡耀楠 on 2017/7/5.
//  Copyright © 2017年 胡耀楠. All rights reserved.
//

/***************************************************************
 *
 *   内容摘要：分别用两种方法对输入图像进行拉普拉斯算子锐化，并比较
 *             两种方法的执行效率，从运行结果来看，第二种方法的效率
 *             更高。
 *   作    者：刘军（Jacky Liu）
 *   完成日期：2012.8.12
 *   参考资料：opencv 2 computer vision application programming
 *            cookbook
 *
 ***************************************************************/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

//输入形参为pass by conference-const，保证输入的图像不会被修改，并且为传递引用
void sharpenImage0(const cv::Mat &image, cv::Mat &result)
{
    //为输出图像分配内存
    result.create(image.size(),image.type());
    
    /*滤波核为拉普拉斯核3x3：
     0 -1 0
     -1 5 -1
     0 -1 0
     */
    for(int j= 1; j<image.rows-1; ++j)
    {
        const uchar *previous = image.ptr<const uchar>(j-1);
        const uchar *current = image.ptr<const uchar>(j);
        const uchar *next = image.ptr<const uchar>(j+1);
        uchar *output = result.ptr<uchar>(j);
        for(int i= 1; i<image.cols-1; ++i)
        {
            *output++ = cv::saturate_cast<uchar>(5*current[i]-previous[i]-next[i]-current[i-1]-current[i+1]);  //saturate_cast<uchar>()保证结果在uchar范围内
        }
    }
    result.row(0).setTo(cv::Scalar(0));
    result.row(result.rows-1).setTo(cv::Scalar(0));
    result.col(0).setTo(cv::Scalar(0));
    result.col(result.cols-1).setTo(cv::Scalar(0));
}

void sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
    //创建并初始化滤波模板
    cv::Mat kernel(3,3,CV_32F,cv::Scalar(0));
    kernel.at<float>(1,1) = 5.0;
    kernel.at<float>(0,1) = -1.0;
    kernel.at<float>(1,0) = -1.0;
    kernel.at<float>(1,2) = -1.0;
    kernel.at<float>(2,1) = -1.0;
    
    result.create(image.size(),image.type());
    
    //对图像进行滤波
    cv::filter2D(image,result,image.depth(),kernel);
}

int main(int argc, char* argv[])
{
    std::cout<<"start"<<std::endl;
    cv::Mat image = cv::imread("/Users/huyaonan/opencv/pics/bear.jpg");
    cv::Mat image_gray;
    image_gray.create(image.size(),image.type());
    
    std::cout<<"load image"<<image.size()<<std::endl;
    
    if(!image.data)
        return -1;
    if(image.channels() == 3)
        cv::cvtColor(image,image_gray,CV_RGB2GRAY);
    
    std::cout<<"prepare to parse image"<<std::endl;
    
    cv::Mat result;
    result.create(image_gray.size(),image_gray.type());
    double time_ = static_cast<double>(cv::getTickCount());
    sharpenImage0(image_gray,result);
    time_ = 1000*(static_cast<double>(cv::getTickCount())-time_)/cv::getTickFrequency();
    std::cout<<"time = "<<time_<<"ms"<<std::endl;
    
    cv::namedWindow("Image 1");
    cv::imshow("Image 1",result);
    
    cv::Mat result1;
    result1.create(image_gray.size(),image_gray.type());
    time_ = static_cast<double>(cv::getTickCount());
    sharpenImage1(image_gray,result1);
    time_ = 1000*static_cast<double>(cv::getTickCount()-time_)/cv::getTickFrequency();
    std::cout<<"time = "<<time_<<"ms"<<std::endl;
    
    cv::namedWindow("Image 2");
    cv::imshow("Image 2",result1);
    
    cv::waitKey();
    return 0;
}
