#define _CRT_SECURE_NO_WARNINGS

/*

#include <conio.h>
#include <graphics.h>

*/

#include <Windows.h>
#include "Message.h"
#include "SignalAnalyze.h"
#include "ImageGenerate.h"

#define WindowWidth 1280
#define WindowHeight 800
#define EdgeWidth 20
#define EdgeHeight 20
#define MaxInputHeight 900000 
#define BmpHeaderLength 54

/*
//for testing
float vertices[] = {
-0.5,-0.5,0.0,
0.5,-0.5,0.0,
0.0,0.5,0.0
};
*/


int DrawShape(char title[], int dSigIndex, int sSigIndex) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(WindowWidth, WindowHeight, title, NULL, NULL);
	if (window == NULL) {
		printf("%sFailed to create GLFW window", ErrorMsg);
		exit(EXIT_FAILURE);
	}

	//set context to current window
	glfwMakeContextCurrent(window);
	FILE* targetFileStream;
	char targetFileName[100];
	char outPutImagePath[100];
	sprintf(targetFileName, "CorrelationResult_Data%d_PSS%d.csv",dSigIndex, sSigIndex);
	
	sprintf(outPutImagePath, "CorrelationResult_Data%d_PSS%d.bmp",
		dSigIndex, sSigIndex);
	if ((targetFileStream = fopen(targetFileName, "r")) == NULL) {
		printf("%sFailed reading %s\n", WarningMsg, targetFileName);
		return -1;
	}

	//shrink display area inside bezel and remap all point x
	float xScale = (float)WindowWidth / (WindowWidth - EdgeWidth) / GetFileLenth(targetFileStream)*2;
	//shrink display area inside bezel and remap all point y
	float yScale = (double)WindowHeight / (WindowHeight - EdgeHeight) / MaxInputHeight*2;
	//printf("%f|%f\n", xScale, yScale);
	float correlationData[MaxLenOfDSig/2*3];
	for (int index = 0; index < MaxLenOfDSig/2; index+=3) {
		fscanf(targetFileStream, "%f", &correlationData[index+1]);
		correlationData[index] = (index - MaxLenOfDSig/4) / 3 * xScale*2;
		correlationData[index + 1] = (correlationData[index + 1]- (MaxInputHeight-100000)/2) * yScale;
		correlationData[index + 2] = (float)0;
		//printf("[%f,%f,%f]", correlationData[index], correlationData[index + 1], correlationData[index + 2]);

	}
	//init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("%sFailed to initialize GLAD", ErrorMsg);
	}

	//Prepare buffer
	//Create VBO and VAO object, give id 1
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//Bind VBO and VAO object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Create data area and get fill static area using correlationData[],use vertices to test
	glBufferData(GL_ARRAY_BUFFER, sizeof(correlationData), correlationData, GL_STATIC_DRAW);
	//Tell shader how to solve buffer info
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FASTEST, 3 * sizeof(float), (void*)0);
	//Enable VAO control 1st info
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	//render cycle
	while (!glfwWindowShouldClose(window))
	{
		//Set state
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//Use state
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(2.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, MaxLenOfDSig / 2);//use 3 to display triangle MaxLenOfDSig/2

		//switch butter and IO
		glfwSwapBuffers(window);
		glfwPollEvents();
		//if input ESC then exit
		glfwSetKeyCallback(window, KeyCallback);
	}

	glfwDestroyWindow(window);
	//free glfw resource
	glfwTerminate();
	return 0;
}


static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window,1);
}

static void ErrorCallback(int error, const char* description) {
	fprintf(stderr, "%s%s\n", ErrorMsg, description);
}



