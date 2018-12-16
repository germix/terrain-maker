#ifndef VERTEX_H
#define VERTEX_H
#include <QColor>
#include <QVector>
#include <math.h>
#include "Vector2d.h"

#define VERTEX_RADIUS 8

class Vertex
{
public:
	enum TYPE
	{
		TYPE_UNDEFINED,
		TYPE_GRASS,
		TYPE_LEFT,
		TYPE_RIGHT,
		TYPE_BOTTOM,
	};
	Vector2d	pos;
	Vertex*		prev;
	Vertex*		next;
	QColor		color;
	int			type;
	double		angle;
	double		relativeAngle;

	Vector2d	top;
	Vector2d	bottom;
public:
	Vertex(const Vector2d& p);
	~Vertex();
public:
	void recomputeType();
};

class VertexList
{
public:
	Vertex*			head;
	Vertex*			tail;
	int				size;
public:
	VertexList()
	{
		head = 0;
		tail = 0;
		size = 0;
	}
	~VertexList()
	{
		clear();
	}
public:
	void clear()
	{
		Vertex* n;
		Vertex* p;
		/*
		for(p = head; p != 0; p = n)
		{
			n = p->next;
			delete p;
		}
		*/
		if(head)
		{
			p = head;
			do
			{
				n = p->next;
				delete p;
				p = n;
			}
			while(n != NULL && n != head);
		}
		head = 0;
		tail = 0;
		size = 0;
	}
	void close()
	{
		if(head != nullptr)
		{
			tail->next = head;
			head->prev = tail;
		}
	}
	void remove(Vertex* v)
	{
		if(v == head)
			head = head->next;
		if(v == tail)
			tail = tail->prev;
		if(v->next)
			v->next->prev = v->prev;
		if(v->prev)
			v->prev->next = v->next;
		delete v;
		size--;
	}
	Vertex* append(double x, double y)
	{
		Vertex* v = new Vertex(Vector2d(x, y));
		if(head == nullptr)
		{
			head = v;
			tail = v;
		}
		else
		{
			tail->next = v;
			v->prev = tail;
			tail = v;
		}
		size++;
		return v;
	}
	Vertex* append(double x, double y, Vertex* afterOf)
	{
		Vertex* v = new Vertex(Vector2d(x, y));

		afterOf->next->prev = v;
		v->next = afterOf->next;
		v->prev = afterOf;
		afterOf->next = v;

		if(afterOf == tail)
		{
			/*
			if(tail == head)
			{
				head->prev = v;
			}
			*/
			tail = v;
		}
		size++;
		return v;
	}

	bool isEmpty() const
	{
		return (head == nullptr);
	}
	bool isClosed() const
	{
		return (head != nullptr && head->prev == tail);
	}

	QVector<Vertex*> getOpenList()
	{
		Vertex* v = head;
		QVector<Vertex*> list;
		if(v != nullptr)
		{
			list.append(v);
			for(v = v->next; v != nullptr && v != head; v = v->next)
				list.append(v);
		}
		return list;
	}

	QVector<Vertex*> getClosedList()
	{
		Vertex* v = head;
		QVector<Vertex*> list;
		if(v != nullptr)
		{
			list.append(v);
			for(v = v->next; v != nullptr && v != head; v = v->next)
				list.append(v);

			if(head->prev == tail)
				list.append(v);
		}
		return list;
	}
	Vertex* getVertexAt(int mouseX, int mouseY)
	{
		Vertex* v = head;
		Vector2d mousePoint(mouseX, mouseY);

		if(v != 0)
		{
			do
			{
				if(mousePoint.distanceOf(v->pos) < VERTEX_RADIUS)
				{
					return v;
				}
				v = v->next;
			}
			while(v != NULL && v != head);
		}
		return NULL;
	}
};

#endif // VERTEX_H
