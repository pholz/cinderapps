#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/ImageIo.h"
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
#define FLASHTIME .5f

static const bool PREMULT = false;

class Player {
public:
	Vec2f pos;
	string name;
	Color color;
	vector<int> moves;
	int dir;
	vector<Vec2f> past;
	float flash;
	gl::Texture ok;
	
	Player(Vec2f _pos, string _name, Color _color, int _dir)
	{
		pos = _pos;
		name = _name;
		color = _color;
		dir = _dir;
		flash = 0;
		
		TextLayout layout;
		layout.setFont( Font( "Helvetica", 24 ) );
		layout.setColor( Color( .1, .1, .1 ) );
		layout.clear( Color(.9,.9,.9));
		layout.addCenteredLine( std::string( "ok" ) );
		Surface8u rendered = layout.render( true, PREMULT );
		ok = gl::Texture( rendered );
	}
	
	
	void move(int mv){
		past.push_back(pos);
		
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
		else if(mv == STOP)
		{
			flash = FLASHTIME;
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
	float last;
};

void nwbApp::setup()
{
	shared_ptr<Player> p1( new Player( Vec2f(.3f,.0f), "p1", Color(.5f, 1.0f, .5f), DIR_L ) );
	shared_ptr<Player> p2( new Player( Vec2f(-.3f,.0f), "p2", Color(.5f, .5f, 1.0f), DIR_R ) );
	
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
	
	last = 0.0f;
	
	gl::enableAlphaBlending( PREMULT );
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
	float now = getElapsedSeconds();
	float dt = now - last;
	last = now;
	
	bool go = true;
	for(int i = 0; i < mPlayers.size(); i++)
	{
		if(mPlayers[i]->moves.size() != round)
			go = false;
		
		if(mPlayers[i]->flash > 0) mPlayers[i]->flash -= dt;
		if(mPlayers[i]->flash < 0) mPlayers[i]->flash = 0;
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

		for(int j = 0; j < mPlayers[i]->past.size(); j++)
		{
			glPushMatrix();
			gl::translate(Vec2f( this->getWindowWidth() * mPlayers[i]->past[j].x, this->getWindowHeight() * mPlayers[i]->past[j].y));
			gl::color(Color(.5f,.5f,1.0f));
			gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 2.0f);
			glPopMatrix();
		}
		
		glPushMatrix();
		
		gl::translate( Vec2f( this->getWindowWidth() * mPlayers[i]->pos.x, this->getWindowHeight() * mPlayers[i]->pos.y) );
		gl::color(mPlayers[i]->color);
		
		if(mPlayers[i]->flash > 0)
			gl::color(Color(mPlayers[i]->color.r + mPlayers[i]->flash, 0, 0));
		
		gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 7.0f + mPlayers[i]->flash * 5.0f);
		
		glPopMatrix();
		
		if(mPlayers[i]->moves.size() == round)
		{
			cout << "WOOOO" << endl;
			glColor3f( 1.0f, 1.0f, 1.0f );
			gl::draw( mPlayers[i]->ok, Vec2f( i == 0 ? 200 : -200, -100 ) );
			
		}
	
	}
	
	glPopMatrix();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( nwbApp, RendererGl )