/*

#define TitleXPos EdgeWidth
#define TitleYPos EdgeHeight
#define TitleWidth WindowWidth-2*EdgeWidth
#define TitleHeight 40

#define BUFFERSIZE 100
wchar_t* TransformMBS(char c[BUFFERSIZE]) {
	wchar_t result[BUFFERSIZE];
	MultiByteToWideChar(CP_ACP, 0, c, -1, result, BUFFERSIZE);
	return result;
}

//Draw Line graph
int DrawShape(char title[], int dSigIndex, int sSigIndex) {
	FILE* targetFileStream;
	char str[10];
	char targetFileName[100];
	char outPutImagePath[100];
	//wchar_t* titleMBS= TransformMBS(title);

	sprintf(targetFileName, "CorrelationResult_Data%d_PSS%d.csv",
		dSigIndex + DSigFileNameOffset, sSigIndex + SSigFileNameOffset);
	sprintf(outPutImagePath, "./CorrelationResult_Data%d_PSS%d.png",
		dSigIndex + DSigFileNameOffset, sSigIndex + SSigFileNameOffset);
	if ((targetFileStream = fopen(targetFileName, "r")) == NULL) {
		printf("%sFailed reading %s", WarningMsg, targetFileName);
		return -1;
	}
	char correlationData[MaxLenOfDSig / 2+1];
	for(int index = 0; index < MaxLenOfDSig / 2; index++) {
		fscanf(targetFileStream, "%lf", &correlationData[index]);
	}



	initgraph(WindowWidth, WindowHeight, EX_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	setorigin(0, 0);
	settextcolor(LIGHTGRAY);
	setfont(30, 0, "SimHei");
	initgraph(1000, 600);//初始化窗口大小：x/y
	settextcolor(BLACK);//设置字体黑色
	setbkcolor(WHITE);//设置背景白色
	cleardevice();//清楚画布内容，使画布颜色为设置颜色
	setlinecolor(RGB(211, 211, 211));//设置线灰色
	for (int i = 0; i < 15; i++)//绘制x轴字符以及网格线
	{
		_itoa(i * 1000, str, 10);
		outtextxy(50 + 50 * i, 490, str);
		line(60 + 50 * (i + 1), 480, 60 + 50 * (i + 1), 50);//竖直方向网格线

	}
	for (int i = 1; i < 11; i++)//绘制y轴字符以及网格线
	{
		_itoa(i * 100000, str, 10);
		outtextxy(5, 475 - 40 * i, str);
		line(60, 480 - 40 * i, 810, 480 - 40 * i);//水平方向网格线
	}
	setlinecolor(GREEN);//设置线蓝色
	line(60, 45, 60, 480);//绘制y轴
	line(60, 480, 820, 480);//绘制x轴
	settextcolor(RGB(0, 191, 255));//设置字体蓝色
	setfont(24, 0, "Times New Roman");//设置字体样式
	outtextxy(20, 20, "y / Power");//绘制y轴单位字符
	outtextxy(820, 500, "x / Potion");//绘制x轴单位字符
	settextcolor(RGB(252, 35, 35));//设置字体红色
	outtextxy(350, 540, title);//绘制表格名

	setlinecolor(LIGHTBLUE);//设置线绿色
	for (int i = 0; i < MaxLenOfDSig/2; i++)//绘制折线
	{
		//x轴单位间隔是50,数值间隔是1000,故每个i之间x间隔是50/1000,即0.05;y轴单位间隔是40,数值间隔是100000,故每个i之间y间隔是40/100000,即0.0004
line(60 + 0.05 * i, (int)(480 - correlationData[i] * 0.0004), 60 + 0.05 * (i + 1), (int)(480 - correlationData[i + 1] * 0.0004));
	}
	saveimage(outPutImagePath);//保存图像

	//getch();//等待用户操作
	closegraph(); // 关闭图形界面
}



#define BUFFERSIZE 100
wchar_t* TransformMBS(char c[BUFFERSIZE]) {
	wchar_t result[BUFFERSIZE];
	MultiByteToWideChar(CP_ACP, 0, c, -1, result, BUFFERSIZE);
	return result;
}
int DrawShape(char title[], int dSigIndex, int sSigIndex) {
	FILE* targetFileStream;
	char targetFileName[50];
	//wchar_t* titleMBS= TransformMBS(title);

	sprintf(targetFileName, "CorrelationResult_Data%d_PSS%d.csv",
		dSigIndex + DSigFileNameOffset, sSigIndex + SSigFileNameOffset);
	if ((targetFileStream = fopen(targetFileName, "r")) == NULL) {
		printf("%sFailed reading %s", WarningMsg, targetFileStream);
	}
	char correlationData[MaxLenOfDSig / 2];
	for(int index = 0; index < MaxLenOfDSig / 2; index++) {
		fscanf(targetFileStream, "%lf", &correlationData[index]);
	}
	char outPutImagePath[50];
	initgraph(WindowWidth, WindowHeight, EX_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	setorigin(0, 0);
	settextcolor(LIGHTGRAY);
	setfont(30, 0, L"SimHei");
	int textHSpace = (TitleWidth - textwidth(titleMBS)) / 2;
	int textVSpace = (TitleHeight - textheight(titleMBS)) / 2;
	outtextxy(textHSpace, textVSpace, titleMBS);
	setorigin(20, WindowHeight - 20);

	setlinecolor(BLACK);
	//PolyPolyline();
	system("pause");
	return 0;
*/

/*
GLFW:OPENGL



*/