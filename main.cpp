/* ZJW simple C++ code to write out a PPM file representing an ellipse(s) */

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <queue>
#include <stdexcept>
#include <thread> 
#include <future>
#include "shape.h"
#include "color.h"
#include "rect.h"
#include "implicit2D.h"
#include "polygon.h"
#include "util.h"
#include "image.h"

using namespace std;


/* new helper funtion to test if a given pixel is inside any of the list of shapes 
    note types
    */
void inAnyShape(bool* inSide, const int x, const int y, const vector<shared_ptr<shape> >& theShapes, color* drawC, double* curDepth) {

	(*inSide) = false;

	//iterate through all possible equations
	for (auto eq : theShapes) {
		if (eq->eval(x, y) && eq->getDepth() > (*curDepth)) {
			(*drawC) = eq->getInC();
			(*inSide) = true;
			(*curDepth) = eq->getDepth();
		}
	}

}

/* note function signature that takes in two lists of shapes for parallelizing */
void createImage(image* theImg,  int startX, int endX, int startY, int endY, 
	const vector<shared_ptr<shape> >& topShapes, const vector<shared_ptr<shape> >& botShapes, color inC) {

	color drawCT;
	color drawCB;
	bool inTopTrue = false;
	bool inBotTrue = false;
	double curDepthT = -1.0;
	double curDepthB = -1.0;

	//for every point in the 2D space
	for (int y=startY; y < endY; y++) {
		for (int x =startX; x < endX; x++) {

			curDepthT = -1.0;
			curDepthB = -1.0;
			
			/* check if this pixel is inside the list of either shapes - BASE only one checked */
			thread c5(inAnyShape,&inTopTrue, x, y, topShapes, &drawCT, &curDepthT);
			thread c6(inAnyShape,&inBotTrue, x, y, botShapes, &drawCB, &curDepthB);
			c5.join();
			c6.join();
			if (inTopTrue || inBotTrue) {
				if ((inBotTrue && inTopTrue && curDepthB > curDepthT) || (inBotTrue && !inTopTrue))	{
					theImg->setPixel(x, y, drawCB);
				}
				else {
					theImg->setPixel(x, y, drawCT);
				}
			} else {
				theImg->setPixel(x, y, inC);
			}
			
		}
	}
}

/* super simple function for testing threads */
void foo(int a, int b) {
	cout << " a: " << a << " b: " << b << endl;
}

/*read command line arguments and write out new ascii file of given size */
int main(int argc, char *argv[]) {

	ofstream outFile;
	int sizeX, sizeY;
	color background(112, 134, 156);


	sizeX = stoi(argv[1]);
	sizeY = stoi(argv[2]);
	image theImage(sizeX, sizeY, background);

	/* two lists of shapes */
	vector<shared_ptr<shape>> topShapes;
	vector<shared_ptr<shape>> botShapes;

	vec2 heroLoc(sizeX/2, sizeY/2);
	topShapes.push_back(make_shared<Implicit2D>(heroLoc, 30, 30, color(12, 34, 156)));
	
	vec2 shapeLoc;
	shared_ptr<shape> theShape;
	for (int j=0; j < 500; j++) {
		shapeLoc = vec2(nicerRand(100, theImage.h()), nicerRand(100, theImage.h()));

		theShape = make_shared<Implicit2D>(shapeLoc,
								nicerRand(11, 30), nicerRand(11, 30), color(nicerRand(123, 250), 12, nicerRand(123, 250)));
		//to start all shapes put in top share list
		if (shapeLoc.y()>theImage.h()/2){
		topShapes.push_back(theShape);
		}
		else {botShapes.push_back(theShape);}

	}

	for (int i=0; i < 100; i++) {
		shapeLoc = vec2(nicerRand(0, theImage.h()), nicerRand(0, theImage.h()));
		theShape = 	make_shared<Rect>(shapeLoc,  nicerRand(18, 43), nicerRand(15, 36), color(nicerRand(123, 250), 12, 112), nicerRand(1, 20));
		//to start all shapes put in top share list
		if (shapeLoc.y()>theImage.h()/2){
			topShapes.push_back(theShape);
		}
		else {botShapes.push_back(theShape);}
	}
	
	string outFilename;
	try {
		if (argc < 4) 
			throw invalid_argument("Error format: a.out sizeX sizeY outfileName");
	} 
    catch (const invalid_argument& e)
    {
        cerr << "Error: " << e.what() << endl;
        exit(0);
    }

    //DEBUG info left as you divide shapes!
    cout << "size top shapes " << topShapes.size() << endl;
    cout << "size bshapes " << botShapes.size() << endl;

	//code to write the files
	outFilename.append(argv[3]);
	outFilename.append(".ppm");
	outFile.open(outFilename);

	//int startX, int endX, int startY, int endY
	thread c1(createImage,&theImage, 0, theImage.w()/2, 0, theImage.h()/2, topShapes, botShapes, color(12));
	thread c2(createImage,&theImage, theImage.w()/2, theImage.w(), 0, theImage.h()/2, topShapes, botShapes, color(79, 52, 235));
	thread c3(createImage,&theImage, 0, theImage.w()/2, theImage.h()/2, theImage.h(), topShapes, botShapes, color(60, 133, 82));
	thread c4(createImage,&theImage, theImage.w()/2, theImage.w(), theImage.h()/2, theImage.h(), topShapes, botShapes, color(133, 60, 60));

	c1.join();
	c2.join();
	c3.join();
	c4.join();

	/* example thread call to foo fn */
	thread t1(foo, 4, 5);
	thread t2(foo, theImage.w(), theImage.h());
	t1.join();
	t2.join();

	if (outFile) {
		cout << "writing an image of size: " << sizeX << " " << sizeY << " to: " << argv[3] << endl;
		theImage.fullWriteOut(outFile);
		outFile.close();
		outFilename.erase();
	} else {
		cout << "Error cannot open outfile" << endl;
		exit(0);
	}

}
