//
// solarSystem.cpp : 定义控制台应用程序的入口点。
// solarSystem
//
// Created by 潘学海.
// Copyright © 2016 潘学海. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include <GLUT/glut.h> //GLUT头文件
#include "SeqList.h" //顺序表类
#include "SeqQueue.h" //顺序队列类
#include "Vector2D.h" //二维矢量类

using namespace std;

#define  GLUT_WHEEL_UP 3
#define  GLUT_WHEEL_DOWN 4

const unsigned int width = 768; //窗口宽度
const unsigned int height = 768; //窗口高度
const double Pi = 3.14159265358979323846; //圆周率
const double G = 4.0 * Pi * Pi; //万有引力常量
const unsigned int bufferSize = 2000; //星体轨道缓冲大小
double scalingFactorX = 10.0; //横向放大因子
double scalingFactorY = 10.0; //纵向放大因子
double radiusFactor = 1.0; //星体半径放大因子
double scalingFactorX0 = 10.0; //初始横向放大因子
double radiusFactor0 = 1.0; //初始星体半径放大因子
const double minv = 1E-2; //最小速度
const double maxds = 5E-4; //最大位移步长
const double maxdv = 5E-3; //最大速度步长
const double minR = 1E-5; //最小星体间距
double maxRandMass = 1E-3; //最大随机星体质量
double maxRandRadius = 1E-3; //最大随机星体半径
unsigned int frame = 0; //帧数 每200帧将星体位置加入轨迹缓冲
double theta = 0.0; //视场旋角
double theta0 = 0.0; //鼠标按下时的视场旋角
Vector2D mouse = Vector2D(); //鼠标按下时的指针位置
bool leftButtonDown = false; //存储鼠标左键按下状态

class Color //颜色类
{
public:
	double R, G, B;

	Color(double r = 1.0, double g = 1.0, double b = 1.0) //构造函数
			: R(r), G(g), B(b)
	{
	}

	Color(const Color &tColor) //拷贝构造函数
	{
		*this = tColor;
	}

	virtual ~Color() //析构函数
	{
	}

	const Color &operator=(const Color &tColor) //拷贝
	{
		R = tColor.R, G = tColor.G, B = tColor.B;
		return *this;
	}

	const bool operator==(const Color &tColor) const //判断相等
	{
		return (R == tColor.R && G == tColor.G && B == tColor.B);
	}

	const bool operator!=(const Color &tColor) const //判断不相等
	{
		return (R != tColor.R || G != tColor.G || B != tColor.B);
	}

	const Color operator+(const Color &tcolor) const //加法
	{
		return Color(abs(R + tcolor.R), abs(G + tcolor.G), abs(B + tcolor.B));
	}

	const Color operator-(const Color &tcolor) const //减法
	{
		return Color(abs(R - tcolor.R), abs(G - tcolor.G), abs(B - tcolor.B));
	}

	const Color operator*(const double k) const //数乘
	{
		return Color(abs(k * R), abs(k * G), abs(k * B));
	}

	const Color operator/(const double k) const //数乘
	{
		return Color(abs(R / k), abs(G / k), abs(B / k));
	}
};

const Color operator*(const double k, const Color &tcolor) //数乘
{
	return Color(abs(k * tcolor.R), abs(k * tcolor.G), abs(k * tcolor.B));
}

const double random01() //获取0到1内的随机数
{
	return (rand() / (RAND_MAX + 1.0));
}

class Planet //星体类
{
public:
	Vector2D r, v; //星体位置和速度
	double m, R; //星体质量和半径
	Color color; //星体颜色
	SeqQueue<Vector2D> orbitalBuffer; //轨迹缓冲

	Planet(const Vector2D &tr = Vector2D(), const Vector2D &tv = Vector2D(), const double tm = 1.0,
	       const double tR = 1.0, const Color &tcolor = Color(0.25 + 0.75 * random01(), 0.25 + 0.75 * random01(),
	                                                          0.25 + 0.75 * random01())) //构造函数
			: r(tr), v(tv), m(tm), R(tR), color(tcolor), orbitalBuffer(bufferSize)
	{
	}

	Planet(const Planet &tplanet) //拷贝构造函数
	{
		*this = tplanet;
	}

	virtual ~Planet() //析构函数
	{
		r.~Vector2D();
		v.~Vector2D();
		color.~Color();
		orbitalBuffer.~SeqQueue();
	}

	const Planet &operator=(const Planet &tplanet) //拷贝
	{
		r = tplanet.r, v = tplanet.v;
		m = tplanet.m, R = tplanet.R;
		color = tplanet.color;
		orbitalBuffer = tplanet.orbitalBuffer;
		return *this;
	}
};

