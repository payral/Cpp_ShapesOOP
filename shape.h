#ifndef SHAPE_H
#define SHAPE_H
#include "vec2.h"
#include "color.h"


/* a base class for our lecture code! */
class shape {
  public:
  	shape(color theColor, double inDepth) : inC(theColor), depth(inDepth) {}

	color getInC() const {return inC; }
	double getDepth() const {return depth; }

	void setDepth(double inD) { depth= inD; }
	void setColor(color col) { inC = col; }

	virtual bool eval(double x, double y) = 0; //pure virtual

	friend std::ostream& operator<<(std::ostream &out, const shape &s);

  protected:
	color inC;
	double depth;
};

inline std::ostream& operator<<(std::ostream &out, const shape &s) {
	return out << "color of shape: " << s.inC << " depth: " << s.depth;
}

#endif
