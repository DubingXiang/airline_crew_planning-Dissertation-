#pragma once
#ifndef IPATH_H
#define IPATH_H

class IPath
{
public:
	IPath() {};
	virtual ~IPath() {};

	virtual double getCost() const = 0;	
};

#endif // !IPATH_H