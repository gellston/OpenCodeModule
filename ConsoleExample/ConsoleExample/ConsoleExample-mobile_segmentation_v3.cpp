#include <opencv2/opencv.hpp>
#include <oloader.h>
#include <string>
#include <Windows.h>

int main()
{
	
	oc::oloader person_segmentation;
	person_segmentation.load(L"mobile_segmentation_v3.dll", oc::oloader::MODULE_TYPE::__DLL); /// 모델을 로드할수있는 dll 

	oc::variant initParam;
	initParam << (const char *)"D:\\OpenCodeModule\\ConsoleExample\\x64\\Release\\mobile_person_segmentation-compress.pb";  /// 모델 경로.
	initParam << 256;  // 이미지 너비
	initParam << 256;  // 이미지 높이
	initParam << 3;             // 이미지 채널

	try {
		oc::variant init_output = person_segmentation.initialize(initParam);
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
	}

	cv::VideoCapture cap("D://OpenCodeModule//ConsoleExample//x64//Release//stock-footage-close-up-of-charming-red-haired-girl-with-stylish-haircut-and-red-lips-smiling-and-making-faces-at.webm");

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
		return -1;
	}

	cv::namedWindow("detected face", CV_WINDOW_FREERATIO);
	cv::resizeWindow("detected face", cv::Size(512, 512));
	cv::namedWindow("thres_output", CV_WINDOW_FREERATIO);
	cv::resizeWindow("thres_output", cv::Size(512, 512));
	cv::namedWindow("crop", CV_WINDOW_FREERATIO);
	cv::resizeWindow("crop", cv::Size(512, 512));
	cv::waitKey();

	while (1) {
		cv::Mat frame;
		cap >> frame;
		if (frame.empty())
			break;

		cv::Rect myROI(100, 10, 300, 300);
		cv::Mat crop = frame(myROI);

		cv::Mat convert;
		crop.convertTo(convert, CV_32F);
		cv::Mat resizedInput;
		cv::resize(convert, resizedInput, cvSize(256, 256));
		cv::Mat output(256, 256, CV_32F, cv::Scalar(0, 0, 0));

		oc::variant imageParam;
		bool phase = false;
		imageParam << (float *)resizedInput.data;   /// image pointer
		imageParam << (float *)output.data;
		imageParam << phase;

		oc::variant run_output = person_segmentation.run(imageParam);

		cv::Mat thres_output;
		cv::threshold(output, thres_output, 0.5, 255, cv::THRESH_BINARY);

		// Display the resulting frame
		
		cv::imshow("detected face", output);
		cv::imshow("thres_output", thres_output);
		cv::imshow("crop", crop);
		cv::imshow("frame", frame);

		// Press  ESC on keyboard to exit
		char c = (char)cv::waitKey(1);
		if (c == 27)
			break;
	}
	// When everything done, release the video capture object
	cap.release();
	cv::waitKey();
	return 0;
}




