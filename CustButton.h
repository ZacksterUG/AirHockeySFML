#pragma once
#ifdef DEBUG
#ifndef _IOSTREAM_
#include <iostream>
#endif
#endif
using namespace std;
constexpr auto MAX_BUTTONS = 255;

class CustomButton
{
private:
	bool active = false;
	float sizeX = 0;
	float sizeY = 0;
	float posX = 0;
	float posY = 0;
	static int count;
	static int Buttons[MAX_BUTTONS];
	int ID = -1;
	Text T;
protected:
public:
	static int activeButtons()
	{
		int k = 0;
		for (int i = 0; i < MAX_BUTTONS; i++) if (CustomButton::Buttons[i] == 1) k++;
		return k;
	}
	friend bool ButtonExist(int ID);
	int returnID() { return this->ID; };
	void getPos(float& x, float& y) { x = this->posX; y = this->posY; }
	void getSize(float& x, float& y) { x = this->sizeX; y = this->sizeY; }
	void setPos(float x, float y) { this->posX = x; this->posY = y; }
	void setSize(float x, float y) { this->sizeX = x; this->sizeY = y; }
	bool isValid() { if (this->ID != -1) true; else false; }
	int currID() { return this->ID; }
	CustomButton()
	{
		this->sizeX = 0;
		this->sizeY = 0;
		this->posX = 0;
		this->posY = 0;
		this->ID = -1;
		this->active = false;
		T.setString("222");
	}
	CustomButton(CustomButton& other)
	{
		this->sizeX = other.sizeX;
		this->sizeY = other.sizeY;
		this->posX = other.posX;
		this->posY = other.posY;
		this->ID = other.ID;
		this->active = other.active;
	}
	CustomButton(float posX, float posY, float sizeX, float sizeY)
	{
		if (count > MAX_BUTTONS)
		{
			cout << "ERROR: Amount of BUTTONS are out of bound, increase MAX_BUTTONS to create more buttons" << endl;
			return;
		}
		this->posX = posX;
		this->posY = posY;
		this->sizeX = sizeX;
		this->sizeY = sizeY;
		this->active = false;
		active = false;
		count++;
		for (int i = 0; i < MAX_BUTTONS; i++)
		{
			if (Buttons[i] == 0)
			{
				this->ID = i;
				Buttons[i] = 1;
				break;
			}
		}
#ifdef DEBUG
		cout << "Created Button: Position - X " << this->posX << " Y " << this->posY << endl << "size - X " << this->sizeX << " Y" << this->sizeY << endl;
		cout << "Buttons ID: " << this->ID << endl;
#endif
	}
	~CustomButton()
	{
		count--;
		Buttons[this->ID] = 0;
	}
};

int CustomButton::count = 0;
int CustomButton::Buttons[MAX_BUTTONS] = { 0 };
bool ButtonExist(int ID)
{
	if (CustomButton::Buttons[ID] == 1) return true;
	else return false;
}