# OpenCodeModule

<br/>

### 실행 환경 

OS : Windows10 64bit <br/>
IDE : Visual Studio 2017 <br/>
Graphic card : Gforce 1050ti cuda 9.0

<br/>

### 예제 실행에 필요한 라이브러리 

 ***주의사항 : 예제를 실행하기위해 아래의 라이브러리들이 필요합니다.*** <br/>
 
<a href="https://drive.google.com/open?id=1rdVEwMDkRjl8mSaTEb2OnKrEZqVz7_EY" target="__blank">opencv 라이브러리 다운로드</a> <br/>
<a href="https://drive.google.com/open?id=1vo4slHsubTmgeCDA1gLAs9sh-0xAz7dn" target="__blank">tensorflow c api 라이브러리 다운로드 (pre-builted gpu cuda 9.0 version)</a> 

<br/>

<br/>

### OpenCodeModule 이란? 
OpenCodeModule은 LoadLibrary 함수를 이용하여 단일 모듈(하나의 기능)을 Addon처럼 쉽게 프로그램에 붙이거나 떨어뜨릴수 있도록 설계된 Visual Studio 프로젝트입니다. 이 단일 모듈 dll은 3개의 함수 ***initialize(초기화), run(실행), dispose(자원해제)*** 로 이루어져 있으며 언급한 목적에 맞게 사용자가 함수 내부에서 코드를 작성하시면됩니다.

``` c++

namespace oc {
	class omodule : oc::Imodule {

	private:

	public:

		omodule() {
		}

		~omodule() {
		}

		variant run(variant &inputs) override;          /// 초기화용으로 미리 선언된 함수
		variant initialize(variant &inputs) override;   /// 실행용으로 미리 선언된 함수
		variant dispose() override;                     /// 자원해제용으로 미리 선언된 함수
	};
}
```

> ***Github에 있는 OpenCodeModule 폴더를 참고하시기 바랍니다.***  <br/>
> 1. OpenCodeModule 프로젝트를 빌드 시 dll이 생성됨.
> 2. ***initialize(초기화), run(실행), dispose(자원해제)*** 함수 3개만을 가지고 있다. 

<br/>

### variant 변수 사용법
OpenCodeModule은 파라메터 전달을 위해 여러개의 매개변수를 사용하지 않습니다. 특별하게 만들어진 variant 변수하나로 형이 다른 여러개의 변수를 전달할 수 있습니다. 
아래의 예는 variant 변수를 통해 파라메터를 전달하는 예 입니다.

``` c++

/// variant 에 파라메터 전달. 
oc::variant input;
input << "animal-darknet19.pb" << 100 << 100 << 3 ;


------------------------------------

/// variant 로 부터 파라메터 추출 예 1.

const char * _model_path = nullptr;
int _image_width;
int _image_height;
int _image_channel;

try {
	inputs >> _model_path;
	inputs >> _image_width;
	inputs >> _image_height;
	inputs >> _image_channel;
}
catch (std::runtime_error e) {
	std::cout << e.what() << std::endl;
}


/// variant 로 부터 파라메터 추출 예 2.

const char * _model_path = nullptr;
int _image_width;
int _image_height;
int _image_channel;

try {
	_model_path = inputs[0].get<const char *>()
	_image_width = inputs[1].get<int>()
	_image_height = inputs[2].get<int>()
	_image_channel = inputs[3].get<int>()
}
catch (std::runtime_error e) {
	std::cout << e.what() << std::endl;
}

```

<br/>

### oloader 클래스를 통한 모듈 로드 및 실행
위에서 설명한 OpenCodeModule 프로젝트를 빌드하면 결과로 dll이 만들어지고 이를 아래의 방법으로 로드하여 사용할 수 있습니다. dll을  사용하는 쪽에서는 특별한 .lib파일이 필요없이 oc::oloader 클래스만을 이용하여 dll을 로드 및 사용할 수 있습니다. 

``` c++

// loader 변수 선언 및 dll경로를 통한 모듈로드
oc::oloader loader;
loader.load("d://Test.dll", oc::oloader::MODULE_TYPE::__DLL)

// 필요에 따라 필요한 값들을 variant 변수를 통해 전달.
oc::variant input1;
oc::variant input2;
oc::variant input3;

//함수 호출
loader.initialize(input1);
loader.run(input2);
loader.dispose(input3);

```

<br/>

### 예제 1 : Tensorflow C api를 이용한 동물 분류
tensorflow c_api를 OpenCodeModule dll로 만들어 동물 사진을 분류하는 예입니다. tensorflow python을 이용하여 모델 데이터를 만든다음에 이를 로드하여 사용하는 예입니다. (5가지의 종류를 분류) <br/>


