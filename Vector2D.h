//
// Vector2D.h
// Vector2D
//
// Created by 潘学海.
// Copyright © 2016 潘学海. All rights reserved.
//

#pragma once

#ifndef _VECTOR2D_H_

#define _VECTOR2D_H_

#include <cmath>

using namespace std;

class Vector2D //矢量类
{
public:
	double x, y; //分量

	Vector2D() //默认构造函数
		:x(0.0), y(0.0) {}

	Vector2D(const double X, const double Y) //构造函数
		:x(X), y(Y) {}

	Vector2D(const double theta) //构造函数
		:x(cos(theta)), y(sin(theta)) {}

	Vector2D(const Vector2D &vec) //拷贝构造函数
	{
		*this = vec;
	}

	virtual ~Vector2D() //析构函数
	{
	}

	const Vector2D & operator=(const Vector2D &vec) //拷贝
	{
		x = vec.x, y = vec.y;
		return *this;
	}

	double moldSquare() const //获取模方
	{
		return (x*x + y*y);
	}

	double mold() const //获取模
	{
		return sqrt(x*x + y*y);
	}

	double argument() const //获取辐角
	{
		return atan2(y, x);
	}

	const Vector2D & normalize() //单位化
	{
		if (moldSquare() > 0.0)
			*this /= mold();
		else
			*this = Vector2D(1.0, 0.0);
		return *this;
	}

	const Vector2D normalized() const //单位化
	{
		if (moldSquare() > 0.0)
			return (*this / mold());
		else
			return Vector2D(1.0, 0.0);
	}

	const Vector2D & rotate(const double theta) //逆时针旋转
	{
		double temp = x*cos(theta) - y*sin(theta);
		y = x*sin(theta) + y*cos(theta), x = temp;
		return *this;
	}

	const Vector2D rotated(const double theta) const //逆时针旋转
	{
		Vector2D temp(*this);
		return temp.rotate(theta);
	}

	const Vector2D operator+(const Vector2D &vec) const //加法
	{
		return Vector2D(x + vec.x, y + vec.y);
	}

	const Vector2D & operator+=(const Vector2D &vec) //加法
	{
		*this = *this + vec;
		return *this;
	}

	const Vector2D operator-(const Vector2D &vec) const //减法
	{
		return Vector2D(x - vec.x, y - vec.y);
	}

	const Vector2D & operator-=(const Vector2D &vec) //减法
	{
		*this = *this - vec;
		return *this;
	}

	const Vector2D operator-() const //负矢量
	{
		return Vector2D(-x, -y);;
	}

	const Vector2D operator*(const double k) const //数乘
	{
		return Vector2D(k*x, k*y);
	}

	const Vector2D & operator*=(const double k) //数乘
	{
		*this = (*this) * k;
		return *this;
	}

	const Vector2D operator/(const double k) const //数乘
	{
		return Vector2D(x / k, y / k);
	}

	const Vector2D & operator/=(const double k) //数乘
	{
		*this = (*this) / k;
		return *this;
	}

	const double operator*(const Vector2D &vec) const //点乘
	{
		return x*vec.x + y*vec.y;
	}

	const bool operator==(const Vector2D &vec) const //判断相等
	{
		return (x == vec.x && y == vec.y);
	}

	const bool operator!=(const Vector2D &vec) const //判断不等
	{
		return (x != vec.x || y != vec.y);
	}
};

const Vector2D operator*(const double k, const Vector2D &vec) //数乘
{
	return Vector2D(k*vec.x, k*vec.y);
}

#endif // !_VECTOR2D_H_