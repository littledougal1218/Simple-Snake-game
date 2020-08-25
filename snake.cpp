/*
CS 349 A1 Skeleton Code - Snake

- - - - - - - - - - - - - - - - - - - - - -

Commands to compile and run:

    g++ -o snake snake.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./snake

Note: the -L option and -lstdc++ may not be needed on some machines.
*/

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
/*
 * Global game state variables
 */
const int Border = 1;
const int BufferSize = 10;
const int FPS = 30;
const int width = 800;
const int height = 600;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
	int		width;		// size of window
	int		height;
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}

void eventLoop(XInfo &xinfo);

/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};       


int score;
int life = 1;


class Fruit : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x, y, 10, 10);
        }

        Fruit() {
        	srand(time(NULL));
		x = rand() % 800 + 1;
		y = rand() % 600 + 1;
        }

	void eat(){
		srand(time(NULL));
		x = rand() % 800 + 1;
		y = rand() % 600 + 1;
	}   
	int getX(){
		return x;
	}     

	int getY(){
		return y;
	}
	
    private:
        int x;
        int y;
};

Fruit fruit = Fruit();
list<Displayable *> dList;           // list of Displayables

class Snake : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			if (direction % 2 == 0){
				XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x, y, 15, blockSize);
			}
			else{
				XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x, y, blockSize, 15);
			}
			if (tails.size() != 0){
				int l = tails.size();
				for (int i = 0; i < l; i ++){
					Snake s = tails.at(i);
					if (direction % 2 == 0){
						XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], s.getX(), s.getY(), 10, blockSize);
					}
					else{
						XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], s.getX(), s.getY(), blockSize, 10);
					}
				}
			}
		}
		
		void move(XInfo &xinfo) {
			int ox = this->x;
			int oy = this->y;
			int od = this->direction;
			if (direction == 0){
				x += speed;
			}
			else if (direction == 1){
				y += speed;
			}
			else if (direction == 2){
				x -= speed;
			}
			else if (direction == 3){
				y -= speed;
			}
			int l = tails.size();
			if (l > 0){
				int tempx = tails.at(0).getX();
				int tempy = tails.at(0).getY();
				int tempd = tails.at(0).getd();
				tails.at(0).x = ox;
				tails.at(0).y = oy;
				tails.at(0).direction = od;
				ox = tempx;
				oy = tempy;
				od = tempd;
				for (int i = 1; i < l; i ++){
					tempx = tails.at(i).getX();
					tempy = tails.at(i).getY();
					tempd = tails.at(i).getd();
					tails.at(i).x = ox;
					tails.at(i).y = oy;
					tails.at(i).direction = od;
					ox = tempx;
					oy = tempy;
					od = tempd;
				}
			}


			if (x >= fruit.getX() - 10 && y >= fruit.getY() - 10 && x <= fruit.getX() + 10 && y <= fruit.getY() + 10){
				this->didEatFruit();
			}

			if (x < 0 || x > 800 - blockSize || y < 0|| y > 600 - blockSize){
				this->didHitObstacle();
			}

			for (int i = 0; i < l; i ++){
				Snake s = tails.at(i);
				if (x >= s.x - 4 && y >= s.y - 4 && x <= s.x + 4 && y <= s.y + 4){
					this->didHitObstacle();
				}
			}
		}
		
		void changeX(int a){
			x = a;
		}

		void changeY(int a){
			y = a;
		}

		int getX() {
			return x;
		}
		
		int getY() {
			return y;
		}
		int getd(){
			return direction;
		}
		void changed(int d){
			direction = d;
		}

	void didEatFruit() {
		fruit.eat();
		score += 10;
		int l = tails.size();
		if (l == 0){
			if (direction == 0){
				Snake s = Snake(x - 25, y, 0);
				tails.emplace_back(s);
			}
			else if (direction == 1){
				Snake s = Snake(x, y - 25, 1);
				tails.emplace_back(s);
			}
			else if (direction == 2){
				Snake s = Snake(x + 25, y, 2);
				tails.emplace_back(s);
			}
			else if (direction == 3){
				Snake s = Snake(x, y + 25, 3);
				tails.emplace_back(s);
			}
		}
		else if (l == 1){
			Snake t = tails.at(0);
			if (t.x < x){
				Snake s = Snake(t.x - 25, t.y, 0);
				tails.emplace_back(s);
			}
			else if (t.x > x){
				Snake s = Snake(t.x + 25, t.y, 1);
				tails.emplace_back(s);
			}
			if (t.y < y){
				Snake s = Snake(t.x, t.y - 25, 2);
				tails.emplace_back(s);
			}
			if (t.y > y){
				Snake s = Snake(t.x, t.y + 25, 3);
				tails.emplace_back(s);
			}

		}

		else{
			int l = tails.size();
			Snake t = tails.at(l - 1);
			Snake t2 = tails.at(l - 2);
			if (t.x < t2.x){
				Snake s = Snake(t.x - 25, t.y, 0);
				tails.emplace_back(s);
			}
			else if (t.x > t2.x){
				Snake s = Snake(t.x + 25, t.y, 1);
				tails.emplace_back(s);
			}
			else if (t.y < t2.y){
				Snake s = Snake(t.x, t.y - 25, 2);
				tails.emplace_back(s);
			}
			else if (t.y > t2.y){
				Snake s = Snake(t.x, t.y + 25, 3);
				tails.emplace_back(s);
			}
		}

        }

        void didHitObstacle() {
		life -= 1;
		if (life > 0){
			x = 100;
			y = 450;
			tails.clear();
		}
        }

        void turnLeft() {
		direction -= 1;
		if (direction == -1){
			direction = 3;
		}
        }

        void turnRight() {
		direction += 1;
		if (direction == 4){
			direction = 0;
		}
        }
		
	Snake(int x, int y, int d): x(x), y(y) {
		direction = d;
        	blockSize = 10;
		speed = 5;
	}
	
	private:
		int x;
		int y;
		int blockSize;
		int direction;
		int speed;
		vector<Snake> tails;
};


