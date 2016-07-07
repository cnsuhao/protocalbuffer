
#include "Precise_Detect.h"

bool dotest()
{ 
    bool n=0;
    cout<<"video: 0\nPicture: 1"<<endl;
    cin>>n;
    char path[256]  ;
    char keyword1[] = "filename" ;
    char keyword2[] = "aviname" ;
    char target[256] ;
    vector<string> plates ;
    vector<string> color ;
    vector<Rect>   positions ;
    if (n != 0)
    {
        IniRead(keyword1 ,target ) ;
        strcpy(path, target) ;
    }
    else
    {
        IniRead(keyword2 ,target ) ;
        strcpy(path, target) ;
    }
    CarPlateRecognition( path,plates , color ,positions , n ) ;
    for ( unsigned int i = 0 ;i<positions.size();i++ )
    {
        printf("%d ,%d ,%d ,%d\n",positions[i].x ,positions[i].y ,
            positions[i].width ,positions[i].height);
    }

    return 0;
}

bool dotest1()
{
    Mat matSrc1 = imread("D:/Workspace/TCE/algorithm/image/cartmode/小轿车/chan08_20150523-113807-965_0_3_川A50Q20_0kph_CAR1.jpg");
    Mat matSrc2 = imread("D:/Workspace/TCE/algorithm/image/cartmode/小轿车/chan08_20150523-113322-075_0_3_渝GCN018_0kph_CAR1.jpg");
    Mat matSrc3 = imread("D:/Workspace/TCE/algorithm/image/cartmode/小轿车/chan08_20150523-114618-709_0_3_渝A62W22_0kph_CAR1.jpg");
    Mat matSrc4 = imread("D:/Workspace/TCE/algorithm/image/cartmode/小轿车/chan01_20150523-121205-654_0_1_渝AXB266_0kph_CAR1.jpg");
    Mat matSrc5 = imread("D:/Workspace/TCE/algorithm/image/cartmode/小轿车/chan01_20150523-115450-561_0_1_渝A911M9_0kph_CAR1.jpg");
    Mat matSrc6 = imread("D:/Workspace/TCE/algorithm/image/cartmode/小轿车/chan05_20150524-102628-310_0_2_琼A9XC10_38kph_CAR1.jpg");
	vector<string> plates ;
	vector<string> color ;
	vector<Rect>   positions ;

    InitCarPlateDetect("");

    DWORD dwCurentTime = GetTickCount();
    detect_pic(matSrc1, plates, color, positions);
    std::cout << (GetTickCount() - dwCurentTime) << "ms" << std::endl;
    plates.clear();
    color.clear();
    positions.clear();

    dwCurentTime = GetTickCount();
    detect_pic(matSrc2, plates, color, positions);
    std::cout << (GetTickCount() - dwCurentTime) << "ms" << std::endl;
    plates.clear();
    color.clear();
    positions.clear();

    dwCurentTime = GetTickCount();
    detect_pic(matSrc3, plates, color, positions);
    std::cout << (GetTickCount() - dwCurentTime) << "ms" << std::endl;
    plates.clear();
    color.clear();
    positions.clear();

    dwCurentTime = GetTickCount();
    detect_pic(matSrc4, plates, color, positions);
    std::cout << (GetTickCount() - dwCurentTime) << "ms" << std::endl;
    plates.clear();
    color.clear();
    positions.clear();

    dwCurentTime = GetTickCount();
    detect_pic(matSrc5, plates, color, positions);
    std::cout << (GetTickCount() - dwCurentTime) << "ms" << std::endl;
    plates.clear();
    color.clear();
    positions.clear();

    dwCurentTime = GetTickCount();
    detect_pic(matSrc6, plates, color, positions);
    std::cout << (GetTickCount() - dwCurentTime) << "ms" << std::endl;
    plates.clear();
    color.clear();
    positions.clear();

    FinishCarPlateDetect();
/*
    for (unsigned int i = 0 ;i<positions.size();i++ )
	{
		printf("%d ,%d ,%d ,%d\n",positions[i].x ,positions[i].y ,
			positions[i].width ,positions[i].height);
	}
*/
	return 0;


};


void main()
{
//dotest() ;
dotest1();
printf("HELLO WOORLD\n ");
//getchar();
}