SeqList<Planet> *psolarSystem; //星系星体顺序表指针 用于绘图

const double potential(const SeqList<Planet>::iterator &p, const SeqList<Planet> &solarSystem) //获取单位引力势
{
	double Ep = 0.0, R;
	for (SeqList<Planet>::iterator i = solarSystem.begin(); i != solarSystem.end(); ++i)
	{
		if (i != p)
		{
			R = ((*p).r - (*i).r).mold();
			if (R >= minR)
			{
				Ep -= (*i).m / R;
			}
		}
	}
	return (G * Ep);
}

const Vector2D gravityAcceleration(const SeqList<Planet>::iterator &p, const SeqList<Planet> &solarSystem) //获取加速度
{
	Vector2D a = Vector2D(), r;
	for (SeqList<Planet>::iterator i = solarSystem.begin(); i != solarSystem.end(); ++i)
	{
		if (i != p)
		{
			r = (*p).r - (*i).r;
			if (r.mold() >= minR)
			{
				a -= (*i).m * r.normalized() / r.moldSquare();
			}
		}
	}
	return (G * a);
}

void collision(SeqList<Planet> &solarSystem) //解决星体碰撞及质心系修正
{
	if (solarSystem.isEmpty())
		return;
	SeqList<Planet>::iterator i, j;
	for (i = solarSystem.begin(); (i + 1) < solarSystem.end(); ++i) //解决星体碰撞
	{
		j = i + 1;
		while (j < solarSystem.end())
		{
			if (((*j).r - (*i).r).mold() <= (*i).R + (*j).R) //若两星体碰撞 则和为新星体 满足质量守恒和动量守恒
			{
				(*i).r = ((*i).m * (*i).r + (*j).m * (*j).r) / ((*i).m + (*j).m);
				(*i).v = ((*i).m * (*i).v + (*j).m * (*j).v) / ((*i).m + (*j).m);
				(*i).color = ((*i).m * (*i).color + (*j).m * (*j).color) / ((*i).m + (*j).m);
				(*i).m += (*j).m;
				(*i).R = pow(pow((*i).R, 3) + pow((*j).R, 3), 1.0 / 3);
				(*i).orbitalBuffer.clear();
				solarSystem.del(j);
				i = solarSystem.begin(), j = solarSystem.begin() + 1;
			}
			else
				++j;
		}
	}
	Vector2D rc = Vector2D(), vc = Vector2D();
	double mc = 0.0;
	for (SeqList<Planet>::iterator p = solarSystem.begin(); p != solarSystem.end(); ++p) //计算质心位置和质心速度
	{
		rc += (*p).r * (*p).m;
		vc += (*p).v * (*p).m;
		mc += (*p).m;
	}
	rc /= mc;
	vc /= mc;
	unsigned int orbitalBufferSize;
	Vector2D r;
	for (SeqList<Planet>::iterator p = solarSystem.begin(); p != solarSystem.end(); ++p) //质心系修正
	{
		(*p).r -= rc;
		(*p).v -= vc;
		orbitalBufferSize = (*p).orbitalBuffer.size();
		for (unsigned int i = 0; i < orbitalBufferSize; ++i)
		{
			(*p).orbitalBuffer.popFront(r);
			(*p).orbitalBuffer.pushBack(r - rc);
		}
	}
}

void next(SeqList<Planet> &solarSystem, double dt = 0.001) //计算下一时刻各星体位置
{
	if (solarSystem.isEmpty())
		return;
	SeqList<Planet>::iterator p;
	Vector2D a;
	double V, Vt, vSquare;
	for (p = solarSystem.begin(); p != solarSystem.end(); ++p) //设置动态步长
	{
		if ((*p).v.mold() >= minv)
		{
			if ((*p).v.mold() * dt > maxds)
				dt = maxds / (*p).v.mold();
		}
		if (gravityAcceleration(p, solarSystem).mold() * dt > maxdv)
			dt = maxdv / gravityAcceleration(p, solarSystem).mold();
	}
	for (p = solarSystem.begin(); p != solarSystem.end(); ++p) //计算下一时刻各星体位置
	{
		V = potential(p, solarSystem);
		a = gravityAcceleration(p, solarSystem);
		vSquare = (*p).v.moldSquare();
		(*p).r += ((*p).v + a * (dt / 2.0)) * dt; //直接蛙跳算法
		(*p).v += a * dt;
		Vt = potential(p, solarSystem);
		vSquare += 2.0 * (V - Vt); //能量修正
		if (vSquare < 0.0)
			vSquare = 0.0;
		(*p).v = (*p).v.normalized() * sqrt(vSquare); //速度矢量方向为由蛙跳算法算得的结果 而速度大小则为能量修正值
	}
	collision(solarSystem); //解决星体碰撞及质心系修正
}

