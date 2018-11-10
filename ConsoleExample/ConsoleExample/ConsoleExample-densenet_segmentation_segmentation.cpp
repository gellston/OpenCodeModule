#include <opencv2/opencv.hpp>
#include <oloader.h>

#include <string>
#include <Windows.h>

std::vector<std::string> get_all_files_names_within_folder(std::string folder)
{
	std::vector<std::string> names;
	std::string search_path = folder + "//*.jpg";
	WIN32_FIND_DATAA fd;
	
	HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

int main()
{

	oc::oloader person_segmentation;
	person_segmentation.load(L"densenet_segmentation_segmentation-compress.dll", oc::oloader::MODULE_TYPE::__DLL); /// 모델을 로드할수있는 dll 


	cv::Mat image = cv::imread("D:\\OpenCodeModule\\ConsoleExample\\x64\\Release\\images\\00001.jpg");  /// 이미지 경로.
	cv::Mat convert;
	image.convertTo(convert, CV_32F);
	


	oc::variant initParam;
	initParam << (const char *)"D:\\OpenCodeModule\\ConsoleExample\\x64\\Release\\densenet_segmentation-compress.pb";  /// 모델 경로.
	initParam << convert.cols;  // 이미지 너비
	initParam << convert.rows;  // 이미지 높이
	initParam << 3;             // 이미지 채널

	try {
		oc::variant init_output = person_segmentation.initialize(initParam);

	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
	}


	
	std::vector<std::string> filenames = get_all_files_names_within_folder("D://OpenCodeModule//ConsoleExample//x64//Release//images");
	std::cout << "it will start segmentation !!" << std::endl;
	cv::waitKey();

	for (int index = 0; index < filenames.size(); index++) {
		try {

			std::string filename = "D:\\OpenCodeModule\\ConsoleExample\\x64\\Release\\images\\";
			filename += filenames[index];
			cv::Mat image = cv::imread(filename.c_str());  /// 이미지 경로.
			cv::Mat convert;
			image.convertTo(convert, CV_32F);
			cv::Mat output(256, 256, CV_32F, cv::Scalar(0, 0, 0));

			cv::Mat resize_input(512, 512, CV_32F, cv::Scalar(0, 0, 0));
			cv::Mat resize_output(512, 512, CV_32F, cv::Scalar(0, 0, 0));

			oc::variant imageParam;
			bool phase = false;
			imageParam << (float *)convert.data;   /// image pointer
			imageParam << (float *)output.data;
			imageParam << phase;                  /// Batch normalization turn off
			oc::variant run_output = person_segmentation.run(imageParam);
			std::cout << "message : " << run_output[0].get<const char *>() << std::endl;


			cv::resize(image, resize_input, cv::Size(512, 512), 0, 0, cv::INTER_CUBIC);
			cv::resize(output, resize_output, cv::Size(512, 512), 0, 0, cv::INTER_CUBIC);
			cv::Mat thres_output;

			cv::threshold(resize_output, thres_output, 0.5, 255, cv::THRESH_BINARY);

			cv::imshow("input image", resize_input);
			cv::imshow("output image", resize_output);
			cv::imshow("thres image", thres_output);
			cv::waitKey(30);
		}
		catch (std::runtime_error e) {
			std::cout << e.what() << std::endl;
		}
	}

	cv::waitKey();

	return 0;
}