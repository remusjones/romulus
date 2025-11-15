//
// Created by Remus on 16/12/2023.
//
#pragma once
class IApplication
{
public:
	virtual ~IApplication() = default;
	virtual void Run() = 0;
};