void initialize(SeqList<Planet> &solarSystem) //初始化星系
{
	srand(time(0));
	ifstream fin("SolarSystem.txt", ios_base::in);
	if (fin.is_open()) //从文件读取数据初始化星系
	{
		double r, R, m, T;
		Vector2D tr, tv;
		Color color;
		char planet[20];
		while (!fin.eof())
		{
			fin >> planet >> r >> R >> m >> T >> color.R >> color.G >> color.B;
			cout << planet << ':' << endl;
			cout << "Mean orbital radius: " << r << "AU" << endl;
			cout << "Mean radius: " << R << "AU" << endl;
			cout << "Mass: " << m << "M☉" << endl;
			cout << "Orbital period: " << T << "y" << endl << endl;
			tr = r * Vector2D(2.0 * Pi * random01()); //用圆轨道初始化星体位置 随机初始辐角
			tv = (2.0 * Pi / T) * tr.rotated(Pi / 2.0);    //利用圆轨道初始化星体速度
			solarSystem.pushBack(Planet(tr, tv, m, R, color));
			if (r * scalingFactorX > 1.5)
				scalingFactorX = 1.5 / r;
		}
		scalingFactorY = 0.667 * scalingFactorX;
		radiusFactor = 0.6 * solarSystem[1].r.mold() / solarSystem[0].R;
		maxRandMass = 1E-3, maxRandRadius = 1E-3;
	}
	else //随机初始化星系
	{
		unsigned int N = 3;
		cout << "Please enter the number of planets in the system: ";
		while (!(cin >> N)) //异常输入处理
		{
			cin.clear();
			while (cin.get() != '\n')
				continue;
			cout << "Bad Input, please enter the number of planets in the system: ";
		}
		if (N == 0)
			N = 1;
		solarSystem.resize(N);
		Planet tplanet;
		maxRandMass = 1.0, maxRandRadius = 1E-2;
		for (unsigned int i = 0; i < N; ++i) //随机初始化星系
		{
			tplanet.r = 5.0 * random01() * Vector2D(2.0 * Pi * random01());
			tplanet.v = Pi * random01() * Vector2D(2.0 * Pi * random01());
			tplanet.m = maxRandMass * random01();
			tplanet.R = maxRandRadius * (0.75 + 0.25 * random01()) * pow(tplanet.m, 0.4);
			tplanet.color = Color(0.25 + 0.75 * random01(), 0.25 + 0.75 * random01(), 0.25 + 0.75 * random01());
			solarSystem.pushBack(tplanet);
			if (tplanet.r.mold() * scalingFactorX > 0.95)
				scalingFactorX = 0.95 / tplanet.r.mold();
		}
		scalingFactorY = scalingFactorX;
		radiusFactor = 2.5;
	}
	scalingFactorX0 = scalingFactorX, radiusFactor0 = radiusFactor; //初始化视角和放缩因子
	theta = 0.0;
	mouse = Vector2D();
	leftButtonDown = false;
	collision(solarSystem); //解决星体碰撞及质心系修正
}

void display() //绘图
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(1.0, 1.0, 1.0);
	glLineWidth(1.0);
	glBegin(GL_LINES); //绘制坐标轴
	glVertex2d(-1.0, 0.0);
	glVertex2d(1.0, 0.0);
	glVertex2d(0.0, -1.0);
	glVertex2d(0.0, 1.0);
	glEnd();
	unsigned int orbitalBufferSize;
	double alpha;
	Vector2D r;
	for (SeqList<Planet>::iterator p = psolarSystem->begin(); p != psolarSystem->end(); ++p)
	{
		glColor3d((*p).color.R, (*p).color.G, (*p).color.B);
		orbitalBufferSize = (*p).orbitalBuffer.size();
		glLineWidth(1.5);
		glBegin(GL_LINE_STRIP); //依据轨迹缓冲绘制轨迹
		for (unsigned int i = 0; i < orbitalBufferSize; ++i)
		{
			alpha = 1.0 - double(orbitalBufferSize - i) / bufferSize;
			(*p).orbitalBuffer.popFront(r);
			glColor3d(alpha * (*p).color.R, alpha * (*p).color.G, alpha * (*p).color.B);
			glVertex2d(scalingFactorX * r.rotated(theta).x, scalingFactorY * r.rotated(theta).y);
			(*p).orbitalBuffer.pushBack(r);
		}
		glVertex2d(scalingFactorX * (*p).r.rotated(theta).x, scalingFactorY * (*p).r.rotated(theta).y);
		glEnd();
		if (frame == 0) //每200帧将星体位置加入轨迹缓冲
		{
			if ((*p).orbitalBuffer.isFull())
				(*p).orbitalBuffer.popFront();
			(*p).orbitalBuffer.pushBack((*p).r);
		}
		r = (*p).r.rotated(theta);
		glBegin(GL_POLYGON); //绘制星体
		for (double phi = 0.0; phi < 2.0 * Pi; phi += Pi / 16)
			glVertex2d(scalingFactorX * (r.x + radiusFactor * (*p).R * cos(phi)),
			           scalingFactorY * r.y + scalingFactorX * radiusFactor * (*p).R * sin(phi));
		glEnd();
	}
	glFlush();
	glutSwapBuffers();
}