***darknet19 기반 모델 python 코드*** <br/>
 <a href="https://github.com/gellston/DeepLearningExamples/blob/master/models/model_darknet19.py" target="__blank">Github 소스코드</a> 
 
***예제 실행을 위해 /models/animal-darknet19-compress.pb 학습된 모델 파일 필요*** <br/>
***예제 실행을 위해 dll 컴파일 혹은 미리 컴파일 된 /dll/animal-darknet19-compress.dll 파일 필요*** <br/>


``` c++

#include <opencv2/opencv.hpp>
#include <oloader.h>

int main()
{

	oc::oloader animal_classifier;
	animal_classifier.load(L"animal-darknet19-compress.dll", oc::oloader::MODULE_TYPE::__DLL); /// 모델을 로드할수있는 dll 


	cv::Mat image = cv::imread("D:\\OpenCodeModuleDeep\\ConsoleExample\\x64\\Debug\\1_1_0_cat.jpg");  /// 이미지 경로.
	cv::Mat convert;
	image.convertTo(convert, CV_32F);


	
	oc::variant initParam;
	initParam << (const char *)"D:\\OpenCodeModuleDeep\\ConsoleExample\\x64\\Debug\\animal-darknet19-compress.pb";  /// 모델 경로.
	initParam << convert.cols;  // 이미지 너비
	initParam << convert.rows;  // 이미지 높이
	initParam << 3;             // 이미지 채널.


	oc::variant imageParam; 
	bool phase = false;        
	imageParam << (float *)convert.data;   /// image pointer
	imageParam << &phase;                  /// Batch normalization turn off


	try {
		oc::variant init_output = animal_classifier.initialize(initParam);
        oc::variant run_output = animal_classifier.run(imageParam);
		std::cout << "message : " << run_output[0].get<const char *>() << std::endl;
		std::cout << "score list :" << run_output[1].get<float>() << std::endl;
		std::cout << "score list :" << run_output[2].get<float>() << std::endl;
		std::cout << "score list :" << run_output[3].get<float>() << std::endl;
		std::cout << "score list :" << run_output[4].get<float>() << std::endl;
		std::cout << "score list :" << run_output[5].get<float>() << std::endl;
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
	}
	
	cv::imshow("input image", image);
	cv::waitKey();

    return 0;
}

```
<center>

[![IMAGE ALT TEXT HERE](https://github.com/gellston/OpenCodeModule/blob/master/preview/preview1.png?raw=true)](https://www.youtube.com/watch?v=8uM_vKebD6Q)
 -실행화면-
 </center>


<br/>

### 예제 2 : Tensorflow C api를 Portrait Segmentation
tensorflow c_api를 OpenCodeModule dll로 만들어 사람의 상반신 부분을 segmentaion하는 예제입니다. <br/>


 ***densenet 기반 모델 python 코드*** <br/>
 <a href="https://github.com/gellston/DeepLearningExamples/blob/master/models/model_custom_densenet_segmentation_v1.py" target="__blank">Github 소스코드</a> 
 
  ***예제 실행을 위해 /models/densenet_segmentation-compress.pb 학습된 모델 파일 필요*** <br/>
  ***예제 실행을 위해 dll 컴파일 혹은 미리 컴파일 된 /dll/densenet_segmentation_segmentation-compress.dll 파일 필요*** <br/>


``` c++

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
	person_segmentation.load(L"densenet_segmentation-compress.dll", oc::oloader::MODULE_TYPE::__DLL); /// 모델을 로드할수있는 dll 


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

```
<center>

[![IMAGE ALT TEXT HERE](https://github.com/gellston/OpenCodeModule/blob/master/preview/preview2.png?raw=true)](https://www.youtube.com/watch?v=BRWIqOLkSNw)
 -실행화면-
 </center>

<br/>

### 예제 3 : Tensorflow C api를 Fast Portrait Segmentation for mobile
tensorflow c_api를 OpenCodeModule dll로 만들어 사람의 상반신 부분을 segmentaion하는 예제입니다. <br/>


 ***residual 기반 모델 python 코드*** <br/>
 <a href="https://github.com/gellston/DeepLearningExamples/blob/master/models/model_custom_mobile_segmentation_v3.py" target="__blank">Github 소스코드</a> 
 
  ***예제 실행을 위해 /models/mobile_person_segmentation-compress.pb 학습된 모델 파일 필요*** <br/>
  ***예제 실행을 위해 dll 컴파일 혹은 미리 컴파일 된 /dll/mobile_person_segmentation_v3.dll 파일 필요*** <br/>


``` c++

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


```
<center>

[![IMAGE ALT TEXT HERE](https://github.com/gellston/OpenCodeModule/blob/master/preview/preview3.png?raw=true)](https://www.youtube.com/watch?v=Otf86HV6fpk)
 -실행화면-
 </center>

<br/>