Snake snake(100, 450, 0);



/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"animation",		// window's title
		"Animate",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | 
		EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask);  // for resize events

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {	

	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	XClearWindow( xinfo.display, xinfo.window );
	
	// get height and width of window (might have changed since last repaint)

	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	// big black rectangle to clear background
    
	// draw display list
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}
	XFlush( xinfo.display );
}

void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		}
	}
	int d = snake.getd();
	switch(key){
		case '.':
			if (score >= 50){
				life += 1;
				score -= 50;
			}
			break;
		case 'r':
			if (life == 0){
				life += 1;
				score = 0;
				snake = Snake(100, 450, 0);
				dList.pop_front();
				dList.pop_front();
				dList.push_front(&snake);
				fruit = Fruit();
				dList.push_front(&fruit);
				
			}
			break;
		case 'w':
			if (d == 0){
				snake.turnLeft();
			}
			else if (d == 2){
				snake.turnRight();
			}
			break;
		case 's':
			if (d == 2){
				snake.turnLeft();
			}
			else if (d == 0){
				snake.turnRight();
			}
			break;
		case 'a':
			if (d == 3){
				snake.turnLeft();
			}
			else if (d == 1){
				snake.turnRight();
			}
			break;
		case 'd':
			if (d == 1){
				snake.turnLeft();
			}
			else if (d == 3){
				snake.turnRight();
			}
			break;

		case XK_Up:
			if (d == 0){
				snake.turnLeft();
			}
			else if (d == 2){
				snake.turnRight();
			}
			break;
		case XK_Down:
			if (d == 2){
				snake.turnLeft();
			}
			else if (d == 0){
				snake.turnRight();
			}
			break;
		case XK_Left:
			if (d == 3){
				snake.turnLeft();
			}
			else if (d == 1){
				snake.turnRight();
			}
			break;
		case XK_Right:
			if (d == 1){
				snake.turnLeft();
			}
			else if (d == 3){
				snake.turnRight();
			}
			break;
	}
}

void handleAnimation(XInfo &xinfo, int inside) {
    /*
     * ADD YOUR OWN LOGIC
     * This method handles animation for different objects on the screen and readies the next frame before the screen is re-painted.
     */ 
	snake.move(xinfo);
}

// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&snake);
    dList.push_front(&fruit);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;


	while( true ) {
		/*
		 * This is NOT a performant event loop!  
		 * It needs help!
		 */
		if (life <= 0){
			dList.remove(&snake);
			dList.remove(&fruit);
			XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 300, 300, "Game Over", 9);
			XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 100, 500, "Your score is printed ",  21);
			cout << score << endl;
		}
		
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			cout << "event.type=" << event.type << "\n";
			switch( event.type ) {

				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		} 

		usleep(1000000/FPS);
		handleAnimation(xinfo, inside);
		repaint(xinfo);
	}
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {

	for (int i = 0; i < )

	XInfo xInfo;
	score = 0;
	life = 1;
	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
