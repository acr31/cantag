/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#ifndef GLUTRENDERWINDOW_GUARD
#define GLUTRENDERWINDOW_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GL_GL_H
#error "This version has been configured without OpenGL support"
#endif

#ifndef HAVE_GL_FREEGLUT_H
#error "This version has been configured without Freeglut support"
#endif

#ifndef HAVELIB_GL
# error "This version has been configured without OpenGL support"
#endif

#ifndef HAVELIB_GLUT
# error "This version has been configured without GLUT support"
#endif

#include <iostream>
#include <map>


#include <cantag/Camera.hh>
#include <cantag/Transform.hh>
#include <cantag/Keypress.hh>

namespace Cantag {
  
    class CANTAG_EXPORT GlutRenderWindow {

    protected:
	int m_width;
	int m_height;
	float m_ratio;
	std::vector<Key::Code> m_presses;

    private:
	int m_window_id;
	static GlutRenderWindow* m_self;
    
    public:
	GlutRenderWindow(int width,int height);
	virtual ~GlutRenderWindow();
    
	/**
	 * Find the next allowable texture size which is greater than the argument
	 */
	int FindNextTextureSize(int size) const;

	void ServiceEventQueue();

	void Flush();

	const std::vector<Key::Code>& GetKeypresses() { return m_presses; }

	static void KeyboardCallback(unsigned char key, int x, int y);
	static void SpecialCallback(int key, int x,int y);
    };
}

#endif//GLUTRENDER_WINDOW_GUARD