void idle()
{
	next(*psolarSystem, 0.001); //计算下一时刻各星体位置
	frame = (frame + 1) % 200;
	display();
}

void mouseFunc(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
	{
		double x = 2.0 * mx / width - 1.0, y = 1.0 - 2.0 * my / height; //计算鼠标指针位置
		if (state == GLUT_DOWN) //按下鼠标按键时
		{
			mouse = Vector2D(x, y); //设置按下鼠标时指针位置
			theta0 = theta; //设置按下鼠标时视角转动角度
			leftButtonDown = (button == GLUT_LEFT_BUTTON); //设置鼠标左键按下状态
		}
		else if (button == GLUT_LEFT_BUTTON) //鼠标左键弹起
			leftButtonDown = false; //设置鼠标左键按下状态
		else //鼠标又键弹起 加入新星体
		{
			Planet tplanet;
			tplanet.r = Vector2D(mouse.x / scalingFactorX, mouse.y / scalingFactorY).rotated(
					-theta); //设置新星体初位置为按下鼠标时鼠标位置
			mouse = Vector2D(x / scalingFactorX, y / scalingFactorY).rotated(-theta);
			tplanet.v = 8.0 * Pi * (mouse - tplanet.r); //设置新星体初速度为按下到弹起鼠标位置移动量
			tplanet.m = maxRandMass * random01();
			tplanet.R = maxRandRadius * (0.75 + 0.25 * random01()) * pow(tplanet.m, 0.4);
			tplanet.color = Color(0.25 + 0.75 * random01(), 0.25 + 0.75 * random01(), 0.25 + 0.75 * random01());
			psolarSystem->pushBack(tplanet);
			collision(*psolarSystem); //解决星体碰撞及质心系修正
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) //按下鼠标中键重设视角和放缩因子
		scalingFactorX = scalingFactorX0, scalingFactorY =
				0.667 * scalingFactorX0, radiusFactor = radiusFactor0, theta = 0.0;
	else if (button == GLUT_WHEEL_DOWN) //鼠标滚轮向下 缩小视角
	{
		scalingFactorX /= 1.1;
		scalingFactorY /= 1.1;
		radiusFactor = pow(radiusFactor, 1.05);
		radiusFactor = (radiusFactor < radiusFactor0 ? radiusFactor : radiusFactor0);
	}
	else if (button == GLUT_WHEEL_UP) //鼠标滚轮向上 放大视角
	{
		scalingFactorX *= 1.1;
		scalingFactorY *= 1.1;
		if (radiusFactor > 1.01 && scalingFactorX > scalingFactorX0)
			radiusFactor = pow(radiusFactor, 0.95);
		radiusFactor = (radiusFactor > 1.01 ? radiusFactor : 1.01);
	}
	glutPostRedisplay();
}

void motionFunc(int mx, int my)
{
	if (leftButtonDown) //若鼠标左键按下 设置视角和视场旋转
	{
		double x = 2.0 * mx / width - 1.0, y = 1.0 - 2.0 * my / height;
		double factor = scalingFactorY / scalingFactorX;
		factor += 0.25 * (mouse.y - y);
		factor = (factor > 1.0 ? 1.0 : factor);
		factor = (factor < 0.4 ? 0.4 : factor);
		scalingFactorY = factor * scalingFactorX;
		theta = theta0 + (Vector2D(x, y).argument() - mouse.argument());
		while (theta < 0.0 || theta > 2.0 * Pi)
			theta += (theta < 0.0 ? 2.0 : -2.0) * Pi;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	SeqList<Planet> solarSystem; //星系星体顺序表
	psolarSystem = &solarSystem; //星系星体顺序表指针
	initialize(solarSystem); //初始化星系
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("SolarSystem");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutMainLoop();
	return 0;
}