#include "cinder/app/AppBasic.h"
#include <vector>
using namespace ci;
using namespace ci::app;
using namespace std;

#define MOVE_F 0
#define MOVE_B 1
#define MOVE_U 2
#define MOVE_D 3
#define STOP 4
#define DIR_L 1
#define DIR_R 0
#define STEP 0.02f

class Player {
public:
	Vec2f pos;
	string name;
	Color color;
	vector<int> moves;
	int dir;
	
	Player(Vec2f _pos, string _name, Color _color, int _dir)
	{
		pos = _pos;
		name = _name;
		color = _color;
		dir = _dir;
	}
	
	
	void move(int mv){
		if(mv == MOVE_F)
		{
			pos.x += (dir == DIR_R ? STEP : -STEP);
		}
		else if(mv == MOVE_B)
		{
			pos.x += (dir == DIR_R ? -STEP : STEP);
		}
		else if(mv == MOVE_U)
		{
			pos.y -= STEP;
		}
		else if(mv == MOVE_D)
		{
			pos.y += STEP;
		}
			
	}
	
};

class Moves
{
public:
	int p0;
	int p1;
	
	Moves(int _p0, int _p1) {
		p0 = _p0;
		p1 = _p1;
	}
	
	Moves(){
		p0 = STOP;
		p1 = STOP;
	}
};


// We'll create a new Cinder Application by deriving from the AppBasic class
class nwbApp : public AppBasic {
 public:
	// Cinder will always call this function whenever the user drags the mouse
	void setup();
	void mouseDrag( MouseEvent event );
	// Cinder calls this function 30 times per second by default
	void draw();
	void update();
	void keyDown(KeyEvent event);
	Moves evaluate(int p0, int p1);
	
	std::vector< shared_ptr<Player> > mPlayers;
	unsigned int round;
	Moves result[4][4];
	char tx[5];
};

void nwbApp::setup()
{
	shared_ptr<Player> p1( new Player( Vec2f(.3f,.0f), "p1", Color(1.0f, .0f, .0f), DIR_L ) );
	shared_ptr<Player> p2( new Player( Vec2f(-.3f,.0f), "p2", Color(.0f, 1.0f, .0f), DIR_R ) );
	
	mPlayers.push_back(p1);
	mPlayers.push_back(p2);
	
	round = 1;
	
	result[MOVE_F][MOVE_B] = Moves(MOVE_B, MOVE_F);
	result[MOVE_F][MOVE_D] = Moves(MOVE_F, MOVE_D);
	result[MOVE_F][MOVE_U] = Moves(MOVE_F, MOVE_U);
	
	result[MOVE_B][MOVE_F] = Moves(MOVE_F, MOVE_B);
	result[MOVE_B][MOVE_D] = Moves(MOVE_B, MOVE_D);
	result[MOVE_B][MOVE_U] = Moves(MOVE_B, MOVE_U);
	
	result[MOVE_U][MOVE_F] = Moves(MOVE_U, MOVE_F);
	result[MOVE_U][MOVE_B] = Moves(MOVE_U, MOVE_B);
	result[MOVE_U][MOVE_D] = Moves(MOVE_D, MOVE_U);
	
	result[MOVE_D][MOVE_F] = Moves(MOVE_D, MOVE_F);
	result[MOVE_D][MOVE_B] = Moves(MOVE_D, MOVE_B);
	result[MOVE_D][MOVE_U] = Moves(MOVE_U, MOVE_D);
	
	tx[MOVE_F] = 'f';
	tx[MOVE_B] = 'b';
	tx[MOVE_U] = 'u';
	tx[MOVE_D] = 'd';
	tx[STOP] = 's';
}

void nwbApp::mouseDrag( MouseEvent event )
{

}

void nwbApp::keyDown( KeyEvent event )
{
	int code = event.getCode();
	
	bool p0moved = !(mPlayers[0]->moves.size() < round);
	bool p1moved = !(mPlayers[1]->moves.size() < round);
	
	switch(code){
		case KeyEvent::KEY_UP:
			if(!p0moved) mPlayers[0]->moves.push_back(MOVE_U); break;
		case KeyEvent::KEY_DOWN:
			if(!p0moved) mPlayers[0]->moves.push_back(MOVE_D); break;
		case KeyEvent::KEY_RIGHT:
			if(!p0moved) mPlayers[0]->moves.push_back(MOVE_B); break;
		case KeyEvent::KEY_LEFT:
			if(!p0moved) mPlayers[0]->moves.push_back(MOVE_F); break;
			
		case KeyEvent::KEY_w:
			if(!p1moved) mPlayers[1]->moves.push_back(MOVE_U); break;
		case KeyEvent::KEY_s:
			if(!p1moved) mPlayers[1]->moves.push_back(MOVE_D); break;
		case KeyEvent::KEY_d:
			if(!p1moved) mPlayers[1]->moves.push_back(MOVE_F); break;
		case KeyEvent::KEY_a:
			if(!p1moved) mPlayers[1]->moves.push_back(MOVE_B); break;
	}
	
}

Moves nwbApp::evaluate(int p0, int p1)
{
	Moves m = Moves(STOP, STOP);
	
	if(p0 != p1)
	{
		cout << " getting result for " << p0 << "/" << p1 << "\n";
		m = result[p0][p1];
	}
	
	return m;
}
		

void nwbApp::update()
{
	bool go = true;
	for(int i = 0; i < mPlayers.size(); i++)
	{
		if(mPlayers[i]->moves.size() != round)
			go = false;
	}
	
	if(go)
	{
		
		
		int mv = round-1;
		
		Moves m = evaluate(mPlayers[0]->moves[mv], mPlayers[1]->moves[mv]);
		
		mPlayers[0]->move(m.p0);
		mPlayers[1]->move(m.p1);
		
		cout << " round " << round << "\n";
		cout << " p0 " << tx[mPlayers[0]->moves[mv]] << "\n";
		cout << " p1 " << tx[mPlayers[1]->moves[mv]] << "\n";
		cout << " p0 " << tx[m.p0] << "\n";
		cout << " p1 " << tx[m.p1] << "\n\n";
		
		
		round++;
		
		
	}
}

void nwbApp::draw()
{
	glPushMatrix();
	
	gl::translate(this->getWindowCenter());
	
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	
	for(int i = 0; i < mPlayers.size(); i++){
	
		glPushMatrix();
	
		gl::translate( Vec2f( this->getWindowWidth() * mPlayers[i]->pos.x, this->getWindowHeight() * mPlayers[i]->pos.y) );
		gl::color(mPlayers[i]->color);
		gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 5.0f);
	
		glPopMatrix();
	
	}
	
	glPopMatrix();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( nwbApp, RendererGl )
