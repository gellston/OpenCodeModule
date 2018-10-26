# OpenCodeModule

<br/>

### 실행 환경 

OS : Windows10 64bit <br/>
IDE : Visual Studio 2017 <br/>
Graphic card : Gforce 1050ti cuda 9.0

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

### 예제 1 : Tensorflow C api 를 이용하여 동물 분류 모델을 로드 및  분류하는 모듈 예. 
tensorflow c_api를 OpenCodeModule dll로 만들어 동물 사진을 분류하는 예입니다. tensorflow python을 이용하여 모델 데이터를 만든다음에 이를 로드하여 사용하는 예입니다. (5가지의 종류를 분류) ***아래 동영상 참고, OpenCodeModule Visual Studio 프로젝트 참고.*** <br/>

 ***주의사항 : 예제를 실행하기위해 아래의 라이브러리들이 필요*** <br/>
<a href="https://drive.google.com/open?id=1rdVEwMDkRjl8mSaTEb2OnKrEZqVz7_EY" target="__blank">opencv 라이브러리 다운로드</a> <br/>
<a href="https://drive.google.com/open?id=1vo4slHsubTmgeCDA1gLAs9sh-0xAz7dn" target="__blank">tensorflow c api 라이브러리 다운로드 (pre-builted gpu cuda 9.0 version)</a> 
<br/>


 ***darknet19 기반 분류기 python 코드*** <br/>
 <a href="https://github.com/gellston/DeepLearningExamples/blob/master/models/model_darknet19.py" target="__blank">Github 소스코드</a> 

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
 darknet19 네트워크로 트레이닝된 모델 animal-darknet19.pb를 로드하여 실행하는 모습
 </center>